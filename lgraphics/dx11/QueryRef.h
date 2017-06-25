#ifndef INC_LGRAPHICS_DX11_QUERYREF_H__
#define INC_LGRAPHICS_DX11_QUERYREF_H__
/**
@file QueryRef.h
@author t-sakai
@date 2014/10/10 create
*/
#include "AsynchronousRef.h"

struct ID3D11Query;

namespace lgfx
{
    //--------------------------------------------------------
    //---
    //--- QueryRef
    //---
    //--------------------------------------------------------
    class QueryRef : public AsynchronousRef
    {
    public:
        typedef ID3D11Query element_type;
        typedef ID3D11Query* pointer_type;

        QueryRef()
        {}
        QueryRef(const QueryRef& rhs)
            :AsynchronousRef(rhs)
        {}

        QueryRef(QueryRef&& rhs)
            :AsynchronousRef(rhs)
        {}

        ~QueryRef()
        {}

        QueryRef& operator=(const QueryRef& rhs);
        QueryRef& operator=(QueryRef&& rhs);
    private:
        friend class Query;

        explicit QueryRef(pointer_type query);
    };

    class Query
    {
    public:
        static QueryRef create(QueryType type, QueryMiscFlag flag);
    };
}
#endif //INC_LGRAPHICS_DX11_QUERYREF_H__
