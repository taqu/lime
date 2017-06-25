/**
@file CounterRef.cpp
@author t-sakai
@date 2014/10/10 create
*/
#include "CounterRef.h"
#include "../../Graphics.h"

namespace lgfx
{
    CounterRef::CounterRef(ID3D11Counter* query)
        :AsynchronousRef(query)
    {
    }

    CounterRef& CounterRef::operator=(const CounterRef& rhs)
    {
        CounterRef(rhs).swap(*this);
        return *this;
    }

    CounterRef& CounterRef::operator=(CounterRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            asynchronous_ = rhs.asynchronous_;
            rhs.asynchronous_ = NULL;
        }
        return *this;
    }

    CounterRef Counter::create(
            CounterType type)
    {
        D3D11_COUNTER_DESC  desc;
        desc.Counter = (D3D11_COUNTER)type;
        desc.MiscFlags = 0;

        ID3D11Device* device = getDevice().getD3DDevice();
        ID3D11Counter* counter = NULL;

        HRESULT hr = device->CreateCounter(&desc, &counter);
        if(SUCCEEDED(hr)){
            return CounterRef(counter);
        }else{
            return CounterRef();
        }
    }
}
