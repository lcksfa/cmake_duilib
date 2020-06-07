#ifndef __INCLUDE_MSG_LOOP_THREAD_FACTORY_H__
#define __INCLUDE_MSG_LOOP_THREAD_FACTORY_H__
#pragma once

#include "thread\message_loop_thread.h"
#include "thread\message_loop_thread_manager.h"

enum class ThreadId
{
    Begin = -1, //线程ID定义开始
    UI,         // UI线程（主线程）
    DB,         //写DB线程
    Misc,       //杂事处理线程（比如：打开url）
    UIHelper,   // UI辅助线程
    UserDefine, //用户自定义应用线程

    End, //线程ID定义结束;
};

class MessageLoopThreadFactory : public ckbase::MessageLoopThread
{
public:
    MessageLoopThreadFactory(ThreadId id)
        : MessageLoopThread(GetThreadNameById(id))
        , m_id(id)
    {
    }

    void Init() override
    {
        ckbase::MessageLoopThreadManager::RegisterThread(static_cast<int>(m_id));
    }
    void Cleanup() override
    {
        ckbase::MessageLoopThreadManager::UnregisterThread();
    }

private:
    const char *GetThreadNameById(ThreadId threadId)
    {
        switch (threadId) {
        case ThreadId::UI:
            return "T:UI";
        case ThreadId::DB:
            return "T:DB";
        case ThreadId::Misc:
            return "T:Miscellaneous";
        }
        return "";
    }

private:
    ThreadId m_id;
};
#endif // __INCLUDE_MSG_LOOP_THREAD_FACTORY_H__