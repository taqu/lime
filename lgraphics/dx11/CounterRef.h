#ifndef INC_LGRAPHICS_DX11_COUNTERREF_H_
#define INC_LGRAPHICS_DX11_COUNTERREF_H_
/**
@file CounterRef.h
@author t-sakai
@date 2014/10/10 create
*/

#include "AsynchronousRef.h"

struct ID3D11Counter;

namespace lgfx
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

        CounterRef(CounterRef&& rhs)
            :AsynchronousRef(rhs)
        {}

        ~CounterRef()
        {}

        CounterRef& operator=(const CounterRef& rhs);
        CounterRef& operator=(CounterRef&& rhs);
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
#endif //INC_LGRAPHICS_DX11_COUNTERREF_H_
