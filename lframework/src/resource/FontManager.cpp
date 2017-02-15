/**
@file FontManager.cpp
@author t-sakai
@date 2016/12/25 create
*/
#include "resource/FontManager.h"
#include <lcore/io/VirtualFileSystem.h>
#include <lcore/Sort.h>

#include "Application.h"
#include "resource/Resources.h"
#include "resource/Font.h"
#include "resource/ResourceTexture2D.h"

namespace lfw
{

    FontManager::FontManager()
    {
        for(s32 i=0; i<MaxSlots; ++i){
            fonts_[i] = NULL;
        }
    }

    FontManager::~FontManager()
    {
        for(s32 i=0; i<MaxSlots; ++i){
            LDELETE(fonts_[i]);
        }
    }

    bool FontManager::load(s32 slot, const Char* path)
    {
        LASSERT(0<=slot && slot<MaxSlots);

        lcore::FileSystem& fileSystem = Application::getInstance().getFileSystem();
        lcore::FileProxy* file = fileSystem.openFile(path);
        if(NULL == file){
            return false;
        }

        u32 identifier = 0;

        s64 offset = 0;
        file->read(offset, 4, reinterpret_cast<u8*>(&identifier));
        offset += 4;
        if(Identifier != identifier){
            fileSystem.closeFile(file);
            return false;
        }

        Resource::pointer texFont;
        static const s32 BufferSize = 32;
        s32 setID = 0;
        s32 pathlen = lcore::strlen_s32(path);
        TextureParameter texParam;
        texParam.initialize();
        texParam.sRGB_ = 0;
        if(BufferSize<pathlen){
            Char* filename = (Char*)LMALLOC(pathlen+8);
            s32 len = lcore::extractFileNameWithoutExt(filename, pathlen, path);
            lcore::strcat(filename, len+8, "_0.dds");
            texFont = Resources::getInstance().load(setID, filename, lfw::ResourceType::ResourceType_Texture2D, texParam);
            LFREE(filename);
        }else{
            Char filename[BufferSize+8];
            s32 len = lcore::extractFileNameWithoutExt(filename, pathlen, path);
            lcore::strcat(filename, len+8, "_0.dds");
            texFont = Resources::getInstance().load(setID, filename, lfw::ResourceType::ResourceType_Texture2D, texParam);
        }
        if(NULL == texFont){
            fileSystem.closeFile(file);
            return false;
        }

        FmtInfo fmtInfo = {0};
        FmtCommon fmtCommon = {0};
        s32 numChars = 0;
        FmtChar* fmtChar = NULL;
        s64 size = file->getUncompressedSize();
        while(offset<size){
            u8 type;
            file->read(offset, 1, &type);
            ++offset;
            switch(type)
            {
            case 1:
                offset = readFmtInfo(fmtInfo, offset, file);
                break;
            case 2:
                offset = readFmtCommon(fmtCommon, offset, file);
                break;
            case 3:
                offset = readFmtPages(offset, file);
                break;
            case 4:
                offset = readFmtChars(numChars, &fmtChar, offset, file);
                break;
            case 5:
                offset = readFmtKerning(offset, file);
                break;
            default:
                offset = readFmtUnknown(offset, file);
                break;
            }
        }

        f32 invWidth = 1.0f/(fmtCommon.scaleW_);
        f32 invHeight = 1.0f/(fmtCommon.scaleH_);
        s32 count = 0;
        for(s32 i=0; i<numChars; ++i){
            if(fmtChar[i].id_<=0xFFU){
                ++count;
            }
        }

        Font* font = LNEW Font;
        Font::FontChar* chars = (Font::FontChar*)LMALLOC(sizeof(Font::FontChar)*count);
        count = 0;
        for(s32 i=0; i<numChars; ++i){
            if(0xFFU<fmtChar[i].id_){
                continue;
            }
            chars[count].id_ = static_cast<u16>(fmtChar[i].id_);
            chars[count].xadvance_ = fmtChar[i].xadvance_;
            chars[count].xoffset_ = fmtChar[i].xoffset_;
            chars[count].yoffset_ = fmtChar[i].yoffset_;
            chars[count].width_ = fmtChar[i].width_;
            chars[count].height_ = fmtChar[i].height_;
            chars[count].uv_[0] = lcore::toBinary16Float(invWidth*fmtChar[i].x_);
            chars[count].uv_[1] = lcore::toBinary16Float(invHeight*fmtChar[i].y_);
            chars[count].uv_[2] = lcore::toBinary16Float(invWidth*(fmtChar[i].x_+fmtChar[i].width_));
            chars[count].uv_[3] = lcore::toBinary16Float(invHeight*(fmtChar[i].y_+fmtChar[i].height_));
            ++count;
        }
        lcore::introsort(count, chars, Font::less);

        font->fontSize_ = fmtInfo.fontSize_;
        font->lineHeight_ = fmtCommon.lineHeight_;
        font->width_ = fmtCommon.scaleW_;
        font->height_ = fmtCommon.scaleH_;
        font->invWidth_ = invWidth;
        font->invHeight_ = invHeight;
        font->numChars_ = count;
        font->chars_ = chars;

        lfw::ResourceTexture2D* ptex = texFont->cast<lfw::ResourceTexture2D>();
        font->texture_ = ptex->get();
        font->srv_ = ptex->getShaderResourceView();

        LDELETE(fonts_[slot]);
        fonts_[slot] = font;

        LFREE(fmtChar);
        fileSystem.closeFile(file);
        return true;
    }

    Font* FontManager::get(s32 index)
    {
        return fonts_[index];
    }

    s64 FontManager::readFmtInfo(FmtInfo& fmtInfo, s64 offset, lcore::FileProxy* file)
    {
        u32 size = 0;
        file->read(offset, 4, reinterpret_cast<u8*>(&size));
        offset += 4;

        file->read(offset, 14, reinterpret_cast<u8*>(&fmtInfo));
        offset += 14;

        u8 c = 0;
        do{
            file->read(offset, 1, &c);
            ++offset;
        }while(c != lcore::CharNull);
        return offset;
    }

    s64 FontManager::readFmtCommon(FmtCommon& fmtCommon, s64 offset, lcore::FileProxy* file)
    {
        u32 size = 0;
        file->read(offset, 4, reinterpret_cast<u8*>(&size));
        offset += 4;

        file->read(offset, 15, reinterpret_cast<u8*>(&fmtCommon));
        offset += 15;

        return offset;
    }

    s64 FontManager::readFmtPages(s64 offset, lcore::FileProxy* file)
    {
        return readFmtUnknown(offset, file);
    }

    s64 FontManager::readFmtChars(s32& numChars, FmtChar** chars, s64 offset, lcore::FileProxy* file)
    {
        u32 size = 0;
        file->read(offset, 4, reinterpret_cast<u8*>(&size));
        offset += 4;

        static const s32 Size = 20;
        numChars = size/Size;
        *chars = (FmtChar*)LMALLOC(sizeof(FmtChar)*numChars);

        file->getUncompressedSize();
        file->read(offset, numChars*Size, reinterpret_cast<u8*>(*chars));
        offset += numChars*Size;

        return offset;
    }

    s64 FontManager::readFmtKerning(s64 offset, lcore::FileProxy* file)
    {
        return readFmtUnknown(offset, file);
    }

    s64 FontManager::readFmtUnknown(s64 offset, lcore::FileProxy* file)
    {
        u32 size = 0;
        file->read(offset, 4, reinterpret_cast<u8*>(&size));
        offset += 4;
        offset += size;
        return offset;
    }
}
