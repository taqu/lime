#ifndef INC_LGRAPHICS_DX11_PREDICATEREF_H__
#define INC_LGRAPHICS_DX11_PREDICATEREF_H__
/**
@file PredicateRef.h
@author t-sakai
@date 2014/10/10 create
*/
#include "AsynchronousRef.h"

struct ID3D11Predicate;

namespace lgfx
{
    //--------------------------------------------------------
    //---
    //--- PredicateRef
    //---
    //--------------------------------------------------------
    class PredicateRef : public AsynchronousRef
    {
    public:
        typedef ID3D11Predicate element_type;
        typedef ID3D11Predicate* pointer_type;

        PredicateRef()
        {}
        PredicateRef(const PredicateRef& rhs)
            :AsynchronousRef(rhs)
        {}

        ~PredicateRef()
        {}
    private:
        friend class Predicate;

        explicit PredicateRef(pointer_type predicate);
    };

    class Predicate
    {
    public:
        static PredicateRef create(
            QueryType type,
            QueryMiscFlag flag);
    };
}
#endif //INC_LGRAPHICS_DX11_PREDICATEREF_H__
