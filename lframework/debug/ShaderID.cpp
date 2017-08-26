/**
@file ShaderID.cpp
@author t-sakai
@date 2017/25 create
*/
#include "ShaderID.h"

namespace debug
{
namespace
{
    // VS
    //-------------------------------------------------------
    const lfw::u8 shaderVS_ProceduralSpace[] =
    {
#include "shader/ProceduralSpaceVS.txt"
    };

    // PS
    //-------------------------------------------------------
    const lfw::u8 shaderPS_ProceduralSpace[] =
    {
#include "shader/ProceduralSpacePS.txt"
    };

    const lfw::u8 shaderPS_ProceduralTerrain00[] =
    {
#include "shader/ProceduralTerrain00PS.txt"
    };

}

    //-------------------------------------------------------
    //---
    //--- ShaderCompiledBytes
    //---
    //-------------------------------------------------------
    lfw::s32 ShaderCompiledBytes::getSizeVS(lfw::s32 id)
    {
        LASSERT(ShaderVS_Start<=id && id<ShaderVS_End);
        return sizeVS_[id-ShaderVS_Start];
    }

    const lfw::u8* ShaderCompiledBytes::getBytesVS(lfw::s32 id)
    {
        LASSERT(ShaderVS_Start<=id && id<ShaderVS_End);
        return bytesVS_[id-ShaderVS_Start];
    }

    const lfw::u8* ShaderCompiledBytes::bytesVS_[] =
    {
        shaderVS_ProceduralSpace,
    };

    const lfw::s32 ShaderCompiledBytes::sizeVS_[] =
    {
        sizeof(shaderVS_ProceduralSpace),
    };

    lfw::s32 ShaderCompiledBytes::getSizePS(lfw::s32 id)
    {
        LASSERT(ShaderPS_Start<=id && id<ShaderPS_End);
        return sizePS_[id-ShaderPS_Start];
    }

    const lfw::u8* ShaderCompiledBytes::getBytesPS(lfw::s32 id)
    {
        LASSERT(ShaderPS_Start<=id && id<ShaderPS_End);
        return bytesPS_[id-ShaderPS_Start];
    }

    const lfw::u8* ShaderCompiledBytes::bytesPS_[] =
    {
        shaderPS_ProceduralSpace,
        shaderPS_ProceduralTerrain00,
    };

    const lfw::s32 ShaderCompiledBytes::sizePS_[] =
    {
        sizeof(shaderPS_ProceduralSpace),
        sizeof(shaderPS_ProceduralTerrain00),
    };

    lfw::s32 ShaderCompiledBytes::getSizeCS(lfw::s32 id)
    {
        LASSERT(ShaderCS_Start<=id && id<ShaderCS_End);
        return sizeCS_[id-ShaderCS_Start];
    }

    const lfw::u8* ShaderCompiledBytes::getBytesCS(lfw::s32 id)
    {
        LASSERT(ShaderCS_Start<=id && id<ShaderCS_End);
        return bytesCS_[id-ShaderCS_Start];
    }

    const lfw::u8* ShaderCompiledBytes::bytesCS_[] =
    {
        NULL,
    };

    const lfw::s32 ShaderCompiledBytes::sizeCS_[] =
    {
        0,
    };
}
