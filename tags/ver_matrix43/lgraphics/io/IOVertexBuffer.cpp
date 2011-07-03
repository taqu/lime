/**
@file IOVertexBuffer.cpp
@author t-sakai
@date 2010/05/06 create
*/
#include "IOVertexBuffer.h"

#include "../lgraphicscore.h"
#include "../api/EnumerationCodes.h"
#include "../api/VertexBufferRef.h"
#include "SectionID.h"

namespace lgraphics
{
namespace io
{
    namespace
    {
        struct VertexBufferHeader
        {
            u32 id_;
            u32 size_;
            u32 num_;
            u16 pool_;
            u32 usage_;
        };
    }

    bool IOVertexBuffer::read(lcore::istream& is, VertexBufferRef& buffer)
    {
        VertexBufferHeader vbHeader;

        lcore::io::read(is, vbHeader);

        vbHeader.pool_ = static_cast<u16>( decodePool(static_cast<CodePool>(vbHeader.pool_)) );
        vbHeader.usage_ = decodeUsage(static_cast<CodeUsage>(vbHeader.usage_));

        buffer = VertexBuffer::create(vbHeader.size_, vbHeader.num_, static_cast<Pool>(vbHeader.pool_), static_cast<Usage>(vbHeader.usage_));

#if defined(LIME_GL)
        //GL版
        u32 size = buffer.getVertexNum() * buffer.getVertexSize();
        u8* data = LIME_NEW u8[size];

        lcore::io::read(is, data, size);
        buffer.blit(data);
        LIME_DELETE_ARRAY(data);
#else
        //DX版
        void* data = NULL;
        bool ret = buffer.lock(0, vbHeader.num_*vbHeader.size_, &data);
        if(ret == false){
            return false;
        }

        lcore::io::read(is, data, vbHeader.num_*vbHeader.size_);

        buffer.unlock();
#endif
        return true;
    }

#if defined(LIME_GL)
    bool IOVertexBuffer::write(lcore::ostream&, VertexBufferRef&)
    {
        return true;
    }

#else
    bool IOVertexBuffer::write(lcore::ostream& os, VertexBufferRef& buffer)
    {
        //TODO:GL版作成
        VertexBufferHeader vbHeader;

        VertexBufferRef::BufferDesc desc;
        buffer.getDesc(desc);

        vbHeader.id_ = format::VertexBufferID;
        vbHeader.size_ = buffer.getVertexSize();
        vbHeader.num_ = buffer.getVertexNum();
        vbHeader.pool_ = static_cast<u16>( encodePool(desc.pool_) );
        vbHeader.usage_ = encodeUsage( static_cast<Usage>(desc.usage_) );

        u32 vsize = vbHeader.num_ * vbHeader.size_;

        void* data = NULL;
        bool ret = buffer.lock(0, vsize, &data);
        if(ret == false){
            return false;
        }

        lcore::io::write(os, vbHeader);        
        lcore::io::write(os, data, vsize);

        buffer.unlock();
        return true;
    }
#endif

}
}
