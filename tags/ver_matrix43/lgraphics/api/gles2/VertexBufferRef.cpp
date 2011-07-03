/**
@file VertexBufferRef.cpp
@author t-sakai
@date 2010/06/07 create
*/
#include "../../lgraphics.h"
#include "VertexBufferRef.h"
#include "GraphicsDeviceRef.h"

namespace lgraphics
{
    //------------------------------------------------------------
    //---
    //--- VertexBufferRef
    //---
    //------------------------------------------------------------
    VertexBufferRef::VertexBufferRef(const VertexBufferRef& rhs)
        :vertexSize_(rhs.vertexSize_)
        ,vertexNum_(rhs.vertexNum_)
        ,vbID_(rhs.vbID_)
    {
        if(vbID_ != NULL){
            vbID_->addRef();
        }
    }


    bool VertexBufferRef::getDesc(BufferDesc& desc)
    {
        desc.format_ = Buffer_Vertex;
        desc.type_ = Resource_VertexBuffer;
        desc.usage_ = Usage_None;
        desc.pool_ = Pool_Default;
        desc.size_ = vertexSize_ * vertexNum_;
        return true;
    }


    void VertexBufferRef::destroy()
    {
        if(vbID_!=NULL){
            if(vbID_->counter_ <= 1){
                glDeleteBuffers( 1, reinterpret_cast<const GLuint*>(&(vbID_->id_)) );
            }

            LRELEASE(vbID_);
        }
    }


    // データ転送
    void VertexBufferRef::blit(void* data, bool dynamic)
    {
        u32 bufferSize = vertexSize_*vertexNum_;

        glBindBuffer( GL_ARRAY_BUFFER, vbID_->id_ );

        glBufferData( GL_ARRAY_BUFFER, bufferSize, data, (dynamic)? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

#if 0
        //チェック
        int s = 0;
        glGetBufferParameteriv( GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &s );
        LASSERT((s>=0) && (static_cast<u32>(s)==bufferSize));
#endif

        glBindBuffer( GL_ARRAY_BUFFER, NULL );
    }

    // データ転送
    void VertexBufferRef::blit(void* data, u32 offset, u32 size)
    {
        LASSERT((offset+size) <= (vertexSize_*vertexNum_));

        glBindBuffer( GL_ARRAY_BUFFER, vbID_->id_ );

        glBufferSubData( GL_ARRAY_BUFFER, offset, size, data);


        glBindBuffer( GL_ARRAY_BUFFER, NULL );
    }


    // バッファデータ取得
    //VertexBufferUPRef VertexBufferRef::retrieveAsUP()
    //{
    //}


    void VertexBufferRef::attach() const
    {
        LASSERT(vbID_->id_>0);
        glBindBuffer(GL_ARRAY_BUFFER, vbID_->id_);
    }

    void VertexBufferRef::detach() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, NULL);
    }

    //------------------------------------------------------------
    //---
    //--- VertexBuffer
    //---
    //------------------------------------------------------------
    VertexBufferRef VertexBuffer::create(u32 vertexSize, u32 vertexNum, Pool, Usage)
    {
        Descriptor *desc = lgraphics::Graphics::getDevice().allocate();
        LASSERT(desc != NULL);

        desc->addRef();

        glGenBuffers(1, reinterpret_cast<GLuint*>(&(desc->id_)));
        if(desc->id_ <= 0){
            desc->release();
            return VertexBufferRef();
        }

        return VertexBufferRef(vertexSize, vertexNum, desc);
    }

    VertexBufferRef VertexBuffer::create(VertexBufferUPRef& vbUP, bool blit, bool dynamic)
    {
        Descriptor *desc = lgraphics::Graphics::getDevice().allocate();
        LASSERT(desc != NULL);

        desc->addRef();

        glGenBuffers(1, reinterpret_cast<GLuint*>(&(desc->id_)));
        if(desc->id_ <= 0){
            desc->release();
            return VertexBufferRef();
        }
        VertexBufferRef ret(vbUP.getVertexSize(), vbUP.getVertexNum(), desc);

        if(blit){
            void *data = NULL;
            if(vbUP.lock(&data)){
                ret.blit(data, dynamic);
                vbUP.unlock();
            }
        }
        return ret;
    }

    //------------------------------------------------------------
    //---
    //--- VertexBufferUPRef
    //---
    //------------------------------------------------------------
    VertexBufferUPRef::VertexBufferUPRef(const VertexBufferUPRef& rhs)
        :buffer_(rhs.buffer_)
    {
        if(buffer_ != NULL){
            buffer_->AddRef();
        }
    }


    void VertexBufferUPRef::destroy()
    {
        SAFE_RELEASE(buffer_);
    }

    bool VertexBufferUPRef::getDesc(BufferDesc& desc)
    {
        LASSERT(buffer_ != NULL);
        desc.format_ = Buffer_Vertex;
        desc.type_ = Resource_VertexBuffer;
        desc.usage_ = 0;
        desc.pool_ = Pool_UserMem;
        desc.size_ = buffer_->getVertexSize() * buffer_->getVertexNum();
        return true;
    }

    bool VertexBufferUPRef::lock(void** data)
    {
        LASSERT(buffer_ != NULL);
        return buffer_->lock(data);
    }

    bool VertexBufferUPRef::lock(const void** data) const
    {
        LASSERT(buffer_ != NULL);
        return buffer_->lock(data);
    }

    //------------------------------------------------------------
    //---
    //--- VertexBufferUP
    //---
    //------------------------------------------------------------
    VertexBufferUPRef VertexBufferUP::create(u32 vertexSize, u32 vertexNum)
    {
        u8 *buffer = LIME_NEW u8[vertexSize * vertexNum];
        if(buffer == NULL){
            return VertexBufferUPRef();
        }

        VertexBufferUP *vb = LIME_NEW VertexBufferUP(buffer, vertexSize, vertexNum);
        if(vb == NULL){
            return VertexBufferUPRef();
        }

        vb->AddRef(); // 参照カウントを増やしておく
        return VertexBufferUPRef(vb);
    }

    VertexBufferUP::~VertexBufferUP()
    {
        LIME_DELETE_ARRAY(buffer_);
    }
}
