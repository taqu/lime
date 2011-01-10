#ifndef INC_IOUTIL_H__
#define INC_IOUTIL_H__
/**
@file IOUtil.h
@author t-sakai
@date 2010/08/29 create
*/

namespace util
{
    enum Code
    {
        Code_OK,
        Code_CompressError,
        Code_FileOpenError,
    };

    struct Header
    {
        enum Compress
        {
            Compress_None = 0,
        };
        unsigned int compress_;
    };

    Code write(const char* path, const char* data, unsigned int size, bool compress);

    char* read(const char* path, unsigned int& size);
}

#endif //INC_IOUTIL_H__
