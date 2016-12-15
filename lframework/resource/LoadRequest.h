#ifndef INC_LFRAMEWORK_LOADREQUEST_H__
#define INC_LFRAMEWORK_LOADREQUEST_H__
/**
@file LoadRequest.h
@author t-sakai
@date 2016/11/23 create
*/
#include "../lframework.h"
#include <lcore/intrusive_ptr.h>

namespace lfw
{
    class Loader;

    class LoadRequest
    {
    public:
        typedef lcore::intrusive_ptr<LoadRequest> pointer;

        void* operator new(lcore::lsize_t size);
        void operator delete(void* ptr);

    protected:
        LoadRequest(const LoadRequest&);
        LoadRequest& operator=(const LoadRequest&);


    };
}
#endif //INC_LFRAMEWORK_LOADREQUEST_H__
