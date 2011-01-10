#ifndef INC_LCORE_CONTAINER_H__
#define INC_LCORE_CONTAINER_H__
/**
@file Container.h
@author t-sakai
@date 2008/11/13 create
*/
#include "lcore.h"

namespace lcore
{
    /// 派生クラス以外にmemcpyを使わせたくない
    class Container
    {
    protected:
        template<typename SizeType>
        void memoryCopy(void* dst, const void* src, SizeType size)
        {
            memcpy(dst, src, size);
        }
    };
}

#endif //INC_LCORE_CONTAINER_H__
