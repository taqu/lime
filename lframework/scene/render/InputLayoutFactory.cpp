/**
@file InputLayoutFactory.cpp
@author t-sakai
@date 2014/12/09 create
*/
#include "InputLayoutFactory.h"
#include <lcore/Search.h>
#include <lgraphics/lgraphicsAPIInclude.h>
#include <lgraphics/api/ShaderRef.h>

namespace lscene
{
namespace lrender
{
    InputLayoutFactory InputLayoutFactory::instance_;

    void InputLayoutFactory::initialize()
    {
        instance_.initImpl();
    }

    void InputLayoutFactory::terminate()
    {
        instance_.termImpl();
    }

    InputLayoutFactory::InputLayoutFactory()
    {
    }

    InputLayoutFactory::~InputLayoutFactory()
    {
    }

    void InputLayoutFactory::initImpl()
    {
        termImpl();
    }

    void InputLayoutFactory::termImpl()
    {
        LayoutVector().swap(layouts_);
    }

    bool InputLayoutFactory::add(s32 id, lgraphics::InputLayoutRef& layoutRef)
    {
        lgraphics::InputLayoutRef* layout = find(id);
        if(NULL != layout){
            return false;
        }
        //lgraphics::ShaderDecompresser decompresser(size, data);
        //u32 decompressedSize = decompresser.getDecompressedSize();
        //if(bufferSize_<decompressedSize){
        //    LSCENE_FREE(buffer_);
        //    bufferSize_ = decompressedSize;
        //    buffer_ = static_cast<u8*>( LSCENE_MALLOC(bufferSize_) );
        //}
        //decompressedSize = decompresser.decompress(buffer_);

        //lgraphics::InputLayoutRef layoutRef = lgraphics::InputLayout::create(elements, numElements, buffer_, decompressedSize);

        //ë}ì¸É\Å[Ég
        s32 size = layouts_.size();
        layouts_.resize(size+1);
        s32 index=0;
        for(s32 i=0; i<size; ++i){
            if(layouts_[i].id_<id){
                index = i+1;
            }
        }

        for(s32 i=size; index<i; --i){
            layouts_[i] = layouts_[i-1];
        }
        layouts_[index].id_ = id;
        layouts_[index].layout_ = layoutRef;

        return true;
    }

    lgraphics::InputLayoutRef* InputLayoutFactory::find(s32 id)
    {
        s32 index = lcore::binarySearchIndex(layouts_.size(), layouts_.begin(), id, CompFunc());
        return (0<=index)? &layouts_[index].layout_ : NULL;
    }

    //-------------------------------------------------------
    //---
    //--- DefaultInputLayoutElement
    //---
    //-------------------------------------------------------
    u32 DefaultInputLayoutElement::getNumElements(s32 id)
    {
        LASSERT(0<=id && id<Layout_Num);
        return numElements_[id];
    }

    D3D11_INPUT_ELEMENT_DESC* DefaultInputLayoutElement::getElements(s32 id)
    {
        LASSERT(0<=id && id<Layout_Num);
        return elements_[id];
    }

    D3D11_INPUT_ELEMENT_DESC DefaultInputLayoutElement::layoutP[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    D3D11_INPUT_ELEMENT_DESC DefaultInputLayoutElement::layoutPN[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    D3D11_INPUT_ELEMENT_DESC DefaultInputLayoutElement::layoutPU[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R16G16_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    D3D11_INPUT_ELEMENT_DESC DefaultInputLayoutElement::layoutPC[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    D3D11_INPUT_ELEMENT_DESC DefaultInputLayoutElement::layoutPNTB[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BINORMAL", 0, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    D3D11_INPUT_ELEMENT_DESC DefaultInputLayoutElement::layoutPNU[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R16G16_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    D3D11_INPUT_ELEMENT_DESC DefaultInputLayoutElement::layoutPNCU[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R16G16_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    D3D11_INPUT_ELEMENT_DESC DefaultInputLayoutElement::layoutPNTBU[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BINORMAL", 0, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R16G16_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    D3D11_INPUT_ELEMENT_DESC DefaultInputLayoutElement::layoutPNUBone[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R16G16_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BLENDINDICES", 0, DXGI_FORMAT_R16G16_UINT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BLENDWEIGHT", 0, DXGI_FORMAT_R16G16_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    D3D11_INPUT_ELEMENT_DESC DefaultInputLayoutElement::layoutPNTUBone[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R16G16_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BLENDINDICES", 0, DXGI_FORMAT_R16G16_UINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BLENDWEIGHT", 0, DXGI_FORMAT_R16G16_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    u32 DefaultInputLayoutElement::numElements_[] =
    {
        1,
        2,
        2,
        2,
        4,
        3,
        4,
        5,
        5,
        6,
    };

    D3D11_INPUT_ELEMENT_DESC* DefaultInputLayoutElement::elements_[] =
    {
        layoutP,
        layoutPN,
        layoutPU,
        layoutPC,
        layoutPNTB,
        layoutPNU,
        layoutPNCU,
        layoutPNTBU,
        layoutPNUBone,
        layoutPNTUBone,
    };
}
}
