#ifndef INC_LSOUND_PACKREADER_H_
#define INC_LSOUND_PACKREADER_H_
/**
@file PackReader.h
@author t-sakai
@date 2013/02/12 create
*/
#include "Pack.h"

namespace lsound
{
    class PackReader
    {
    public:
        static bool read(Pack& pack, const Char* path);

    private:
        PackReader() = delete;
        ~PackReader() = delete;

        PackReader(const PackReader&) = delete;
        PackReader(PackReader&&) = delete;
        PackReader& operator=(const PackReader&) = delete;
        PackReader& operator=(PackReader&&) = delete;
    };
}
#endif //INC_LSOUND_PACKREADER_H_
