#ifndef __PLYLOADER_H__
#define __PLYLOADER_H__
/**
@file PlyLoader.h
@author t-sakai
@date 2009/01/29 create
*/
#include <iostream>

namespace lmath
{
    class Vector3;
}

namespace lloader
{
    class PlyLoader
    {
    public:
        PlyLoader();
        ~PlyLoader();

        bool loadHeader(std::istream& istrm);
    private:
        class PlyLoaderImpl;
        PlyLoaderImpl *_impl;
    };
}

#endif //__PLYLOADER_H__
