#ifndef INC_LCORE_ALLOCATORNEW_H__
#define INC_LCORE_ALLOCATORNEW_H__
/**
@file AllocatorNew.h
@author t-sakai
@date 2008/11/13 create
*/
#include "lcore.h"

namespace lcore
{
    /// newによるメモリアロケータ
    template<class Type>
    class AllocatorNew
    {
    public:
        typedef size_t size_type;
        typedef long long difference_type;
        typedef Type* pointer;
        typedef const T* const_pointer;
        typedef Type& reference;
        typedef const Type& const_reference;
        typedef Type value_type;


        /// Constructor
        AllocatorNew() throw()
        {
        }

        /// Copy constructor
        AllocatorNew(const AllocatorNew& ) throw()
        {
        }

        /// Copy constructor
        template<class OtherType>
        AllocatorNew(const AllocatorNew<OtherType>& ) throw()
        {
        }

        /// Destructor
        ~AllocatorNew() throw()
        {
        }


        pointer address(reference ref) const
        {
            return &ref;
        }

        const_pointer address(const_reference ref) const
        {
            return &ref;
        }

        size_type max_size() const throw()
        {
            size_type count = (size_type)(-1)/sizeof(value_type);
            return ((0<count)? count : 1);
        }

        pointer allocate(size_type size) const
        {
            return allocate(size, (pointer)0);
        }

        pointer allocate(size_type size, const void*) const
        {
            LASSERT(size > 0);
            return pointer( ::operator new(size * sizeof(value_type)) );
        }

        void deallocate(pointer ptr, size_type) const
        {
            ::operator LIME_DELETE_NONULL(ptr);
        }

        void construct(pointer ptr, const_reference ref) const
        {
            ::new(reinterpret_cast<void*>(ptr)) value_type(ref);
        }

        void destroy(pointer ptr) const
        {
            ptr->~Type();
        }

        template<class OtherType>
        struct rebind
        {
            typedef AllocatorNew<OtherType> other;
        };

    };
}

#endif //INC_LCORE_ALLOCATORNEW_H__
