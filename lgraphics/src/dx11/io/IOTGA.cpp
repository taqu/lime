/**
@file IOTGA.cpp
@author t-sakai
@date 2010/05/20 create
@date 2018/10/06 modify to using cppimg
*/
#include "TextureRef.h"
#include "io/IOTGA.h"

namespace lgfx
{
namespace io
{
    //---------------------------------------------------------------------------------------------------
    bool IOTGA::read(
        ISStream& is,
        u8* buffer,
        s32& width,
        s32& height,
        DataFormat& format)
    {
        ColorType colorType;
        if(!cppimg::TGA::read(width, height, colorType, buffer, is)){
            return false;
        }

        switch(colorType){
        case cppimg::ColorType_GRAY:
            format = Data_R8_UNorm;
            if(NULL != buffer){
                toLinearGray(width, height, buffer);
            }
            break;
        case cppimg::ColorType_RGB:
            format = Data_R8G8B8A8_UNorm_SRGB;
            if(NULL != buffer){
                rgb2rgba(width, height, buffer);
            }
            break;
        case cppimg::ColorType_RGBA:
            format = Data_R8G8B8A8_UNorm_SRGB;
            break;
        default:
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------------------------------------------
    bool IOTGA::read(
        Texture2DRef& texture,
        ISStream& is,
        Usage usage,
        BindFlag bindFlag,
        CPUAccessFlag access,
        ResourceMisc misc,
        s32& width,
        s32& height,
        DataFormat& format)
    {
        ColorType colorType;
        bool result = cppimg::TGA::read(width, height, colorType, NULL, is);
        if(!result){
            return false;
        }
        switch(colorType){
        case cppimg::ColorType_GRAY:
            format = Data_R8_UNorm;
            break;
        case cppimg::ColorType_RGB:
            format = Data_R8G8B8A8_UNorm_SRGB;
            break;
        case cppimg::ColorType_RGBA:
            format = Data_R8G8B8A8_UNorm_SRGB;
            break;
        default:
            return false;
        }

        u32 rowBytes = getBitsPerPixel(format) * width;
        u32 size = rowBytes * height;
        u8* buffer = LNEW u8[size];
        result = cppimg::TGA::read(width, height, colorType, buffer, is);
        if(!result){
            LDELETE_ARRAY(buffer);
            return false;
        }

        switch(colorType){
        case cppimg::ColorType_GRAY:
            toLinearGray(width, height, buffer);
            break;
        case cppimg::ColorType_RGB:
            rgb2rgba(width, height, buffer);
            break;
        case cppimg::ColorType_RGBA:
            break;
        default:
            LDELETE_ARRAY(buffer);
            return false;
        }

        SubResourceData initData;
        initData.pitch_ = rowBytes;
        initData.slicePitch_ = 0;
        initData.mem_ = buffer;

        texture = lgfx::Texture::create2D(
            width,
            height,
            1,
            1,
            format,
            usage,
            bindFlag,
            access,
            misc,
            &initData);

        LDELETE_ARRAY(buffer);
        return texture.valid();
    }

    //-----------------------------------------------------------------
    bool IOTGA::write(
        OSStream& os,
        const u8* buffer,
        s32 width,
        s32 height,
        ColorType colorType)
    {
        LASSERT(NULL != buffer);
        LASSERT(0<=width);
        LASSERT(0<=height);
        return cppimg::TGA::write(os, width, height, colorType, buffer);
    }
}
}
