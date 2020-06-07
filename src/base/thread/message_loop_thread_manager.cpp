// Copyright (c) 2012, NetEase Inc. All rights reserved.
//
// a thread manager for iter-thread communicatios, etc.

#include "thread/message_loop_thread_manager.h"
#include "messageloop/message_loop.h"
#include "singleton.h"
#include "chrome/atomicops.h"

#define AUTO_MAP_LOCK() NAutoLock __l(&GetInstance()->lock_);
#define ACQUIRE_ACCESS()                                                                 \
    {                                                                                    \
        if (!AcquireAccess()) { /*DCHECK(false);*/                                       \
            return false;                                                                \
        }                                                                                \
    }

namespace ckbase
{

bool ThreadMap::AcquireAccess()
{
    MessageLoopThreadTlsData *tls = MessageLoopThread::GetTlsData();
    if (!tls || tls->managed < 1)
        return false;
    return true;
}

bool ThreadMap::RegisterThread(int self_identifier)
{
    // DCHECK(self_identifier >= 0);
    if (self_identifier < 0)
        return false;

    MessageLoopThreadTlsData *tls = MessageLoopThread::GetTlsData();
    // DCHECK(tls); // should be called by a Framework thread
    if (tls == NULL)
        return false;

    AUTO_MAP_LOCK()
    std::pair<std::map<int, MessageLoopThread *>::iterator, bool> pr =
        ThreadMap::GetInstance()->threads_.emplace(self_identifier, tls->self);
    if (!pr.second) {
        if (pr.first->second != tls->self) {
            // DCHECK(false); // another thread has registered with the same id
            return false;
        }
        // yes, it's me, check logic error
        // DCHECK(tls->managed > 0);
        // DCHECK(tls->managed_thread_id == self_identifier);
    }
    // 'self' is registered
    tls->managed++;
    tls->managed_thread_id = self_identifier;
    return true;
}

bool ThreadMap::UnregisterThread()
{
    MessageLoopThreadTlsData *tls = MessageLoopThread::GetTlsData();
    // DCHECK(tls); // should be called by a Framework thread
    // DCHECK(tls->managed > 0); // should be managed
    if (!tls || tls->managed < 1)
        return false;

    // remove from internal thread map
    // here, since tls->managed is greater than zero,
    // we must have a reference of the glabal MessageLoopThreadManager instance (see
    // RegisterThread)
    if (--tls->managed == 0) {
        AUTO_MAP_LOCK()
        std::map<int, MessageLoopThread *>::iterator iter =
            GetInstance()->threads_.find(tls->managed_thread_id);
        if (iter != GetInstance()->threads_.end())
            GetInstance()->threads_.erase(iter);
        else {
        }
        // DCHECK(false);	// logic error, we should not come here
        tls->managed_thread_id = -1;
    }

    return true;
}

// no lock
MessageLoopThread *ThreadMap::QueryThreadInternal(int identifier) const
{
    std::map<int, MessageLoopThread *>::iterator iter =
        GetInstance()->threads_.find(identifier);
    if (iter == GetInstance()->threads_.end())
        return NULL;
    return iter->second;
}

int ThreadMap::QueryThreadId(const MessageLoopThread *thread)
{
    ACQUIRE_ACCESS()
    AUTO_MAP_LOCK()
    std::map<int, MessageLoopThread *>::iterator iter;
    for (iter = GetInstance()->threads_.begin(); iter != GetInstance()->threads_.end();
         iter++) {
        if (iter->second == thread)
            return iter->first;
    }
    return -1;
}

std::shared_ptr<MessageLoopProxy> ThreadMap::GetMessageLoop(int identifier) const
{
    MessageLoopThread *thread = QueryThreadInternal(identifier);
    if (thread == NULL)
        return NULL;
    MessageLoop *message_loop = thread->message_loop();
    if (message_loop == NULL)
        return NULL;
    return message_loop->message_loop_proxy();
}

bool MessageLoopThreadManager::RegisterThread(int self_identifier)
{
    return ThreadMap::GetInstance()->RegisterThread(self_identifier);
}

bool MessageLoopThreadManager::UnregisterThread()
{
    return ThreadMap::GetInstance()->UnregisterThread();
}

int MessageLoopThreadManager::QueryThreadId(const MessageLoopThread *thread)
{
    return ThreadMap::GetInstance()->QueryThreadId(thread);
}

MessageLoopThread *MessageLoopThreadManager::CurrentThread()
{
    MessageLoopThreadTlsData *tls = MessageLoopThread::GetTlsData();
    // DCHECK(tls); // should be called by a MessageLoopThread
    // DCHECK(tls->managed > 0); // should be managed

    if (!tls || tls->managed < 1)
        return NULL;
    return tls->self;
}

bool MessageLoopThreadManager::PostTask(const StdClosure &task)
{
    MessageLoop::current()->PostTask(task);
    return true;
}

bool MessageLoopThreadManager::PostTask(int identifier, const StdClosure &task)
{
    std::shared_ptr<MessageLoopProxy> message_loop =
        ThreadMap::GetInstance()->GetMessageLoop(identifier);
    if (message_loop == NULL)
        return false;
    message_loop->PostTask(task);
    return true;
}

bool MessageLoopThreadManager::PostDelayedTask(const StdClosure &task, TimeDelta delay)
{
    MessageLoop::current()->PostDelayedTask(task, delay);
    return true;
}

bool MessageLoopThreadManager::PostDelayedTask(int identifier, const StdClosure &task,
                                               TimeDelta delay)
{
    std::shared_ptr<MessageLoopProxy> message_loop =
        ThreadMap::GetInstance()->GetMessageLoop(identifier);
    if (message_loop == NULL)
        return false;
    message_loop->PostDelayedTask(task, delay);
    return true;
}

void MessageLoopThreadManager::PostRepeatedTask(const WeakCallback<StdClosure> &task,
                                                const TimeDelta &delay, int times)
{
    StdClosure closure =
        ckbase::Bind(&MessageLoopThreadManager::RunRepeatedly, task, delay, times);
    ckbase::MessageLoopThreadManager::PostDelayedTask(closure, delay);
}

void MessageLoopThreadManager::PostRepeatedTask(int thread_id,
                                                const WeakCallback<StdClosure> &task,
                                                const TimeDelta &delay, int times)
{
    StdClosure closure =
        ckbase::Bind(&MessageLoopThreadManager::RunRepeatedlyWithThreadId, thread_id,
                     task, delay, times);
    ckbase::MessageLoopThreadManager::PostDelayedTask(thread_id, closure, delay);
}

bool MessageLoopThreadManager::PostNonNestableTask(const StdClosure &task)
{
    MessageLoop::current()->PostNonNestableTask(task);
    return true;
}

bool MessageLoopThreadManager::PostNonNestableTask(int identifier, const StdClosure &task)
{
    std::shared_ptr<MessageLoopProxy> message_loop =
        ThreadMap::GetInstance()->GetMessageLoop(identifier);
    if (message_loop == NULL)
        return false;
    message_loop->PostNonNestableTask(task);
    return true;
}

bool MessageLoopThreadManager::PostNonNestableDelayedTask(const StdClosure &task,
                                                          TimeDelta delay)
{
    MessageLoop::current()->PostNonNestableDelayedTask(task, delay);
    return true;
}

bool MessageLoopThreadManager::PostNonNestableDelayedTask(int identifier,
                                                          const StdClosure &task,
                                                          TimeDelta delay)
{
    std::shared_ptr<MessageLoopProxy> message_loop =
        ThreadMap::GetInstance()->GetMessageLoop(identifier);
    if (message_loop == NULL)
        return false;
    message_loop->PostNonNestableDelayedTask(task, delay);
    return true;
}

void MessageLoopThreadManager::RunRepeatedly(const WeakCallback<StdClosure> &task,
                                             const TimeDelta &delay, int times)
{
    if (task.Expired()) {
        return;
    }
    task();
    if (task.Expired()) {
        return;
    }
    if (times != TIMES_FOREVER) {
        times--;
    }
    if (times != 0) {
        PostRepeatedTask(task, delay, times);
    }
}

void MessageLoopThreadManager::RunRepeatedlyWithThreadId(
    int thread_id, const WeakCallback<StdClosure> &task, const TimeDelta &delay,
    int times)
{
    if (task.Expired()) {
        return;
    }
    task();
    if (task.Expired()) {
        return;
    }
    if (times != TIMES_FOREVER) {
        times--;
    }
    if (times != 0) {
        PostRepeatedTask(thread_id, task, delay, times);
    }
}

} // namespace ckbase
