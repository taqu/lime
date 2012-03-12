/**
@file liostream.cpp
@author t-sakai
@date 2010/07/26 create
*/
#include <stdarg.h>
#include "lcore.h"
#include "liostream.h"

namespace lcore
{
//#if defined(ANDROID)
#if 1

    int ios::ModeInt[Mode_Num] =
    {
        ios::in,
        ios::out,
        ios::app,
        (ios::in | ios::binary),
        (ios::out | ios::binary),
        (ios::app | ios::out | ios::binary),
        (ios::in | ios::out),
        (ios::in | ios::out| ios::trunc),
        (ios::in | ios::out| ios::app),
        (ios::in | ios::out| ios::binary),
        (ios::in | ios::out| ios::trunc|ios::binary),
        (ios::in | ios::out| ios::app|ios::binary),
    };

    const Char* ios::ModeString[] =
    {
        "r",
        "w",
        "a",
        "rb",
        "wb",
        "ab",
        "r+",
        "w+",
        "a+",
        "rb+",
        "wb+",
        "ab+",
    };

    const Char* ios::getModeString(int mode)
    {
        for(int i=0; i<Mode_Num; ++i){
            if(mode == ModeInt[i]){
                return ModeString[i];
            }
        }
        return NULL;
    }

    //----------------------------------------------------------
    //---
    //--- ofstream
    //---
    //----------------------------------------------------------
    int ofstream::print(const Char* format, ... )
    {
        LASSERT(file_ != NULL);
        LASSERT(format != NULL);

        va_list ap;
        va_start(ap, format);

        int ret = vfprintf(file_, format, ap);

        va_end(ap);
        return ret;
    }

    //----------------------------------------------------------
    //---
    //--- sstream_base<istream>
    //---
    //----------------------------------------------------------
    sstream_base<istream>::sstream_base()
        :buffer_(NULL)
        ,current_(0)
        ,capacity_(0)
    {
    }

    sstream_base<istream>::sstream_base(const Char* buffer, u32 size)
        :buffer_(buffer)
        ,current_(0)
        ,capacity_(size)
    {
    }

    bool sstream_base<istream>::eof()
    {
        return (current_>=capacity_);
    }

    bool sstream_base<istream>::good()
    {
        if(buffer_ == NULL){
            return false;
        }
        return (current_<=capacity_);
    }

    bool sstream_base<istream>::seekg(s32 offset, int dir)
    {
        switch(dir)
        {
        case ios::beg:
            if(capacity_<=offset){
                current_ = capacity_;
            }else{
                current_ += offset;
            }
            break;
            
        case ios::cur:
            if(offset>=0){
                if((current_+offset)>=capacity_){
                    current_ = capacity_;
                    break;
                }
            }else{
                if((current_-offset)<0){
                    current_ = 0;
                    break;
                }
            }
            current_ += offset;
            break;
                        
        case ios::end:
            if(capacity_ <= offset){
                current_ = 0;
            }else{
                current_ = capacity_ - offset;
            }
            break;
            
        default:
            break;
        };
        
        return true;
    }

    s32 sstream_base<istream>::tellg()
    {
        return current_;
    }

    lsize_t sstream_base<istream>::read(Char* dst, u32 count)
    {
        s32 end = current_ + count;
        end = (end>capacity_)? capacity_ : end;
        count = end - current_;
        
        while(current_<end){
            *dst = buffer_[current_];
            ++dst;
            ++current_;
        }
        return count;
    }
#endif
}
