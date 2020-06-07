// Copyright (c) 2011, NetEase Inc. All rights reserved.

// This file implements some useful thread local template data structures for Windows

#if defined(OS_WIN)
#include "thread/thread_local.h"

#include <assert.h>
#include <windows.h>

namespace ckbase
{

namespace internal
{

// static
void ThreadLocalPlatform::AllocateSlot(SlotType &slot)
{
    slot = ::TlsAlloc();
    assert(slot != TLS_OUT_OF_INDEXES);
}

// static
void ThreadLocalPlatform::FreeSlot(SlotType &slot)
{
    if (!::TlsFree(slot)) {
        assert(false);
    }
}

// static
void *ThreadLocalPlatform::GetValueFromSlot(SlotType &slot)
{
    return ::TlsGetValue(slot);
}

// static
void ThreadLocalPlatform::SetValueInSlot(SlotType &slot, void *value)
{
    if (!::TlsSetValue(slot, value)) {
        assert(false);
    }
}

} // namespace internal

} // namespace ckbase

#endif // OS_WIN
