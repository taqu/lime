/**
@file IOVertexBufferUP.cpp
@author t-sakai
@date 2010/05/06 create
*/
#include "IOUtil.h"

#include "IOVertexBufferUP.h"
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
            u32 stream_;
            u16 offset_;
            u16 pool_;
            u32 usage_;
        };
    }

    bool IOVertexBufferUP::read(lcore::istream& is, VertexBufferUPRef& buffer)
    {
        VertexBufferHeader vbHeader;

        io::read(is, vbHeader);

        buffer = VertexBufferUP::create(vbHeader.size_, vbHeader.num_, vbHeader.stream_, vbHeader.offset_);

        void* data = NULL;
        bool ret = buffer.lock(&data);
        if(ret == false){
            return false;
        }

        io::read(is, data, vbHeader.num_*vbHeader.size_);

        buffer.unlock();
        return true;
    }

    bool IOVertexBufferUP::write(lcore::ostream& os, VertexBufferUPRef& buffer)
    {
        VertexBufferHeader vbHeader;

        VertexBufferUPRef::BufferDesc desc;
        buffer.getDesc(desc);

        vbHeader.id_ = format::VertexBufferID;
        vbHeader.size_ = buffer.getVertexSize();
        vbHeader.num_ = buffer.getVertexNum();
        vbHeader.stream_ = buffer.getStream();
        vbHeader.offset_ = buffer.getOffset();
        vbHeader.pool_ = static_cast<u16>( encodePool(desc.pool_) );
        vbHeader.usage_ = encodeUsage( static_cast<Usage>(desc.usage_) );


        void* data = NULL;
        bool ret = buffer.lock(&data);
        if(ret == false){
            return false;
        }

        io::write(os, vbHeader);        
        io::write(os, data, vbHeader.num_*vbHeader.size_);

        buffer.unlock();
        return true;
    }
}
}
