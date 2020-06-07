#include "closure\closure.h"

inline bool PostInnerTask(ThreadId id, const StdClosure &closure)
{
    return ckbase::MessageLoopThreadManager::PostTask(static_cast<int>(id), closure);
}

inline void PostInnerRepeatTask(ThreadId id, ckbase::WeakCallback<StdClosure> closure,
                                const ckbase::TimeDelta &delay, int times)
{
    return ckbase::MessageLoopThreadManager::PostRepeatedTask(static_cast<int>(id),
                                                              closure, delay, times);
}

void Post2UI(const StdClosure &closure)
{
    PostInnerTask(ThreadId::UI, closure);
}

void Post2GlobalMisc(const StdClosure &closure)
{
    PostInnerTask(ThreadId::Misc, closure);
}

void Post2Database(const StdClosure &closure)
{
    PostInnerTask(ThreadId::DB, closure);
}

void PostRepeatedTask2UI(const ckbase::WeakCallback<StdClosure> &task,
                         const ckbase::TimeDelta &delay, int times)
{
    PostInnerRepeatTask(ThreadId::UI, task, delay, times);
}

void PostRepeatedTask2GlobalMisc(const ckbase::WeakCallback<StdClosure> &task,
                                 const ckbase::TimeDelta &delay, int times)
{
    PostInnerRepeatTask(ThreadId::Misc, task, delay, times);
}

void Post2UIHelp(const StdClosure &closure)
{
    PostInnerTask(ThreadId::UIHelper, closure);
}

void Post2UserDefine(const StdClosure &closure)
{
    PostInnerTask(ThreadId::UserDefine, closure);
}

std::string GetCurrentMessageLoopThreadName()
{
    assert(ckbase::MessageLoopThreadManager::CurrentThread() != nullptr);
    return ckbase::MessageLoopThreadManager::CurrentThread()->thread_name();
}
