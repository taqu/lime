/**
@file TextureRef.cpp
@author t-sakai
@date 2010/06/08
*/
#include "../../lgraphics.h"
#include "../../lgraphicsAPIInclude.h"
#include "GraphicsDeviceRef.h"

#include "TextureRef.h"
#include "../../io/IOTextureUtil.h"

namespace lgraphics
{

    namespace
    {
        void TexRGB2BGR(u8* buffer, u32 size)
        {
            u8* end = buffer + size;
            u8 tmp;
            while(buffer<end){
                tmp = buffer[0];
                buffer[0] = buffer[2];
                buffer[2] = tmp;
                buffer += 3;
            }
        }

        void TexARGB2ABGR(u8* buffer, u32 size)
        {
            u8* end = buffer + size;
            u8 tmp;
            while(buffer<end){
                tmp = buffer[0];
                buffer[0] = buffer[2];
                buffer[2] = tmp;
                buffer += 4;
            }
        }

        void TexXRGB2BGR(u8* buffer, u32 size)
        {
            u8* end = buffer + size;
            u8* dst = buffer;

            //最初２回だけコンフリクト
            {
                u8 tmp;
                if(buffer<end){
                    tmp = buffer[0];
                    buffer[0] = buffer[2];
                    buffer[2] = tmp;
                    dst += 3;
                    buffer += 4;
                }
                if(buffer<end){
                    dst[0] = buffer[2];
                    tmp = buffer[0];
                    buffer[0] = buffer[1];
                    buffer[1] = tmp;
                    dst += 3;
                    buffer += 4;
                }
            }

            while(buffer<end){
                dst[0] = buffer[2];
                dst[1] = buffer[1];
                dst[2] = buffer[0];
                dst += 3;
                buffer += 4;
            }
        }

        void TexXBGR2BGR(u8* buffer, u32 size)
        {
            u8* end = buffer + size;
            u8* dst = buffer;
            while(buffer<end){
                dst[0] = buffer[0];
                dst[1] = buffer[1];
                dst[2] = buffer[2];
                dst += 3;
                buffer += 4;
            }
        }
    }

    struct TextureRef::TextureDesc
    {
        NameString name_;
        s32 format_;
        u32 width_;
        u32 height_;
        u16 levels_;
        u16 bpp_;
    };

    //--------------------------------------------
    //---
    //--- TextureRef
    //---
    //--------------------------------------------
    TextureRef::TextureRef(Descriptor* texID, TextureDesc* texDesc)
        :texDesc_(texDesc)
        ,texID_(texID)
    {
        LASSERT(texDesc_ != NULL);
        LASSERT(texID_ != NULL);
        switch(texDesc_->format_)
        {
        case Buffer_A8:
            texDesc_->bpp_ = sizeof(u8);
            break;

        case Buffer_R8G8B8:
            texDesc_->bpp_ = sizeof(u8)*3;
            break;

        case Buffer_B8G8R8:
            texDesc_->bpp_ = sizeof(u8)*3;
            break;

        case Buffer_X8B8G8R8:
            texDesc_->bpp_ = sizeof(u8)*4;
            break;

        case Buffer_X8R8G8B8:
            texDesc_->bpp_ = sizeof(u8)*4;
            break;

        case Buffer_A8R8G8B8:
            texDesc_->bpp_ = sizeof(u8)*4;
            break;

        case Buffer_A8B8G8R8:
            texDesc_->bpp_ = sizeof(u8)*4;
            break;

        case Buffer_L8:
            texDesc_->bpp_ = sizeof(u8);
            break;

        case Buffer_ETC1:
            texDesc_->bpp_ = 0;
            break;

        default:
            LASSERT(false);
            break;
        };
    }

    TextureRef::TextureRef(const TextureRef& rhs)
        :texDesc_(rhs.texDesc_)
        ,texID_(rhs.texID_)
    {
        if(texID_ != NULL){
            texID_->addRef();
        }
    }


    void TextureRef::destroy()
    {
        if(texID_!=NULL){
            if(texID_->counter_ <= 1){
                glDeleteTextures( 1, reinterpret_cast<const GLuint*>(&(texID_->id_)) );
                LIME_DELETE(texDesc_);
            }

            texDesc_ = NULL;
            LRELEASE(texID_);
        }
        LASSERT((texID_ == NULL) && (texDesc_ == NULL));
    }

    u16 TextureRef::getLevels()
    {
        LASSERT(texDesc_ != NULL);
        return texDesc_->levels_;
    }

    bool TextureRef::getLevelDesc(u32 level, SurfaceDesc& desc)
    {
        LASSERT(texDesc_ != NULL);
        if(level>=texDesc_->levels_){
            return false;
        }

        desc.format_ = static_cast<BufferFormat>(texDesc_->format_);
        desc.type_ = Resource_Texture;
        desc.usage_ = Usage_None;
        desc.pool_ = Pool_Default;
        desc.multiSampleType_ = MutiSample_None;
        desc.multiSampleQuality_ = 0;
        desc.width_ = texDesc_->width_;
        desc.height_ = texDesc_->height_;
        return true;
    }


    bool TextureRef::lock(u32, LockedRect&, Lock )
    {
        LASSERT(false);
        return false;
    }

    void TextureRef::unlock(u32)
    {
    }

    // GL的インターフェイス
    void TextureRef::blit(u8* data)
    {
        LASSERT(data != NULL);

        glBindTexture(GL_TEXTURE_2D, texID_->id_);

        blit(0, texDesc_->width_, texDesc_->height_, data);
        glBindTexture(GL_TEXTURE_2D, NULL);
    }

    void TextureRef::blit(u32 level, u32 width, u32 height, u8* data)
    {
        LASSERT(level<=texDesc_->levels_);
        LASSERT(data != NULL);

        u32 align = 4;

        s32 internalFormat = 0;

        GLenum type = GL_UNSIGNED_BYTE;

        switch(texDesc_->format_)
        {
        case Buffer_A8:
            align = 1; //1バイトアライメント
            internalFormat = GL_ALPHA;
            break;

        case Buffer_R8G8B8: //バイト並び反転の必要あり
            align = 1; //1バイトアライメント
            internalFormat = GL_RGB;

            {//反転
                u32 size = getLevelSize(level, texDesc_->bpp_);
                TexRGB2BGR(data, size);
            }
            break;

        case Buffer_B8G8R8:
            align = 1; //1バイトアライメント
            internalFormat = GL_RGB;

            break;

        case Buffer_A8R8G8B8: //バイト並び反転の必要あり
            internalFormat = GL_RGBA;

            {//反転
                u32 size = getLevelSize(level, texDesc_->bpp_);
                TexARGB2ABGR(data, size);
            }
            break;

        case Buffer_X8R8G8B8: //バイト並び反転の必要あり
            align = 1; //1バイトアライメント
            internalFormat = GL_RGB;

            {//反転
                u32 size = getLevelSize(level, texDesc_->bpp_);
                TexXRGB2BGR(data, size);
            }
            break;

        case Buffer_A8B8G8R8:
            internalFormat = GL_RGBA;
            break;

        case Buffer_X8B8G8R8: //データ削減
            align = 1; //1バイトアライメント
            internalFormat = GL_RGB;

            {//削減
                u32 size = getLevelSize(level, texDesc_->bpp_);
                TexXBGR2BGR(data, size);
            }
            break;

        case Buffer_R5G6B5: //TODO:変換
             LASSERT(false);
             break;

        case Buffer_B5G6R5:
            align = 2; //2バイトアライメント
            internalFormat = GL_RGB;
            type = GL_UNSIGNED_SHORT_5_6_5;
            break;

        case Buffer_A4R4G4B4: //TODO:変換
            LASSERT(false);
            break;

        case Buffer_A4B4G4R4:
            align = 2; //2バイトアライメント
            internalFormat = GL_RGBA;
            type = GL_UNSIGNED_SHORT_4_4_4_4;
            break;

        case Buffer_L8:
            align = 1; //1バイトアライメント
            internalFormat = GL_LUMINANCE;
            break;

        case Buffer_ETC1:
            {
                glPixelStorei(GL_UNPACK_ALIGNMENT, 8); //バイトアライメント
                internalFormat = GL_ETC1_RGB8_OES;
                u32 w = width + 0x03U;
                w >>= 2;

                u32 h = height + 0x03U;
                h >>= 2;

                u32 size = (w * h * 8);

                glCompressedTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, 0, size, data);
                return;
            }
            break;

        default:
            LASSERT(false);
            break;
        };
        glPixelStorei(GL_UNPACK_ALIGNMENT, align); //バイトアライメント
        
        glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, 0, internalFormat, type, data);
    }

    //------------------------------------------------
    // シェーダにセット
    void TextureRef::attach(u32 index, u32 location) const
    {
        glBindTexture(GL_TEXTURE_2D, texID_->id_);
        glUniform1i( location, index ); //シェーダのuniform変数にバインド
    }

    void TextureRef::attach() const
    {
        glBindTexture(GL_TEXTURE_2D, texID_->id_);
    }

    void TextureRef::detach() const
    {
        glBindTexture(GL_TEXTURE_2D, NULL);
        //glDisable(GL_TEXTURE_2D);
    }

    const NameString& TextureRef::getName() const
    {
        LASSERT(texDesc_ != NULL);

        return texDesc_->name_;
    }

    NameString& TextureRef::getName()
    {
        LASSERT(texDesc_ != NULL);
        return texDesc_->name_;
    }

    void TextureRef::setName(const NameString& name)
    {
        LASSERT(texDesc_ != NULL);
        texDesc_->name_ = name;
    }

    void TextureRef::setName(const char* name, u32 len)
    {
        LASSERT(texDesc_ != NULL);
        texDesc_->name_.assign(name, len);
    }

    //-------------------------------------------
    // 指定レベルまでのバイトオフセットを計算
    u32 TextureRef::getLevelOffset(u32 level) const
    {
        LASSERT(texDesc_ != NULL);

        u32 offset = 0;
        u32 w = texDesc_->width_;
        u32 h = texDesc_->height_;
        for(u32 i=0; i<level; ++i){
            offset += w * h;
            w = (w>=2)? (w>>1) : 1;
            h = (h>=2)? (h>>1) : 1;
        }
        return offset * texDesc_->bpp_;
    }

    //--------------------------------------------
    // 指定レベルでのサイズ計算
    u32 TextureRef::getLevelSize(u32 level, u32 bpp) const
    {
        LASSERT(texDesc_ != NULL);

        u32 w = texDesc_->width_;
        u32 h = texDesc_->height_;
        for(u32 i=0; i<level; ++i){
            w = (w>=2)? (w>>1) : 1;
            h = (h>=2)? (h>>1) : 1;
        }
        return (w*h*bpp);
    }

    u16 TextureRef::getBytePerPixel() const
    {
        LASSERT(texDesc_ != NULL);

        return texDesc_->bpp_;
    }

    u32 TextureRef::getTextureID() const
    {
        return texID_->id_;
    }

    s32 TextureRef::getFormat() const
    {
        return texDesc_->format_;
    }

    //--------------------------------------------
    //---
    //--- Texture
    //---
    //--------------------------------------------
     TextureRef Texture::create(u32 width, u32 height, u32 levels, Usage , BufferFormat format, Pool )
    {
        Descriptor *desc = lgraphics::Graphics::getDevice().allocate();
        LASSERT(desc != NULL);

        desc->addRef();

        glGenTextures(1, reinterpret_cast<GLuint*>(&(desc->id_)));
        if(desc->id_ <= 0){
            desc->release();
            return TextureRef();
        }


        levels = (levels<=0)? 1 : levels;

        TextureRef::TextureDesc* texDesc = LIME_NEW TextureRef::TextureDesc;
        texDesc->width_ = width;
        texDesc->height_ = height;
        texDesc->levels_ = static_cast<u16>(levels);
        texDesc->format_ = format;

        return TextureRef(desc, texDesc);
    }


     //------------------------------------------------
    //void Texture::update(TextureSystemMemRef& src, TextureRef& dst)
    //{
    //}
}
