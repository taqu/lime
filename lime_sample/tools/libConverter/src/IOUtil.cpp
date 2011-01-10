/**
@file IOUtil.cpp
@author t-sakai
@date 2010/08/29 create
*/
#include "stdafx.h"

#include <zlib.h>
#include <lcore/lcore.h>
#include <lcore/liostream.h>
#include "IOUtil.h"

namespace util
{
    using namespace lcore;

    Code write(const char* path, const char* data, unsigned int size, bool compress)
    {
        LASSERT(path != NULL);

        if(compress){
            uLongf dstSize = static_cast<u32>(size*1.1f) + 12;
            ScopedArrayPtr<Bytef> buffer( LIME_NEW Bytef[dstSize] );
            int ret = compress2(buffer.get(), &dstSize, reinterpret_cast<const Bytef*>(data), size, (compress)?Z_DEFAULT_COMPRESSION : Z_NO_COMPRESSION);
            if(ret != Z_OK){
                return Code_CompressError;
            }

            lcore::ofstream out(path, lcore::ios::binary);
            if(out.is_open() == false){
                return Code_FileOpenError;
            }
             Header header;
             header.compress_ = size;
            out.write(reinterpret_cast<const char*>(&header), sizeof(Header));
            out.write(reinterpret_cast<const char*>(buffer.get()), dstSize);
        }else{
             lcore::ofstream out(path, lcore::ios::binary);
            if(out.is_open() == false){
                return Code_FileOpenError;
            }
            Header header;
            header.compress_ = 0;
            out.write(reinterpret_cast<const char*>(&header), sizeof(Header));
            out.write(data, size);
        }

        return Code_OK;
    }

    char* read(const char* path, unsigned int& size)
    {
        LASSERT(path != NULL);
        lcore::ifstream in(path, lcore::ios::binary);
        if(in.is_open() == false){
            return NULL;
        }
        Header header;
        in.read(reinterpret_cast<char*>(&header), sizeof(Header));

        in.seekg(sizeof(Header), lcore::ios::end);
        u32 fileSize = in.tellg();
        in.seekg(sizeof(Header), lcore::ios::beg);

        if(header.compress_ > 0){
            ScopedArrayPtr<char> buffer( LIME_NEW char[fileSize] );
            in.read(buffer.get(), fileSize);

            char *dstBuffer = LIME_NEW char[header.compress_];

            uLongf dstSize = header.compress_;
            int ret = uncompress(reinterpret_cast<Bytef*>(dstBuffer), &dstSize, reinterpret_cast<const Bytef*>(buffer.get()), fileSize);
            if(ret != Z_OK){
                LIME_DELETE_ARRAY(dstBuffer);
                return NULL;
            }
            size = dstSize;
            return dstBuffer;
        }else{
            char *dstBuffer = LIME_NEW char[fileSize];
            in.read(dstBuffer, fileSize);
            size = fileSize;
            return dstBuffer;
        }
    }
}
