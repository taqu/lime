/**
@file TextureRef.cpp
@author t-sakai
@date 2009/05/12
*/
#include <fstream>
#include "../../lgraphics.h"
#include "../../lgraphicsAPIInclude.h"
#include "GraphicsDeviceRef.h"

#include "TextureRef.h"

namespace lgraphics
{

    struct TextureRef::TextureDesc
    {
        TextureDesc()
            :refCount_(0)
        {}

        void addRef(){ ++refCount_;}
        void release(){ --refCount_;}

        s32 refCount_;
        NameString name_;
    };

    //--------------------------------------------
    //---
    //--- TextureRef
    //---
    //--------------------------------------------
    TextureRef::TextureRef(IDirect3DTexture9* texture, TextureDesc* texDesc)
        :texDesc_(texDesc)
        ,texture_(texture)
    {
        LASSERT(texDesc_ != NULL);
        LASSERT(texture_ != NULL);
    }

    TextureRef::TextureRef(const TextureRef& rhs)
        :texDesc_(rhs.texDesc_)
        ,texture_(rhs.texture_)
    {
        if(texDesc_ != NULL){
            LASSERT(texture_ != NULL);
            texDesc_->addRef();
            texture_->AddRef();
        }
    }

    void TextureRef::destroy()
    {
        if(texDesc_ != NULL){
            if(texDesc_->refCount_ <= 1){
                LIME_DELETE(texDesc_);
            }else{
                LRELEASE(texDesc_);
            }

            SAFE_RELEASE(texture_);
        }

        LASSERT((texture_ == NULL) && (texDesc_ == NULL));
    }

    u16 TextureRef::getLevels()
    {
        LASSERT(texture_ != NULL);
        return static_cast<u16>( texture_->GetLevelCount() );
    }

    bool TextureRef::getLevelDesc(u32 level, SurfaceDesc& desc)
    {
        LASSERT(texture_ != NULL);
        LASSERT(0<=level && level<getLevels());
        HRESULT hr = texture_->GetLevelDesc(level, (D3DSURFACE_DESC*)&desc);
        return SUCCEEDED(hr);
    }

    bool TextureRef::lock(u32 level, LockedRect& lockedRect, Lock lock)
    {
        LASSERT(texture_ != NULL);
        HRESULT hr = texture_->LockRect(level, (D3DLOCKED_RECT*)&lockedRect, NULL, lock);
        return SUCCEEDED(hr);
    }

    void TextureRef::unlock(u32 level)
    {
        LASSERT(texture_ != NULL);
        texture_->UnlockRect(level);
    }

    // データ転送
    void TextureRef::blit(u8* data)
    {
        LASSERT(data != NULL);

        LockedRect lockedRect;
        HRESULT hr = texture_->LockRect(0, (D3DLOCKED_RECT*)&lockedRect, NULL, Lock_None);
        if(FAILED(hr)){
            return;
        }

        SurfaceDesc desc;
        hr = texture_->GetLevelDesc(0, (D3DSURFACE_DESC*)&desc);
        if(FAILED(hr)){
            return;
        }

        //ミップマップサイズ計算
        u32 size = desc.height_ * lockedRect.pitch_;

        //TODO:ちゃんとピッチやサイズをみないと
        lcore::memcpy(lockedRect.bits_, data, size);

        texture_->UnlockRect(0);

    }

    // データ転送
    void TextureRef::blit(u32 level, u32 width, u32 height, u8* data)
    {
        LASSERT(data != NULL);

        LockedRect lockedRect;
        HRESULT hr = texture_->LockRect(level, (D3DLOCKED_RECT*)&lockedRect, NULL, Lock_None);
        if(FAILED(hr)){
            return;
        }

        //ミップマップサイズ計算
        u32 size = width * height;

        //TODO:ちゃんとピッチやサイズをみないと
        lcore::memcpy(lockedRect.bits_, data, size);

        texture_->UnlockRect(level);

    }

    void TextureRef::attach(u32 index) const
    {
        GraphicsDeviceRef &device = Graphics::getDevice();
        device.setTexture(index, texture_);
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

    s32 TextureRef::getFormat() const
    {
        SurfaceDesc desc;
        HRESULT hr = texture_->GetLevelDesc(0, (D3DSURFACE_DESC*)&desc);
        if(FAILED(hr)){
            return Buffer_Unknown;
        }
        return desc.format_;
    }

    //-------------------------------------------------------
    namespace
    {
        D3DXIMAGE_FILEFORMAT getFormatFromExt(const char* filename)
        {
            std::string str(filename);
            std::string::size_type pos = str.rfind('.');
            if(pos == std::string::npos){
                return D3DXIFF_FORCE_DWORD;
            }

            str = str.substr(pos+1);
            
            static const char* ext[D3DXIFF_PFM+1] =
            {
                "bmp",
                "jpg",
                "tga",
                "png",
                "dds",
                "ppm",
                "dib",
                "hdr",
                "pfm",
            };
            for(s32 i=0; i<=D3DXIFF_PFM; ++i){
                if(str.compare(ext[i]) == 0){
                    return static_cast<D3DXIMAGE_FILEFORMAT>(i);
                }
            }

            return D3DXIFF_FORCE_DWORD;
        }
    }

    void TextureRef::debugout(const char* filename)
    {
        LASSERT(texture_ != NULL);
        LASSERT(filename != NULL);
        D3DXIMAGE_FILEFORMAT format = getFormatFromExt(filename);
        if(format == D3DXIFF_FORCE_DWORD){
            return;
        }

        D3DXSaveTextureToFile(filename, format, texture_, NULL);
    }



    //--------------------------------------------
    //---
    //--- Texture
    //---
    //--------------------------------------------
     TextureRef Texture::create(u32 width, u32 height, u32 levels, Usage usage, BufferFormat format, Pool poolType)
    {
        GraphicsDeviceRef &device = Graphics::getDevice();

        IDirect3DTexture9 *d3dtex = device.createTexture(
            width,
            height,
            levels,
            usage,
            format,
            poolType);

        if(d3dtex == NULL){
            return TextureRef();
        }

        TextureRef::TextureDesc *texDesc = LIME_NEW TextureRef::TextureDesc;
        texDesc->addRef();

        return TextureRef(d3dtex, texDesc);
    }


     //------------------------------------------------
    void Texture::update(TextureSystemMemRef& src, TextureRef& dst)
    {
        GraphicsDeviceRef &device = Graphics::getDevice();
        device.updateTexture(src.texture_, dst.texture_);
    }
}
