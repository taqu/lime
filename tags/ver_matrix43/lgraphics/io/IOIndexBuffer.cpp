/**
@file IOIndexBuffer.cpp
@author t-sakai
@date 2010/05/06 create
*/
#include "IOIndexBuffer.h"
#include "../api/IndexBufferRef.h"
#include "SectionID.h"

namespace lgraphics
{
namespace io
{
    namespace
    {
        struct IndexBufferHeader
        {
            u32 id_;
            u32 format_;
            u32 size_;
        };
    }

    bool IOIndexBuffer::read(lcore::istream& is, IndexBufferRef& buffer)
    {
#if defined(LIME_GL)
        IndexBufferHeader ibHeader;
        lcore::io::read(is, ibHeader);

        u32 numIndices = ibHeader.size_  / sizeof(u16);
        buffer = IndexBuffer::create(numIndices, Pool_Default, Usage_None);

        char *data = LIME_NEW char[ibHeader.size_];
        if(data == NULL){
            return false;
        }

        lcore::io::read(is, data, ibHeader.size_);
        buffer.blit(data);

        LIME_DELETE_ARRAY(data);

#else
        IndexBufferHeader ibHeader;
        lcore::io::read(is, ibHeader);

        u32 numIndices = ibHeader.size_  / sizeof(u16);
        buffer = IndexBuffer::create(numIndices, Pool_Default, Usage_None);

        void *data = NULL;
        bool ret = buffer.lock(0, ibHeader.size_, &data);
        if(ret == false){
            return false;
        }

        lcore::io::read(is, data, ibHeader.size_);

        buffer.unlock();
#endif
        return true;
    }

#if defined(LIME_GL)
    bool IOIndexBuffer::write(lcore::ostream&, IndexBufferRef&)
    {
        return true;
    }
#else
    bool IOIndexBuffer::write(lcore::ostream& os, IndexBufferRef& buffer)
    {
        IndexBufferDesc desc;
        bool ret = buffer.getDesc(desc);
        if(ret == false){
            return false;
        }

        IndexBufferHeader ibHeader;
        ibHeader.id_ = format::IndexBufferID;
        ibHeader.format_ = desc.format_;
        ibHeader.size_ = desc.size_;

        void *data = NULL;
        ret = buffer.lock(0, ibHeader.size_, &data);
        if(ret == false){
            return false;
        }

        lcore::io::write(os, ibHeader);
        lcore::io::write(os, data, ibHeader.size_);

        buffer.unlock();
        return true;
    }
#endif
}
}
