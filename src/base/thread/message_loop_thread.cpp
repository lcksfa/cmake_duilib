// Copyright (c) 2011, NetEase Inc. All rights reserved.

// Thread with framework(message loop)

#include "thread/message_loop_thread.h"
#include "base_types.h"
#include "macros.h"
#include "messageloop/lazy_instance.h"

namespace ckbase
{

// We use this thread-local variable to record whether or not a thread exited
// because its Stop method was called.  This allows us to catch cases where
// MessageLoop::Quit() is called directly, which is unexpected when using a
// Thread to setup and run a MessageLoop.
ckbase::LazyInstance<ckbase::ThreadLocalPointer<MessageLoopThreadTlsData>> lazy_tls_data;

MessageLoopThread::MessageLoopThread(const char *name)
    : started_(false)
    , stopping_(false)
    , loop_type_(MessageLoop::kDefaultMessageLoop)
    , message_loop_(NULL)
    , event_(false, false)
    , name_(name)
{
}

MessageLoopThread::~MessageLoopThread()
{
    Stop();
}

bool MessageLoopThread::Start()
{
    return StartWithLoop(MessageLoop::kDefaultMessageLoop);
}

#if defined(OS_WIN)
bool MessageLoopThread::StartWithLoop(const MessageLoop::Type type,
                                      Dispatcher *dispatcher)
#else
bool MessageLoopThread::StartWithLoop(const MessageLoop::Type type)
#endif
{
    if (message_loop_ != NULL)
        return false;

    loop_type_ = type;
#if defined(OS_WIN)
    dispatcher_ = dispatcher;
#endif

    if (Create() == false)
        return false;

    // Wait for the thread to start and initialize message_loop_
    event_.Wait();

    started_ = true;
    return true;
}

#if defined(OS_WIN)
bool MessageLoopThread::StartWithLoop(CustomMessageLoopFactory *factory,
                                      Dispatcher *dispatcher)
#else
bool MessageLoopThread::StartWithLoop(CustomMessageLoopFactory *factory)
#endif
{
    if (factory == NULL)
        return false;

    factory_.reset(factory);
    loop_type_ = MessageLoop::kCustomMessageLoop;
#if defined(OS_WIN)
    dispatcher_ = dispatcher;
#endif

    if (!Create())
        return false;

    // Wait for the thread to start and initialize message_loop_
    event_.Wait();

    started_ = true;

    return true;
}

#if defined(OS_WIN)
void MessageLoopThread::RunOnCurrentThreadWithLoop(const MessageLoop::Type type,
                                                   Dispatcher *dispatcher)
#else
void MessageLoopThread::RunOnCurrentThreadWithLoop(const MessageLoop::Type type)
#endif
{
    loop_type_ = type;
    started_ = true;
#if defined(OS_WIN)
    dispatcher_ = dispatcher;
#endif
    Run();
}

void MessageLoopThread::Stop()
{
    if (!thread_was_started())
        return;

    StopSoon();

    // Wait for the thread to exit.
    Close();

    started_ = false;
    stopping_ = false;
}

void MessageLoopThread::StopSoon()
{
    if (stopping_ || !message_loop_)
        return;

    stopping_ = true;
    message_loop_->PostNonNestableTask(
        ckbase::Bind(&MessageLoopThread::DoStopSoon, this));
}

void MessageLoopThread::DoStopSoon()
{
    MessageLoop::current()->Quit();
    SetThreadWasQuitProperly(true);
}

void MessageLoopThread::Run()
{
#ifndef NDEBUG
#if defined(OS_WIN) && defined(COMPILER_MSVC)
    SetThreadName(GetCurrentThreadId(), name_.c_str());
#endif
#endif

    InitTlsData(this);
    SetThreadWasQuitProperly(false);
    {
        ThreadId thread_id = Thread::CurrentId();
        set_thread_id(thread_id);

        // The message loop for this thread.
        MessageLoop *message_loop;
        if (loop_type_ == MessageLoop::kCustomMessageLoop)
            message_loop = factory_->CreateMessageLoop();
        else {
            if (loop_type_ == MessageLoop::kIOMessageLoop)
                message_loop = new IOMessageLoop;
#if defined(OS_WIN)
            else if (loop_type_ == MessageLoop::kUIMessageLoop)
                message_loop = new UIMessageLoop;
#endif
            else
                message_loop = new MessageLoop;
        }
        message_loop_ = message_loop;

        // Let the thread do extra initialization.
        // Let's do this before signaling we are started.
        Init();

        event_.Signal();

#if defined(OS_WIN)
        message_loop_->RunWithDispatcher(dispatcher_);
#else
        message_loop_->Run();
#endif // OS_WIN

        // Let the thread do extra cleanup.
        Cleanup();

        // DCHECK(GetThreadWasQuitProperly());

        // We can't receive messages anymore.
        if (loop_type_ != MessageLoop::kCustomMessageLoop)
            delete message_loop_;
        else {
            delete message_loop_;
            factory_.reset();
        }
        message_loop_ = NULL;
    }
    set_thread_id(kInvalidThreadId);
    {
        MessageLoopThreadTlsData *tls = GetTlsData();
        if (tls != NULL) {
        }
        // DCHECK(tls->managed == 0); // you must call
        // MessageLoopThreadManager::UnregisterThread before come there
    }
    FreeTlsData();
}

void MessageLoopThread::InitTlsData(MessageLoopThread *self)
{
    MessageLoopThreadTlsData *tls = GetTlsData();
    // DCHECK(tls == NULL);
    if (tls != NULL)
        return;
    tls = new MessageLoopThreadTlsData;
    tls->self = self;
    tls->managed = 0;
    tls->managed_thread_id = -1;
    tls->quit_properly = false;
    tls->custom_data = NULL;
    lazy_tls_data.Pointer()->Set(tls);
}

void MessageLoopThread::FreeTlsData()
{
    MessageLoopThreadTlsData *tls = GetTlsData();
    // DCHECK(tls != NULL);
    if (tls == NULL)
        return;
    lazy_tls_data.Pointer()->Set(NULL);
    delete tls;
}

MessageLoopThreadTlsData *MessageLoopThread::GetTlsData()
{
    return lazy_tls_data.Pointer()->Get();
}

bool MessageLoopThread::GetThreadWasQuitProperly()
{
    MessageLoopThreadTlsData *tls = GetTlsData();
    // DCHECK(tls != NULL);
    if (tls == NULL)
        return false;
    return tls->quit_properly;
}

void MessageLoopThread::SetThreadWasQuitProperly(bool flag)
{
    MessageLoopThreadTlsData *tls = GetTlsData();
    // DCHECK(tls != NULL);
    if (tls == NULL)
        return;
    tls->quit_properly = flag;
}

MessageLoopThread *MessageLoopThread::current()
{
    MessageLoopThreadTlsData *tls = GetTlsData();
    // DCHECK(tls != NULL);
    if (tls == NULL)
        return NULL;
    return tls->self;
}

int MessageLoopThread::GetManagedThreadId()
{
    MessageLoopThreadTlsData *tls = GetTlsData();
    // DCHECK(tls != NULL);
    if (tls == NULL)
        return -1;
    return tls->managed_thread_id;
}

void *MessageLoopThread::GetCustomTlsData()
{
    MessageLoopThreadTlsData *tls = GetTlsData();
    // DCHECK(tls != NULL);
    if (tls == NULL)
        return NULL;
    return tls->custom_data;
}

void MessageLoopThread::SetCustomTlsData(void *data)
{
    MessageLoopThreadTlsData *tls = GetTlsData();
    // DCHECK(tls != NULL);
    if (tls == NULL)
        return;
    tls->custom_data = data;
}

} // namespace ckbase