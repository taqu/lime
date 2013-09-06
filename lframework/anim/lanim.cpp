/**
@file lanim.cpp
@author t-sakai
@date 2013/08/19 create
*/
#include "lanim.h"

namespace lanim
{
    void write(lcore::ostream& os, const Name& name)
    {
        lcore::io::write(os, name.c_str(), name.size()+1);
    }

    void read(Name& name, lcore::istream& is)
    {
        Char tmp[Name::Size];
        Char c;
        s32 count=0;
        for(s32 i=0; i<Name::Size; ++i){
            lcore::io::read(is, c);
            tmp[count] = c;
            if(c == '\0'){
                break;
            }
            ++count;
        }
        name.assign(tmp, count);
    }
}
