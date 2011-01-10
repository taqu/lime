#ifndef INC_LCORE_NONCOPYABLE_H__
#define INC_LCORE_NONCOPYABLE_H__
/**
@file NonCopyable.h
@author t-sakai
@date 2009/04/30 create
*/
#include "Nothing.h"

namespace lcore
{
    /// コピー禁止
    template<class T, class U = lcore::Nothing<T> >
    class NonCopyable : public U
    {
    protected:
        NonCopyable(){}
        ~NonCopyable(){}

    private:
        NonCopyable(const NonCopyable&);
        T& operator=(const T&);
    };
}
#endif //INC_LCORE_NONCOPYABLE_H__
