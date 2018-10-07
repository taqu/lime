#ifndef INC_LCORE_INTRUSIVE_PTR_H_
#define INC_LCORE_INTRUSIVE_PTR_H_
/**
@file intrusive_ptr.h
@author t-sakai
@date 2008/12/18 create
*/
#include "lcore.h"

namespace lcore
{
    //---------------------------------------------------
    //---
    //--- void参照のための特性
    //---
    //---------------------------------------------------
    template<class T>
    struct intrusive_ptr_traits
    {
        typedef T& reference_type;
        typedef const T& const_reference_type;
    };

    template<>
    struct intrusive_ptr_traits<void>
    {
        typedef void reference_type;
        typedef void const_reference_type;
    };

    template<class T>
    class intrusive_ptr
    {
    public:
        typedef intrusive_ptr<T> this_type;
        typedef T value_type;
        typedef T* pointer_type;
        typedef const T* const_pointer_type;
        typedef typename intrusive_ptr_traits<T>::reference_type reference_type;
        typedef typename intrusive_ptr_traits<T>::const_reference_type const_reference_type;

        intrusive_ptr()
            :pointer_(NULL)
        {
        }

        intrusive_ptr(T* pointer)
            :pointer_(pointer)
        {
            if(NULL != pointer_){
                intrusive_ptr_addref(pointer_);
            }
        }

        intrusive_ptr(const intrusive_ptr& rhs)
            :pointer_(rhs.pointer_)
        {
            if(NULL != pointer_){
                intrusive_ptr_addref(pointer_);
            }
        }

        intrusive_ptr(this_type&& rhs)
            :pointer_(rhs.pointer_)
        {
            rhs.pointer_ = NULL;
        }

        template<class U>
        intrusive_ptr(U* ptr)
            :pointer_(ptr)
        {
            if(NULL != pointer_){
                intrusive_ptr_addref(pointer_);
            }
        }

        template<class U>
        intrusive_ptr(intrusive_ptr<U>& rhs)
            :pointer_(rhs.get())
        {
            if(NULL != pointer_){
                intrusive_ptr_addref(pointer_);
            }
        }

        ~intrusive_ptr()
        {
            if(NULL != pointer_){
                intrusive_ptr_release(pointer_);
            }
        }

        const_pointer_type get() const
        {
            return pointer_;
        }

        pointer_type get()
        {
            return pointer_;
        }

        template<class U>
        U* get_static_cast() const
        {
            return static_cast<U*>(pointer_);
        }

        template<class U>
        U* get_reinterpret_cast() const
        {
            return static_cast<U*>(pointer_);
        }

        template<class U>
        U* get_dynamic_cast() const
        {
            return dynamic_cast<U*>(pointer_);
        }

        const_reference_type operator*() const
        {
            return *pointer_;
        }

        reference_type operator*()
        {
            return *pointer_;
        }

        const_pointer_type operator->() const
        {
            return pointer_;
        }

        pointer_type operator->()
        {
            return pointer_;
        }

        operator bool() const
        {
            return (NULL != pointer_);
        }

        bool operator!() const
        {
            return (NULL == pointer_);
        }

        intrusive_ptr& operator=(const this_type& rhs)
        {
            this_type(rhs).swap(*this);
            return *this;
        }

        this_type& operator=(this_type&& rhs);

        intrusive_ptr& operator=(T* rhs)
        {
            this_type(rhs).swap(*this);
            return *this;
        }

        void swap(this_type& rhs);
    private:

        T *pointer_;
    };

    template<class T>
    typename intrusive_ptr<T>::this_type& intrusive_ptr<T>::operator=(this_type&& rhs)
    {
        if(this == &rhs){
            return *this;
        }
        if(NULL != pointer_){
            intrusive_ptr_release(pointer_);
        }
        pointer_ = rhs.pointer_;
        rhs.pointer_ = NULL;
        return *this;
    }

    template<class T>
    void intrusive_ptr<T>::swap(this_type& rhs)
    {
        T* tmp = pointer_;
        pointer_ = rhs.pointer_;
        rhs.pointer_ = tmp;
    }

    template<class T, class U>
    inline bool operator==(const intrusive_ptr<T>& left, const intrusive_ptr<U>& right)
    {
        return left.get() == right.get();
    }

    template<class T, class U>
    inline bool operator!=(const intrusive_ptr<T>& left, const intrusive_ptr<U>& right)
    {
        return left.get() != right.get();
    }

    template<class T, class U>
    inline bool operator==(const intrusive_ptr<T>& left, const U* right)
    {
        return left.get() == right;
    }

    template<class T, class U>
    inline bool operator!=(const intrusive_ptr<T>& left, const U* right)
    {
        return left.get() != right;
    }

    template<class T, class U>
    inline bool operator==(const T* left, const intrusive_ptr<U>& right)
    {
        return left == right.get();
    }

    template<class T, class U>
    inline bool operator!=(const T* left, const intrusive_ptr<U>& right)
    {
        return left != right.get();
    }

    //--- std::null_ptr_t
    //-------------------------------------------------------------------
    template<class T>
    inline bool operator==(const intrusive_ptr<T>& left, const std::nullptr_t right)
    {
        return left.get() == right;
    }

    template<class T>
    inline bool operator!=(const intrusive_ptr<T>& left, const std::nullptr_t right)
    {
        return left.get() != right;
    }

    template<class T>
    inline bool operator==(const std::nullptr_t left, const intrusive_ptr<T>& right)
    {
        return left == right.get();
    }

    template<class T>
    inline bool operator!=(const std::nullptr_t left, const intrusive_ptr<T>& right)
    {
        return left != right.get();
    }

    template<class T>
    inline bool operator<(const intrusive_ptr<T>& left, const intrusive_ptr<T>& right)
    {
        return left.get() < right.get();
    }

    template<class T>
    inline bool operator<=(const intrusive_ptr<T>& left, const intrusive_ptr<T>& right)
    {
        return left.get() <= right.get();
    }

    template<class T>
    inline bool operator>(const intrusive_ptr<T>& left, const intrusive_ptr<T>& right)
    {
        return left.get() > right.get();
    }

    template<class T>
    inline bool operator>=(const intrusive_ptr<T>& left, const intrusive_ptr<T>& right)
    {
        return left.get() >= right.get();
    }

    template<class Dst, class Src>
    inline intrusive_ptr<Dst> static_pointer_cast(const intrusive_ptr<Src>& pointer)
    {
        return static_cast<Dst*>(pointer.get());
    }

    template<class Dst, class Src>
    inline intrusive_ptr<Dst> reinterpret_pointer_cast(const intrusive_ptr<Src>& pointer)
    {
        return static_cast<Dst*>(pointer.get());
    }

    template<class Dst, class Src>
    inline intrusive_ptr<Dst> const_pointer_cast(const intrusive_ptr<Src>& pointer)
    {
        return const_cast<Dst*>(pointer.get());
    }

    template<class Dst, class Src>
    inline intrusive_ptr<Dst> dynamic_pointer_cast(const intrusive_ptr<Src>& pointer)
    {
        return dynamic_cast<Dst*>(pointer.get());
    }

}
#endif //INC_LCORE_INTRUSIVE_PTR_H_
