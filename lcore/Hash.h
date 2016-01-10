#ifndef INC_LCORE_HASH_H__
#define INC_LCORE_HASH_H__
/**
@file Hash.h
@author t-sakai
@date 2015/03/13 create
*/
#include "lcore.h"
//#include <string>

namespace lcore
{
    inline u32 calc_hash(const u8* v, u32 count)
    {
        //Bernstein hash
        u32 val = 5381U;

        for(u32 i=0; i<count; ++i){
            //val = 33*val + v[i];
            val = ((val<<5)+val) + v[i];
        }
        return val;
    }

    inline u32 calc_hash_string(const u8* v)
    {
        //Bernstein hash
        u32 val = 5381U;

        u8 c = *v;
        while(c != '\0'){
            //val = 33*val + c;
            val = ((val<<5)+val) + c;
            ++v;
            c = *v;
        }
        return val;
    }

    template<typename T>
    struct hash_type_traits
    {
        static u32 count(const T& /*t*/)
        {
            return sizeof(T); //sizeofが0になることはない
        }
    };

    template<typename T>
    struct hasher
    {
        inline static u32 calc(const T& t)
        {
            const u8* v = reinterpret_cast<const u8*>(&t);
            u32 count = hash_type_traits<T>::count(t);
            return calc_hash(v, count);
        }
    };

#define HASH_TYPE_TRAITS_PRIMITIVE_CREATOR(type) \
    template<>\
    struct hasher<type>\
    {\
        inline static u32 calc(const type t)\
        {\
            const u8* v = reinterpret_cast<const u8*>(&t);\
            u32 count = hash_type_traits<type>::count(t);\
            return calc_hash(v, count);\
        }\
    };\

//HASH_TYPE_TRAITS_PRIMITIVE_CREATOR(char)
//HASH_TYPE_TRAITS_PRIMITIVE_CREATOR(unsigned char)
HASH_TYPE_TRAITS_PRIMITIVE_CREATOR(short)
HASH_TYPE_TRAITS_PRIMITIVE_CREATOR(unsigned short)
HASH_TYPE_TRAITS_PRIMITIVE_CREATOR(int)
HASH_TYPE_TRAITS_PRIMITIVE_CREATOR(unsigned int)
HASH_TYPE_TRAITS_PRIMITIVE_CREATOR(long)
HASH_TYPE_TRAITS_PRIMITIVE_CREATOR(unsigned long)
HASH_TYPE_TRAITS_PRIMITIVE_CREATOR(long long)
HASH_TYPE_TRAITS_PRIMITIVE_CREATOR(unsigned long long)
HASH_TYPE_TRAITS_PRIMITIVE_CREATOR(float)
HASH_TYPE_TRAITS_PRIMITIVE_CREATOR(double)

#undef HASH_TYPE_TRAITS_PRIMITIVE_CREATOR

    // char*, unsigned char* は文字列と見なして特殊化
    template<>
    struct hasher<s8*>
    {
        inline static size_t calc(const s8* t)
        {
            return calc_hash_string(reinterpret_cast<const u8*>(t));
        }
    };

    template<>
    struct hasher<u8*>
    {
        inline static size_t calc(u8* t)
        {
            return calc_hash_string(t);
        }
    };

    template<>
    struct hasher<const s8*>
    {
        inline static size_t calc(const s8* t)
        {
            return calc_hash_string(reinterpret_cast<const u8*>(t));
        }
    };

    template<>
    struct hasher<const u8*>
    {
        inline static size_t calc(const u8* t)
        {
            return calc_hash_string(t);
        }
    };

    namespace hash_detail
    {
        template<class T>
        struct type_traits
        {
            typedef T* pointer;
            typedef const T* const_pointer;
            typedef T& reference;
            typedef const T& const_reference;
            typedef T& param_tye;
            typedef const T& const_param_tye;

            static void swap(T& lhs, T& rhs)
            {
                lcore::swap(lhs, rhs);
            }
        };

        template<class T>
        struct type_traits<T*>
        {
            typedef T** pointer;
            typedef const T** const_pointer;
            typedef T*& reference;
            typedef const T*& const_reference;
            typedef T* param_tye;
            typedef const T* const_param_tye;

            static void swap(T*& lhs, T*& rhs)
            {
                lcore::swap(lhs, rhs);
            }
        };

        template<typename T> struct PrimeList
        {
            static T const value_[];
            static T const length_;
        };

        template<typename T>
        T const PrimeList<T>::value_[] =
        {
            5ul, 11ul, 17ul, 29ul, 37ul, 53ul, 67ul, 79ul,
            97ul, 131ul, 193ul, 257ul, 389ul, 521ul, 769ul,
            1031ul, 1543ul, 2053ul, 3079ul, 6151ul, 12289ul, 24593ul,
            49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
            1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul,
            50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
            1610612741ul, 3221225473ul, 4294967291ul,
        };

        template<typename T>
        T const PrimeList<T>::length_ = 40;

        typedef PrimeList<u32> prime_list;


        template<typename T>
        inline T next_prime(T n)
        {
            const T * const prime_list_begin = PrimeList<T>::value_;
            const T * const prime_list_end = prime_list_begin + PrimeList<T>::length_;
            const T *bound = lower_bound(prime_list_begin, prime_list_end, n);
            if(bound == prime_list_end){
                --bound;
            }
            return *bound;
        }

        template<typename T>
        inline T prev_prime(T n)
        {
            const T * const prime_list_begin = PrimeList<T>::value_;
            const T * const prime_list_end = prime_list_begin + PrimeList<T>::length_;
            const T *bound = upper_bound(prime_list_begin, prime_list_end, n);
            if(bound == prime_list_begin){
                --bound;
            }
            return *bound;
        }

        //------------------------------------------------------
        /// newによるメモリアロケータ
        template<class T>
        class AllocatorNew
        {
        public:
            typedef size_t size_type;
            typedef ptrdiff_t difference_type;
            typedef T* pointer;
            typedef const T* const_pointer;
            typedef T& reference;
            typedef const T& const_reference;
            typedef T value_type;

            inline static pointer address(reference ref)
            {
                return &ref;
            }

            inline static const_pointer address(const_reference ref)
            {
                return &ref;
            }

            inline static size_type max_size()
            {
                size_type count = (size_type)(-1)/sizeof(value_type);
                return ((0<count)? count : 1);
            }

            inline static pointer allocate()
            {
                return pointer(LIME_MALLOC(sizeof(value_type)));
            }

            inline static pointer allocate(size_type count)
            {
                return pointer(LIME_MALLOC(count*sizeof(value_type)));
            }

            inline static void deallocate(pointer ptr)
            {
                LIME_FREE(ptr);
            }

            inline static void construct(pointer ptr)
            {
                LIME_PLACEMENT_NEW(reinterpret_cast<void*>(ptr)) value_type;
            }

            inline static void construct(pointer ptr, const_reference ref)
            {
                LIME_PLACEMENT_NEW(reinterpret_cast<void*>(ptr)) value_type(ref);
            }

            inline static void destruct(pointer ptr)
            {
                LASSERT(NULL != ptr);
                *ptr;
                ptr->~T();
            }
        };

        /// newによるメモリアロケータ
        template<class T>
        class AllocatorNew<T*>
        {
        public:
            typedef size_t size_type;
            typedef ptrdiff_t difference_type;
            typedef T** pointer;
            typedef const T** const_pointer;
            typedef T* reference;
            typedef const T* const_reference;
            typedef T* value_type;

            inline static pointer address(reference ref)
            {
                return &ref;
            }

            inline static const_pointer address(const_reference ref)
            {
                return &ref;
            }

            inline static size_type max_size()
            {
                size_type count = (size_type)(-1)/sizeof(value_type);
                return ((0<count)? count : 1);
            }

            inline static pointer allocate()
            {
                return pointer(LIME_MALLOC(sizeof(value_type)));
            }

            inline static pointer allocate(size_type count)
            {
                return pointer(LIME_MALLOC(count*sizeof(value_type)));
            }

            inline static void deallocate(pointer ptr)
            {
                LIME_FREE(ptr);
            }

            inline static void construct(pointer ptr)
            {
                LIME_PLACEMENT_NEW(reinterpret_cast<void*>(ptr)) value_type;
            }

            inline static void construct(pointer ptr, const_reference ref)
            {
                LIME_PLACEMENT_NEW(reinterpret_cast<void*>(ptr)) value_type(const_cast<reference>(ref));
            }

            inline static void destruct(pointer)
            {
            }
        };


        template<class Allocator>
        class ArrayAllocator
        {
        public:
            typedef Allocator allocator_type;
            typedef typename Allocator::pointer pointer;
            typedef typename Allocator::size_type size_type;
            typedef typename Allocator::value_type value_type;

            inline static pointer allocate(size_type count)
            {
                return allocator_type::allocate(count);
            }

            inline static void deallocate(pointer ptr)
            {
                allocator_type::deallocate(ptr);
            }

            inline static pointer construct(size_type count, pointer ptr)
            {
                pointer ptr = allocator_type::allocate(count);
                for(size_type i=0; i<count; ++i){
                    allocator_type::construct(&(ptr[i]));
                }
                return ptr;
            }

            inline static void destruct(size_type count, pointer ptr)
            {
                for(size_type i=0; i<count; ++i){
                    allocator_type::destruct(&(ptr[i]));
                }
            }
        };
    }
}

#endif //INC_LCORE_HASH_H__
