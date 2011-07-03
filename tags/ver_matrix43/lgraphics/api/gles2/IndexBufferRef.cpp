/**
@file IndexBufferRef.cpp
@author t-sakai
@date 2009/05/07 create
@data 2010/01/08 add IndexBufferUPRef
*/
#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"

#include "IndexBufferRef.h"

namespace lgraphics
{
    //------------------------------------------------------------
    //---
    //--- IndexBufferRef
    //---
    //------------------------------------------------------------
    IndexBufferRef::IndexBufferRef(const IndexBufferRef& rhs)
        :numIndices_(rhs.numIndices_)
        ,ibID_(rhs.ibID_)
    {
        if(ibID_ != NULL){
            ibID_->addRef();
        }
    }

    void IndexBufferRef::destroy()
    {
        if(ibID_!=NULL){
            if(ibID_->counter_ <= 1){
                glDeleteBuffers( 1, reinterpret_cast<const GLuint*>(&(ibID_->id_)) );
            }

            LRELEASE(ibID_);
        }
    }


    // データ転送
    void IndexBufferRef::blit(void* data, bool dynamic)
    {
        LASSERT(data != NULL);
        LASSERT(ibID_ != NULL);

        u32 bufferSize = sizeof(u16)*numIndices_;

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibID_->id_);
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, bufferSize, data, (dynamic)?GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, NULL);
    }

    void IndexBufferRef::attach() const
    {
        LASSERT(ibID_->id_>0);
        //glEnableClientState(GL_INDEX_ARRAY);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibID_->id_);
        //glIndexPointer(GL_SHORT, sizeof(u16), LBUFFER_OFFSET(0));
    }

    void IndexBufferRef::detach() const
    {
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, NULL );
        //glDisableClientState(GL_INDEX_ARRAY);
    }

    bool IndexBufferRef::getDesc(BufferDesc& desc)
    {
        desc.format_ = Buffer_Index16;
        desc.type_ = Resource_IndexBuffer;
        desc.usage_ = Usage_None;
        desc.pool_ = Pool_Default;
        desc.size_ = sizeof(u16)*numIndices_;

        return true;
    }


    //------------------------------------------------------------
    //---
    //--- IndexBuffer
    //---
    //------------------------------------------------------------
    IndexBufferRef IndexBuffer::create(u32 numIndices, Pool, Usage)
    {
        Descriptor *desc = lgraphics::Graphics::getDevice().allocate();
        LASSERT(desc != NULL);

        desc->addRef();

        glGenBuffers(1, reinterpret_cast<GLuint*>(&(desc->id_)));
        if(desc->id_ <= 0){
            desc->release();
            return IndexBufferRef();
        }

        return IndexBufferRef(numIndices, desc);
    }



    //------------------------------------------------------------
    //---
    //--- IndexBufferUPRef
    //---
    //------------------------------------------------------------
    IndexBufferUPRef::IndexBufferUPRef(const IndexBufferUPRef& rhs)
        :buffer_(rhs.buffer_)
    {
        if(buffer_ != NULL){
            buffer_->AddRef();
        }
    }


    void IndexBufferUPRef::destroy()
    {
        SAFE_RELEASE(buffer_);
    }

    bool IndexBufferUPRef::getDesc(BufferDesc& desc)
    {
        LASSERT(buffer_ != NULL);
        desc.format_ = Buffer_Index16;
        desc.type_ = Resource_IndexBuffer;
        desc.usage_ = 0;
        desc.pool_ = Pool_UserMem;
        desc.size_ = sizeof(u16) * 3 * buffer_->getNumFaces();
        return true;
    }

    bool IndexBufferUPRef::lock(void** data)
    {
        LASSERT(buffer_ != NULL);
        return buffer_->lock(data);
    }

    bool IndexBufferUPRef::lock(const void** data) const
    {
        LASSERT(buffer_ != NULL);
        return buffer_->lock(data);
    }


    //------------------------------------------------------------
    //---
    //--- IndexBufferUP
    //---
    //------------------------------------------------------------
    IndexBufferUPRef IndexBufferUP::create(u32 numIndices)
    {
        u8 *buffer = LIME_NEW u8[numIndices * sizeof(u16)];
        if(buffer == NULL){
            return IndexBufferUPRef();
        }

        IndexBufferUP *ib = LIME_NEW IndexBufferUP(buffer, numIndices/3);
        if(ib == NULL){
            return IndexBufferUPRef();
        }

        ib->AddRef(); // 参照カウントを増やしておく
        return IndexBufferUPRef(ib);
    }

    IndexBufferUP::~IndexBufferUP()
    {
        LIME_DELETE_ARRAY(buffer_);
    }

}
