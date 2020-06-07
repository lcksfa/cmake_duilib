#ifndef __INCLUDE_ASYNCMODALRUNNERTHREAD_H__
#define __INCLUDE_ASYNCMODALRUNNERTHREAD_H__

#pragma once
#include <memory>
#include <list>
#include "thread\basic_thread.h"
#include "AsyncModalBase.h"
#include "synchronization/waitable_event.h"
#include "closure/weakcallbacksupport.hpp"
#include "singleton.h"

class AsyncModalRunnerThread : public ckbase::Thread
{
public:
    class Delegate
    {
    public:
        virtual void OnThreadWillExit(AsyncModalRunnerThread *runner) = 0;
    };
    // Once this method is called the runner will take
    // the ownership of the dialog
    bool DoModal(AsyncModalBase *dlg);
    void CancelModalThenExit();
    AsyncModalRunnerThread(Delegate *delegate);
    virtual ~AsyncModalRunnerThread();

private:
    // template <class _Ty> friend class std::_Ref_count_obj;

    friend class AsyncModalRunnerManager;
    friend class std::shared_ptr<AsyncModalRunnerThread>;
    friend class std::_Ref_count<AsyncModalRunnerThread>;

    void Run();

    bool is_running_;
    bool quit_posted_;
    Delegate *delegate_;
    ckbase::WaitableEvent event_;
    std::unique_ptr<AsyncModalBase> modal_dlg_;
};

class AsyncModalRunnerThreadManager : public AsyncModalRunnerThread::Delegate,
                                      public ckbase::SupportWeakCallback
{
public:
    SINGLETON_DEFINE(AsyncModalRunnerThreadManager);

    // Once this method is called the runner will take
    // the ownership of the dialog
    bool DoModal(AsyncModalBase *dlg);
    void CancelAllThreads();

private:
    AsyncModalRunnerThreadManager();
    virtual ~AsyncModalRunnerThreadManager();

    void OnThreadWillExit(AsyncModalRunnerThread *runner);
    void Deregister(AsyncModalRunnerThread *runner);

    ckbase::NLock threads_lock_;
    std::list<std::shared_ptr<AsyncModalRunnerThread>> runners_;
};

#endif // __INCLUDE_ASYNCMODALRUNNERTHREAD_H__