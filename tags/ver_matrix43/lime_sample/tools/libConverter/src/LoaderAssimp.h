#ifndef INC_LOADERASSIMP_H__
#define INC_LOADERASSIMP_H__
/**
@file LoaderAssimp.h
@author t-sakai
@date 2009/08/16 create
*/

namespace lgraphics
{
    class AnimObject;
}

namespace lloader
{
    class LoaderAssimp
    {
    public:
        LoaderAssimp();
        ~LoaderAssimp();

        bool readFile(const char* filename);

        lgraphics::AnimObject* releaseAnimObject();
    private:
        class Impl;
        Impl *impl_;
    };
}

#endif //INC_LOADERASSIMP_H__
