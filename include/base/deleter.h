// a implementation of scoped handle which
// ensures the safe use of standard handles

#ifndef BASE_MEMORY_SCOPED_STD_HANDLE_H_
#define BASE_MEMORY_SCOPED_STD_HANDLE_H_

#include <stdlib.h>
#include <stdio.h>
#include "base_config.h"
#include "base_export.h"
#include "base_types.h"

namespace ckbase
{

// This class wraps the c library function free() in a class that can be
// passed as a template argument to scoped_ptr_malloc below.
class BASE_EXPORT DeleterFree
{
public:
    inline void operator()(void *x) const
    {
        free(x);
    }
};

class BASE_EXPORT DeleterFileClose
{
public:
    inline void operator()(FILE *x) const
    {
        fclose(x);
    }
};

} // namespace ckbase

#endif // BASE_MEMORY_SCOPED_STD_HANDLE_H_
