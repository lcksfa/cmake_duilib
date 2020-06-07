#ifndef __INCLUDE_ASYNCMODALBASE_H__
#define __INCLUDE_ASYNCMODALBASE_H__

#pragma once

class AsyncModalBase
{
private:
    /* data */
public:
    AsyncModalBase(/* args */) = default;
    ~AsyncModalBase() = default;
    virtual void SyncShowModal() = 0;
};

#endif // __INCLUDE_ASYNCMODALBASE_H__