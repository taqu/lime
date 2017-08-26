#ifndef INC_DEBUG_SHADERID_H__
#define INC_DEBUG_SHADERID_H__
/**
@file ShaderID.h
@author t-sakai
@date 2017/25 create
*/
#include "../resource/ShaderID.h"

namespace debug
{
    enum ShaderVS
    {
        ShaderVS_Start = lfw::ShaderVS_User,
        ShaderVS_ProceduralSpace = lfw::ShaderVS_User,
        ShaderVS_End,
    };

    enum ShaderPS
    {
        ShaderPS_Start = lfw::ShaderPS_User,
        ShaderPS_ProceduralSpace = lfw::ShaderPS_User,
        ShaderPS_ProceduralTerrain00,
        ShaderPS_End,
    };

    enum ShaderCS
    {
        ShaderCS_Start = lfw::ShaderCS_User,
        ShaderCS_End,
    };

    class ShaderCompiledBytes
    {
    public:
        static lfw::s32 getSizeVS(lfw::s32 id);
        static const lfw::u8* getBytesVS(lfw::s32 id);
        static lfw::s32 getSizePS(lfw::s32 id);
        static const lfw::u8* getBytesPS(lfw::s32 id);
        static lfw::s32 getSizeCS(lfw::s32 id);
        static const lfw::u8* getBytesCS(lfw::s32 id);
    private:
        static const lfw::u8* bytesVS_[];
        static const lfw::s32 sizeVS_[];
        static const lfw::u8* bytesPS_[];
        static const lfw::s32 sizePS_[];
        static const lfw::u8* bytesCS_[];
        static const lfw::s32 sizeCS_[];
    };
}
#endif //INC_DEBUG_SHADERID_H__
