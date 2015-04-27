#ifndef INC_LGRAPHICS_DX11_COUNTERREF_H__
#define INC_LGRAPHICS_DX11_COUNTERREF_H__
/**
@file CounterRef.h
@author t-sakai
@date 2014/10/10 create
*/

#include "AsynchronousRef.h"

struct ID3D11Counter;

namespace lgraphics
{
    //--------------------------------------------------------
    //---
    //--- CounterRef
    //---
    //--------------------------------------------------------
    class CounterRef : public AsynchronousRef
    {
    public:
        CounterRef()
        {}
        CounterRef(const CounterRef& rhs)
            :AsynchronousRef(rhs)
        {}

        ~CounterRef()
        {}
    private:
        friend class Counter;

        explicit CounterRef(ID3D11Counter* counter);
    };

    class Counter
    {
    public:
        static CounterRef create(CounterType type);
    };
}
#endif //INC_LGRAPHICS_DX11_COUNTERREF_H__