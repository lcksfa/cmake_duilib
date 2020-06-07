#ifndef BASE_AT_EXIT_H_
#define BASE_AT_EXIT_H_
#pragma once

#include <stack>

#include "base_export.h"
#include "base_types.h"
#include "closure/weakcallbacksupport.hpp"
#include "synchronization/lock.h"

namespace ckbase
{

// This class provides a facility similar to the CRT atexit(), except that
// we control when the callbacks are executed. Under Windows for a DLL they
// happen at a really bad time and under the loader lock. This facility is
// mostly used by base::Singleton.
//
// The usage is simple. Early in the main() or WinMain() scope create an
// OnExitManager object on the stack:
// int main(...) {
//    base::OnExitManager exit_manager;
//
// }
// When the exit_manager object goes out of scope, all the registered
// callbacks and singleton destructors will be called.

class BASE_EXPORT OnExitManager
{
public:
    typedef void (*AtExitCallbackType)(void *);

    OnExitManager();

    // The dtor calls all the registered callbacks. Do not try to register more
    // callbacks after this point.
    ~OnExitManager();

    // Registers the specified function to be called at exit. The prototype of
    // the callback function is void func(void*).
    static void RegisterCallback(AtExitCallbackType func, void *param);

    // Registers the specified task to be called at exit.
    static void RegisterTask(StdClosure task);

    // Calls the functions registered with RegisterCallback in LIFO order. It
    // is possible to register new callbacks after calling this function.
    static void ProcessCallbacksNow();

protected:
    // This constructor will allow this instance of OnExitManager to be created
    // even if one already exists.  This should only be used for testing!
    // AtExitManagers are kept on a global stack, and it will be removed during
    // destruction.  This allows you to shadow another OnExitManager.
    explicit OnExitManager(bool shadow);

private:
    ckbase::NLock lock_;
    std::stack<StdClosure> stack_;
    OnExitManager *next_manager_; // Stack of managers to allow shadowing.

    DISALLOW_COPY_AND_ASSIGN(OnExitManager);
};

} // namespace ckbase

#endif // BASE_AT_EXIT_H_
