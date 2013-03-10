/**
@file InputLayoutRef.cpp
@author t-sakai
@date 2012/07/15 create
*/
#include "InputLayoutRef.h"

#include "../../lgraphicsAPIInclude.h"
#include <D3Dcompiler.h>
#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"

namespace lgraphics
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
        SAFE_RELEASE(layout_);
    }

    InputLayoutRef InputLayout::create(
        D3D11_INPUT_ELEMENT_DESC* elements,
        u32 numElements,
        const void* data,
        u32 size)
    {
        LASSERT(NULL != elements);
        LASSERT(NULL != data);

        ID3D11Device* device = Graphics::getDevice().getD3DDevice();

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
