#ifndef INC_LGRAPHICS_BUFFERCREATOR_H__
#define INC_LGRAPHICS_BUFFERCREATOR_H__
/**
@file BufferCreator.h
@author t-sakai
@date 2016/11/10 create
*/
#include "lgraphics.h"

#include "Enumerations.h"
#include "VertexBufferRef.h"
#include "IndexBufferRef.h"
#include "TextureRef.h"

namespace lgfx
{
    //--------------------------------------------------
    //---
    //--- VertexBufferCreator
    //---
    //--------------------------------------------------
    struct VertexBufferCreator
    {
        static lgfx::VertexBufferRef create(u32 size)
        {
            return lgfx::VertexBuffer::create(
                size,
                lgfx::Usage_Dynamic,
                lgfx::BindFlag_VertexBuffer,
                lgfx::CPUAccessFlag_Write,
                lgfx::ResourceMisc_None,
                0,
                NULL);
        }
    };

    //--------------------------------------------------
    //---
    //--- IndexBufferCreator
    //---
    //--------------------------------------------------
    struct IndexBufferCreator
    {
        static lgfx::IndexBufferRef create(u32 size)
        {
            return lgfx::IndexBuffer::create(
                size,
                lgfx::Usage_Dynamic,
                lgfx::CPUAccessFlag_Write,
                lgfx::ResourceMisc_None,
                0,
                NULL);
        }
    };
}

#endif //INC_LGRAPHICS_BUFFERCREATOR_H__
