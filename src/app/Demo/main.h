#pragma once

#include <memory>
#include "closure\msg_loop_thread_factory.hpp"

class MainThread : public MessageLoopThreadFactory
{
public:
    MainThread()
        : MessageLoopThreadFactory(ThreadId::UI)
        , misc_thread_(std::make_unique<MessageLoopThreadFactory>(ThreadId::Misc))

    {
    }
    virtual ~MainThread() = default;
    bool ResourceInit(HINSTANCE hInstance);

private:
    virtual void Init() final;

    virtual void Cleanup() final;

private:
    std::unique_ptr<MessageLoopThreadFactory>
        misc_thread_; // Specially dealing with chores thread
    // std::unique_ptr<MessageLoopThreadFactory>
    // db_thread_; // Specially dealing with db operations
};
