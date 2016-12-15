/**
@file InputLayoutRef.cpp
@author t-sakai
@date 2012/07/15 create
*/
#include "InputLayoutRef.h"

#include <D3Dcompiler.h>
#include "../../Graphics.h"

namespace lgfx
{
    InputLayoutRef::InputLayoutRef(const InputLayoutRef& rhs)
        :layout_(rhs.layout_)
    {
        if(NULL != layout_){
            layout_->AddRef();
        }
    }

    void InputLayoutRef::destroy()
    {
        LDXSAFE_RELEASE(layout_);
    }

    void InputLayoutRef::attach(ContextRef& context)
    {
        context.setInputLayout(layout_);
    }

    InputLayoutRef InputLayout::create(
        D3D11_INPUT_ELEMENT_DESC* elements,
        u32 numElements,
        const void* data,
        u32 size)
    {
        LASSERT(NULL != elements);
        LASSERT(NULL != data);

        ID3D11Device* device = getDevice().getD3DDevice();

        ID3D11InputLayout* layout = NULL;
        HRESULT hr = device->CreateInputLayout(
            elements,
            numElements,
            data,
            size,
            &layout);

        if(FAILED(hr)){
            return InputLayoutRef();
        }
        return InputLayoutRef(layout);
    }
}
