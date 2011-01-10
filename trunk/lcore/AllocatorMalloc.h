#ifndef INC_LCORE_ALLOCATORMALLOC_H__
#define INC_LCORE_ALLOCATORMALLOC_H__
/**
@file AllocatorMalloc.h
@author t-sakai
@date 2008/11/13 create
*/
#include <stdlib.h>

namespace lcore
{
    /// mallocによるメモリアロケータ
    template<class Type>
    class AllocatorMalloc
    {
    public:
        typedef size_t size_type;
        typedef long long difference_type;
        typedef Type* pointer;
        typedef const Type* const_pointer;
        typedef Type& reference;
        typedef const Type& const_reference;
        typedef Type value_type;


        /// Constructor
        AllocatorMalloc() throw()
        {
        }

        /// Copy constructor
        AllocatorMalloc(const AllocatorMalloc& ) throw()
        {
        }

        /// Copy constructor
        template<class OtherType>
        AllocatorMalloc(const AllocatorMalloc<OtherType>& ) throw()
        {
        }

        /// Destructor
        ~AllocatorMalloc() throw()
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
            return pointer( malloc( size*sizeof(value_type) ) );
        }

        void deallocate(pointer ptr, size_type) const
        {
            if(ptr != 0){
                free(ptr);
            }
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
            typedef AllocatorMalloc<OtherType> other;
        };

    };
}

#endif //INC_LCORE_ALLOCATORMALLOC_H__
