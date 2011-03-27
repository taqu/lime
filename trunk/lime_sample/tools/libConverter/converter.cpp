/**
@file converter.cpp
@author t-sakai
@date 2011/01/25 create

*/
#include "converter.h"
#include "charcode/conv_charcode.h"

#include <wchar.h>
#include <locale.h>
#include <string.h>

#include <lcore/String.h>
#include <lcore/utility.h>
#include <lgraphics/api/TextureRef.h>
#include <lgraphics/io/IOBMP.h>
#include <lgraphics/io/IODDS.h>
#include <lgraphics/io/IOTGA.h>
#include <lgraphics/io/IOPNG.h>

#include <lframework/IOUtil.h>
using namespace lgraphics;

namespace lconverter
{
    static const u32 TexNameSize = lgraphics::MAX_NAME_BUFFER_SIZE;
    typedef lgraphics::NameString TextureName;

    s16 F32ToS16(f32 value)
    {
        if(value>=0.0f){
            return static_cast<s16>(SHRT_MAX*value);
        }else{
            return static_cast<s16>((-SHRT_MIN)*value);
        }
    }

    s16 F32ToS16Clamp(f32 value)
    {
        if(value>1.0f){
            return SHRT_MAX;
        }else if(value<0.0f){
            return 0;
        }else{
            return static_cast<s16>( SHRT_MAX*value );
        }
    }

    u16 F32ToU16(f32 value)
    {
        if(value>1.0f){
            return USHRT_MAX;
        }else if(value<0.0f){
            return 0;
        }else{
            return static_cast<s16>( USHRT_MAX*value );
        }
    }


    lgraphics::TextureRef* loadTexture(const Char* path, u32 size, const Char* directory, NameTextureMap& texMap, bool transpose)
    {
        LASSERT(path != NULL);

        lframework::ImageFormat format = lframework::Img_None;
        const Char* ext = lcore::rFindChr(path, '.', size);
        if(ext == NULL){
            //lcore::Log("pmd tex name has no ext(%s)", path);
            return NULL;

        }else{
            format = lframework::io::getFormatFromExt(ext+1);
        }

        u32 mapPos = texMap.find(path, size);
        if(texMap.isEnd(mapPos) == false){
            //すでに同じ名前のテクスチャが存在した
            //lcore::Log("pmd tex has already loaded");
            return texMap.getValue(mapPos);
        }

        Char* filepath = NULL;
        if(directory == NULL){
            filepath = LIME_NEW Char[size+1];
            lcore::strncpy(filepath, size+1, path, size);
        }else{
            u32 dlen = lcore::strlen(directory);
            u32 pathLen = dlen + size + 1;
            filepath = LIME_NEW Char[pathLen];
            lcore::memcpy(filepath, directory, dlen);
            lcore::memcpy(filepath+dlen, path, size + 1);
        }

        lcore::ifstream is(filepath, lcore::ios::in|lcore::ios::binary);
        if(is.is_open() == false){
            lcore::Log("pmd failt to open tex(%s)", filepath);
            LIME_DELETE_ARRAY(filepath);
            return NULL;
        }

        TextureRef *texture = LIME_NEW TextureRef;
        if(texture == NULL){
            LIME_DELETE_ARRAY(filepath);
            return NULL;
        }

        bool ret = false;
        switch(format)
        {
        case lframework::Img_BMP:
            ret = lgraphics::io::IOBMP::read(is, *texture, transpose);
            break;

        case lframework::Img_TGA:
            ret = lgraphics::io::IOTGA::read(is, *texture);
            break;

        case lframework::Img_DDS:
            ret = lgraphics::io::IODDS::read(is, *texture);
            break;

        case lframework::Img_PNG:
            ret = lgraphics::io::IOPNG::read(is, *texture);
            break;

        default:
            break;
        };

        if(ret){
            texture->setName(path, size);

            texMap.insert(texture->getName().c_str(), texture->getName().size(), texture);
            //lcore::Log("pmd success to load tex(%s)", filepath);
        }else{
            LIME_DELETE(texture);
            lcore::Log("pmd fail to load tex(%s)", filepath);
        }

        LIME_DELETE_ARRAY(filepath);
        return texture;
    }


    //--------------------------------------------
    f32 texAddress(f32 value, TextureAddress op)
    {
        switch(op)
        {
        case TexAddress_Wrap:
            {
                while(value>1.0f){
                    value -= 1.0f;
                }
                while(value<0.0f){
                    value += 1.0f;
                }
            }
            break;

        default:
            if(value>1.0f){
                value = 1.0f;
            }else if(value<0.0f){
                value = 0.0f;
            }
            break;
        };
        return value;
    }


    void extractFileNameUTF8(Char* dst)
    {
        LASSERT(dst);
        StringBuffer path;
        //ディレクトリパス抽出
        u32 pathLen = lcore::strlen(dst);
        path.resize(pathLen);
        u32 dlen = lcore::extractDirectoryPath(path.c_str(), dst, pathLen);

        //ファイル名取得
        Char* filename = dst + dlen;
        u32 lenFilename = pathLen - dlen;
#if defined(_WIN32)
        for(u32 i=0; i<lenFilename; ++i){
            dst[i] = filename[i];
        }
        dst[lenFilename] = '\0';
#else
        //SJISをUTF-8へ変換
        for(u32 i=0; i<lenFilename; ++i){
            path[i] = filename[i];
        }
        path[lenFilename] = '\0';

        u32 utf8size = charcode::strSJISToUTF8(NULL, reinterpret_cast<const u8*>(filename));
        if(utf8size>255){
            //変換後が255バイトを超える場合サポートしない
            for(u32 i=0; i<lenFilename; ++i){
                dst[i] = filename[i];
            }
            dst[lenFilename] = '\0';
        }else{
            for(u32 i=0; i<lenFilename; ++i){
                path[i] = filename[i];
            }
            path[lenFilename] = '\0';

            charcode::strSJISToUTF8(reinterpret_cast<u8*>(dst), reinterpret_cast<const u8*>(&path[0]));
            dst[utf8size] = '\0';
        }
#endif
    }
}
