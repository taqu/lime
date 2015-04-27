#ifndef INC_LSCENE_MIXIN_REFERENCECOUNTED_H__
#define INC_LSCENE_MIXIN_REFERENCECOUNTED_H__
/**
@file ReferenceCounted.h
@author t-sakai
@date 2014/11/03 create
*/
#include "../lscene.h"

namespace lscene
{
namespace mixin
{
    template<class Base>
    class ReferenceCounted : public Base
    {
    public:
        inline void addRef();
        inline void release();
        inline s32 getReferenceCount() const;
    protected:
        ReferenceCounted();
        virtual ~ReferenceCounted();

        s32 refCount_;
    };

    template<class Base>
    ReferenceCounted<Base>::ReferenceCounted()
        :refCount_(0)
    {
    }

    template<class Base>
    ReferenceCounted<Base>::~ReferenceCounted()
    {
    }

    template<class Base>
    inline void ReferenceCounted<Base>::addRef()
    {
        ++refCount_;
    }

    template<class Base>
    inline void ReferenceCounted<Base>::release()
    {
        if(0 == --refCount_){
            LSCENE_DELETE_NO_NULL(this);
        }
    }

    template<class Base>
    inline s32 ReferenceCounted<Base>::getReferenceCount() const
    {
        return refCount_;
    }
}
}
#endif //INC_LSCENE_MIXIN_REFERENCECOUNTED_H__
