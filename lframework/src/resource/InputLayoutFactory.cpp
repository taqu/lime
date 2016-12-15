/**
@file InputLayoutFactory.cpp
@author t-sakai
@date 2016/11/12 create
*/
#include "resource/InputLayoutFactory.h"
#include <lgraphics/BlobRef.h>

namespace lfw
{
    InputLayoutFactory::InputLayoutFactory()
    {
    }

    InputLayoutFactory::~InputLayoutFactory()
    {
        LayoutArray().swap(layouts_);
    }

    void InputLayoutFactory::loadDefaults()
    {
        layouts_.resize(InputLayout_Num);

        s32 bufferSize = 0;
        u8* buffer = NULL;
        lgfx::BlobPackInfo info;
        for(s32 i=0; i<InputLayout_Num; ++i){
            const u8* bytes = DefaultInputLayoutCompiledBytes::getBytes(i);
            s32 size = DefaultInputLayoutCompiledBytes::getSize(i);

            if(!lgfx::getPackBlobStatus(info, size, bytes)){
                continue;
            }
            if(bufferSize<info.uncompressedSize_){
                bufferSize = (info.uncompressedSize_+1023)&~1023;
                LDELETE_ARRAY(buffer);
                buffer = LNEW u8[bufferSize];
            }
            info.uncompressedSize_ = lgfx::unpackBlob(info.uncompressedSize_, buffer, size, bytes);
            layouts_[i] = lgfx::InputLayout::create(
                DefaultInputLayoutElement::getElements(i),
                DefaultInputLayoutElement::getNumElements(i),
                buffer,
                info.uncompressedSize_);
        }
        LDELETE_ARRAY(buffer);
    }

    void InputLayoutFactory::resize(s32 size)
    {
        LASSERT(0<=size);
        layouts_.resize(size);
    }

    bool InputLayoutFactory::set(s32 id, lgfx::InputLayoutRef& layoutRef)
    {
        LASSERT(0<=id && id<layouts_.size());
        if(!layoutRef.valid()){
            return false;
        }
        layouts_[id] = layoutRef;
        return true;
    }

    lgfx::InputLayoutRef& InputLayoutFactory::get(s32 id)
    {
        LASSERT(0<=id && id<layouts_.size());
        return layouts_[id];
    }

    //-------------------------------------------------------
    //---
    //--- DefaultInputLayoutElement
    //---
    //-------------------------------------------------------
    u32 DefaultInputLayoutElement::getNumElements(s32 id)
    {
        LASSERT(0<=id && id<InputLayout_Num);
        return numElements_[id];
    }

    D3D11_INPUT_ELEMENT_DESC* DefaultInputLayoutElement::getElements(s32 id)
    {
        LASSERT(0<=id && id<InputLayout_Num);
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


    //-------------------------------------------------------
    //---
    //--- DefaultInputLayoutCompiledBytes
    //---
    //-------------------------------------------------------
    const u8 DefaultInputLayoutCompiledBytes::bytesP[] =
    {
#include "../../shader/Layout_P.txt"
    };

    const u8 DefaultInputLayoutCompiledBytes::bytesPN[] =
    {
#include "../../shader/Layout_PN.txt"
    };

    const u8 DefaultInputLayoutCompiledBytes::bytesPU[] =
    {
#include "../../shader/Layout_PU.txt"
    };

    const u8 DefaultInputLayoutCompiledBytes::bytesPC[] =
    {
#include "../../shader/Layout_PC.txt"
    };

    const u8 DefaultInputLayoutCompiledBytes::bytesPNTB[] =
    {
#include "../../shader/Layout_PNTB.txt"
    };

    const u8 DefaultInputLayoutCompiledBytes::bytesPNU[] =
    {
#include "../../shader/Layout_PNU.txt"
    };

    const u8 DefaultInputLayoutCompiledBytes::bytesPNCU[] =
    {
#include "../../shader/Layout_PNCU.txt"
    };

    const u8 DefaultInputLayoutCompiledBytes::bytesPNTBU[] =
    {
#include "../../shader/Layout_PNTBU.txt"
    };

    const u8 DefaultInputLayoutCompiledBytes::bytesPNUBone[] =
    {
#include "../../shader/Layout_PNUBone.txt"
    };

    const u8 DefaultInputLayoutCompiledBytes::bytesPNTUBone[] =
    {
#include "../../shader/Layout_PNTUBone.txt"
    };

    s32 DefaultInputLayoutCompiledBytes::getSize(s32 id)
    {
        LASSERT(0<=id && id<InputLayout_Num);
        return size_[id];
    }

    const u8* DefaultInputLayoutCompiledBytes::getBytes(s32 id)
    {
        LASSERT(0<=id && id<InputLayout_Num);
        return bytes_[id];
    }

    const u8* DefaultInputLayoutCompiledBytes::bytes_[] =
    {
        bytesP,
        bytesPN,
        bytesPU,
        bytesPC,
        bytesPNTB,
        bytesPNU,
        bytesPNCU,
        bytesPNTBU,
        bytesPNUBone,
        bytesPNTUBone,
    };

    const s32 DefaultInputLayoutCompiledBytes::size_[] =
    {
        sizeof(bytesP),
        sizeof(bytesPN),
        sizeof(bytesPU),
        sizeof(bytesPC),
        sizeof(bytesPNTB),
        sizeof(bytesPNU),
        sizeof(bytesPNCU),
        sizeof(bytesPNTBU),
        sizeof(bytesPNUBone),
        sizeof(bytesPNTUBone),
    };

#if 0
    //-------------------------------------------------------
    //---
    //--- DefaultInputLayoutSource
    //---
    //-------------------------------------------------------
    const Char DefaultInputLayoutSource::sourceP[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "};"
        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"
    };

    const Char DefaultInputLayoutSource::sourcePN[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "float4 normal : NORMAL;"
        "};"
        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"
    };

    const Char DefaultInputLayoutSource::sourcePU[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "float2 tex0 : TEXCOORD0;"
        "};"

        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"
    };

    const Char DefaultInputLayoutSource::sourcePC[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "float4 color : COLOR;"
        "};"

        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"
    };

    const Char DefaultInputLayoutSource::sourcePNTB[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "float4 normal : NORMAL;"
        "float4 tangent : TANGENT;"
        "float4 binormal : BINORMAL;"
        "};"

        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"
    };

    const Char DefaultInputLayoutSource::sourcePNU[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "float4 normal : NORMAL;"
        "float2 tex0 : TEXCOORD0;"
        "};"

        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"
    };

    const Char DefaultInputLayoutSource::sourcePNCU[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "float4 normal : NORMAL;"
        "float4 color : COLOR;"
        "float2 tex0 : TEXCOORD0;"
        "};"

        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"

    };

    const Char DefaultInputLayoutSource::sourcePNTBU[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "float4 normal : NORMAL;"
        "float4 tangent : TANGENT;"
        "float4 binormal : BINORMAL;"
        "float2 tex0 : TEXCOORD0;"
        "};"

        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"
    };

    const Char DefaultInputLayoutSource::sourcePNUBone[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "float4 normal : NORMAL;"
        "float2 uv : TEXCOORD0;"
        "uint2 bone : BLENDINDICES;"
        "float weight : BLENDWEIGHT;"
        "};"

        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"
    };

    const Char DefaultInputLayoutSource::sourcePNTUBone[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "float4 normal : NORMAL;"
        "float4 tangent : TANGENT;"
        "float2 uv : TEXCOORD0;"
        "uint2 bone : BLENDINDICES;"
        "float weight : BLENDWEIGHT;"
        "};"

        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"
    };

    const Char* DefaultInputLayoutSource::getSource(s32 id)
    {
        LASSERT(0<=id && id<InputLayout_Num);
        return sources_[id];
    }

    const Char* DefaultInputLayoutSource::sources_[] =
    {
        sourceP,
        sourcePN,
        sourcePU,
        sourcePC,
        sourcePNTB,
        sourcePNU,
        sourcePNCU,
        sourcePNTBU,
        sourcePNUBone,
        sourcePNTUBone,
    };
#endif
}
