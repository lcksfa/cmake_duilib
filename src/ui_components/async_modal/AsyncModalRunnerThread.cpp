#include "async_modal\AsyncModalRunnerThread.h"
#include "thread/message_loop_thread_manager.h"
#include "closure\msg_loop_thread_factory.hpp"
#include <algorithm>

static const char kModalThreadName[] = "AsyncModalRunnerThread";

AsyncModalRunnerThread::AsyncModalRunnerThread(Delegate *delegate)
    : event_(true, false)
    , is_running_(false)
    , quit_posted_(false)
    , delegate_(delegate)
{
}

AsyncModalRunnerThread::~AsyncModalRunnerThread()
{
    CancelModalThenExit();
}

void AsyncModalRunnerThread::CancelModalThenExit()
{
    if (!is_running_ || quit_posted_)
        return;
    quit_posted_ = true;
    PostThreadMessage(thread_id(), WM_QUIT, 0, 0);
}

bool AsyncModalRunnerThread::DoModal(AsyncModalBase *dlg)
{
    assert(!is_running_);
    modal_dlg_.reset(dlg);
    if (!Create())
        return false;
    is_running_ = event_.Wait();
    return is_running_;
}

void AsyncModalRunnerThread::Run()
{
#ifndef NDEBUG
#if defined(OS_WIN) && defined(COMPILER_MSVC)
    ckbase::SetThreadName(GetCurrentThreadId(), kModalThreadName);
#endif
#endif

    event_.Signal();
    if (modal_dlg_ != nullptr) {
        modal_dlg_->SyncShowModal();
    }
    if (delegate_ != nullptr) {
        delegate_->OnThreadWillExit(this);
    }
}

AsyncModalRunnerThreadManager::AsyncModalRunnerThreadManager()
{
}

AsyncModalRunnerThreadManager::~AsyncModalRunnerThreadManager()
{
    CancelAllThreads();
}

bool AsyncModalRunnerThreadManager::DoModal(AsyncModalBase *dlg)
{
    if (dlg == nullptr)
        return false;
    auto runner = std::make_shared<AsyncModalRunnerThread>(this);
    /*std::shared_ptr<AsyncModalRunner> runner = nullptr;
    runner.reset(new AsyncModalRunner(this));*/
    {
        ckbase::NAutoLock lock(&threads_lock_);
        runners_.push_back(runner);
    }
    if (runner->DoModal(std::move(dlg)))
        return true;
    return false;
}

void AsyncModalRunnerThreadManager::CancelAllThreads()
{
    std::list<std::shared_ptr<AsyncModalRunnerThread>> threads;
    {
        ckbase::NAutoLock lock(&threads_lock_);
        threads.swap(runners_);
    }
    // First, we notify the modal dialogs to quit the modal loop
    std::for_each(threads.begin(), threads.end(),
                  [](std::shared_ptr<AsyncModalRunnerThread> runner) {
                      runner->CancelModalThenExit();
                  });
    // Then, we wait util all modal runner exited
    std::for_each(
        threads.begin(), threads.end(),
        [](std::shared_ptr<AsyncModalRunnerThread> runner) { runner->Close(); });
}

void AsyncModalRunnerThreadManager::OnThreadWillExit(AsyncModalRunnerThread *runner)
{
    ckbase::MessageLoopThreadManager::PostTask(
        static_cast<int>(ThreadId::UI),
        ckbase::Bind(&AsyncModalRunnerThreadManager::Deregister, this, runner));
}

void AsyncModalRunnerThreadManager::Deregister(AsyncModalRunnerThread *runner)
{
    std::shared_ptr<AsyncModalRunnerThread> found;
    {
        ckbase::NAutoLock lock(&threads_lock_);
        for (std::list<std::shared_ptr<AsyncModalRunnerThread>>::const_iterator iter =
                 runners_.begin();
             iter != runners_.end(); iter++) {
            if ((*iter).get() == runner) {
                found = *iter;
                runners_.erase(iter);
                break;
            }
        }
    }
    // If the runner is found,
    // it will be destroyed out of the scope of the lock
}
