/**
@file ObjectFactory.cpp
@author t-sakai
@date 2014/12/08 create
*/
#include "ObjectFactory.h"
#include "file/File.h"
#include "render/Object.h"
#include "render/AnimObject.h"
#include "anim/AnimationClip.h"
#include "load/load.h"

#include <lgraphics/api/TextureRef.h>

namespace lscene
{
    //------------------------------------------------
    //---
    //--- ObjectFactory
    //---
    //------------------------------------------------
    ObjectFactory::ObjectFactory()
    {
    }

    ObjectFactory::~ObjectFactory()
    {
    }

    Resource* ObjectFactory::create(s32 type, const Char* path, lfile::SharedFile& file)
    {
        LASSERT(NULL != path);

        switch(type)
        {
        case ResourceType_Unknown:
            {
                s32 size = file.getSize();
                u8* data = reinterpret_cast<u8*>( LSCENE_MALLOC(size) );
                file.read(data, size);
                return LSCENE_NEW lscene::ResourceUnknown(size, data);
            }
            break;

        case ResourceType_Object:
            {
                lrender::Object* object = LSCENE_NEW lrender::Object;
                modelLoader_.setDirectoryPath(path);
                if(!modelLoader_.load(*object, &file)){
                    LSCENE_DELETE(object);
                    return NULL;
                }
                return object;
            }
            break;
        case ResourceType_AnimObject:
            {
                lrender::AnimObject* animObject = LSCENE_NEW lrender::AnimObject;
                modelLoader_.setDirectoryPath(path);
                if(!modelLoader_.load(*animObject, &file)){
                    LSCENE_DELETE(animObject);
                    return NULL;
                }
                return animObject;
            }
            break;

        case ResourceType_AnimClip:
            {
                lanim::AnimationClip* clip = NULL;
                if(!lanim::AnimationClip::deserialize(&clip, file)){
                    LSCENE_DELETE(clip);
                }
                return clip;
            }
            break;

        case ResourceType_Texture2D:
            {
                lload::FileType type = lload::getFileType(path);
                if(lload::FileType_Unknown == type){
                    return NULL;
                }

                lgraphics::Texture2DRef texture;
                if(!lload::load(texture, file, type, lgraphics::Usage_Immutable, lgraphics::TexFilter_MinMagMipLinear, lgraphics::TexAddress_Clamp)){
                    return NULL;
                }
                return LSCENE_NEW ResourceTexture2D(texture);
            }

        case ResourceType_TextureCube:
            {
                lload::FileType type = lload::getFileType(path);
                if(lload::FileType_Unknown == type){
                    return NULL;
                }

                lgraphics::Texture2DRef texture;
                if(!lload::load(texture, file, type, lgraphics::Usage_Immutable, lgraphics::TexFilter_MinMagMipLinear, lgraphics::TexAddress_Clamp)){
                    return NULL;
                }
                return LSCENE_NEW ResourceTextureCube(texture);
            }
        default:
            return NULL;
        }
    }
}
