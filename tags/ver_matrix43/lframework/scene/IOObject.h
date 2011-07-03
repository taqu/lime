#ifndef INC_LSCENE_IOOBJECT_H__
#define INC_LSCENE_IOOBJECT_H__
/**
@file IOObject.h
@author t-sakai
@date 2010/05/16 create
*/
#include <lcore/liostream.h>

namespace lscene
{
    class AnimObject;

    class IOObject
    {
    public:
        IOObject(){}
        ~IOObject(){}

        static bool read(lcore::istream& is, AnimObject& obj);
        static bool write(lcore::ostream& os, AnimObject& obj);
    };
}
#endif //INC_LSCENE_IOOBJECT_H__
