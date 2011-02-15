/**
@file IOUtil.cpp
@author t-sakai
@date 2010/05/25 create
*/
#include "IOUtil.h"
#include <lgraphics/lgraphicsAPIInclude.h>
#include <lgraphics/api/TextureRef.h>

namespace lframework
{
namespace io
{
    using namespace lgraphics;

    // 画像の起点、左上・左下を入れ替える
    void swapOrigin(TextureRef& texture)
    {
        u32 levels = texture.getLevels();
        TextureRef::SurfaceDesc desc;

        if( false == texture.getLevelDesc(0, desc) ){
            return;
        }

        u32 height = desc.height_;

        for(u32 i=0; i<levels; ++i){
            TextureRef::LockedRect rect;
            if(false == texture.lock(i, rect)){
                break;
            }

            u8* upper = reinterpret_cast<u8*>(rect.bits_);
            u8* lower = upper + (height - 1)*rect.pitch_;
            while(upper<lower){
                for(s32 j=0; j<rect.pitch_; ++j){
                    lcore::swap(upper[j], lower[j]);
                }
                upper += rect.pitch_;
                lower -= rect.pitch_;
            }

            texture.unlock(i);
        }
    }

    ImageFormat getFormatFromExt(const Char* ext)
    {
        LASSERT(ext != NULL);
        for(u32 i=0; i<Img_Num; ++i){
            if(strncmp(ext, ImageFormatExt[i], 3) == 0){
                return static_cast<ImageFormat>(i);
            }
        }
        return Img_None;
    }
}
}
