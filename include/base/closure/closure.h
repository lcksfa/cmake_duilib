#ifndef __INCLUDE_CLOSURE_H__
#define __INCLUDE_CLOSURE_H__
#pragma once
#include "weakcallbacksupport.hpp"
#include "thread/message_loop_thread_manager.h"
#include "msg_loop_thread_factory.hpp"

void Post2UI(const StdClosure &closure);
void Post2GlobalMisc(const StdClosure &closure);
void Post2Database(const StdClosure &closure);
void PostRepeatedTask2UI(const ckbase::WeakCallback<StdClosure> &task,
                                 const ckbase::TimeDelta &delay,int times);

void PostRepeatedTask2GlobalMisc(const ckbase::WeakCallback<StdClosure> &task,
                                 const ckbase::TimeDelta &delay,int times);
void Post2UIHelp(const StdClosure &closure);
void Post2UserDefine(const StdClosure &closure);
std::string GetCurrentMessageLoopThreadName();

template <typename T1, typename T2>
bool Await(ThreadId id, const std::function<T1> &task, const std::function<T2> &reply)
{
    int threadIdentifier = static_cast<int>(id);
    return ckbase::MessageLoopThreadManager::Await(threadIdentifier, task, reply);
}


#endif // __INCLUDE_CLOSURE_H__