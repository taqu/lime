#ifndef INC_LSCENE_BUFFERCREATOR_H__
#define INC_LSCENE_BUFFERCREATOR_H__
/**
@file BufferCreator.h
@author t-sakai
@date 2014/10/21 create
*/
#include "lscene.h"

#include <lgraphics/api/Enumerations.h>
#include <lgraphics/api/VertexBufferRef.h>
#include <lgraphics/api/IndexBufferRef.h>

namespace lscene
{
    //--------------------------------------------------
    //---
    //--- VertexBufferCreator
    //---
    //--------------------------------------------------
    struct VertexBufferCreator
    {
        static lgraphics::VertexBufferRef create(u32 size)
        {
            return lgraphics::VertexBuffer::create(
                size,
                lgraphics::Usage_Dynamic,
                lgraphics::BindFlag_VertexBuffer,
                lgraphics::CPUAccessFlag_Write,
                lgraphics::ResourceMisc_None,
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
        static lgraphics::IndexBufferRef create(u32 size)
        {
            return lgraphics::IndexBuffer::create(
                size,
                lgraphics::Usage_Dynamic,
                lgraphics::CPUAccessFlag_Write,
                lgraphics::ResourceMisc_None,
                0,
                NULL);
        }
    };
}

#endif //INC_LSCENE_BUFFERCREATOR_H__
