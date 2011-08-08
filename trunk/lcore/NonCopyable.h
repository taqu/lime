#ifndef INC_LCORE_NONCOPYABLE_H__
#define INC_LCORE_NONCOPYABLE_H__
/**
@file NonCopyable.h
@author t-sakai
@date 2009/04/30 create
*/
namespace lcore
{
    /// コピー禁止
    template<class T>
    class NonCopyable
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
