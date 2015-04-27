/**
@file CounterRef.cpp
@author t-sakai
@date 2014/10/10 create
*/
#include "CounterRef.h"

namespace lgraphics
{
    CounterRef::CounterRef(ID3D11Counter* query)
        :AsynchronousRef(query)
    {
    }

    CounterRef Counter::create(
            CounterType type)
    {
        D3D11_COUNTER_DESC  desc;
        desc.Counter = (D3D11_COUNTER)type;
        desc.MiscFlags = 0;

        ID3D11Device* device = Graphics::getDevice().getD3DDevice();
        ID3D11Counter* counter = NULL;

        HRESULT hr = device->CreateCounter(&desc, &counter);
        if(SUCCEEDED(hr)){
            return CounterRef(counter);
        }else{
            return CounterRef();
        }
    }
}
