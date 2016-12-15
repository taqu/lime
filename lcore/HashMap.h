#ifndef INC_LCORE_HASHMAP_H__
#define INC_LCORE_HASHMAP_H__
/**
@file HashMap.h
@author t-sakai
@date 2015/03/13 create
*/
#include "lcore.h"

namespace lcore
{
    struct StringWrapper
    {
        StringWrapper()
            :length_(0)
            ,str_(NULL)
        {}

        StringWrapper(s32 length, const Char* str)
            :length_(length)
            ,str_(str)
        {}

#if _DEBUG
        ~StringWrapper()
        {
            length_ = 0;
            str_ = NULL;
        }
#endif

        bool operator==(const StringWrapper& rhs) const
        {
            if(rhs.length_ != length_){
                return false;
            }
            return (0 == lcore::memcmp(str_, rhs.str_, length_));
        }

        bool operator!=(const StringWrapper& rhs) const
        {
            if(rhs.length_ != length_){
                return true;
            }
            return (0 != lcore::memcmp(str_, rhs.str_, length_));
        }

        operator const Char*() const{ return str_; }

        s32 length_;
        const Char* str_;
    };

    inline u32 calcHash(const StringWrapper& x)
    {
        return hash_FNV1(reinterpret_cast<const u8*>(x.str_),  x.length_);
    }


    namespace hash_detail
    {
        template<class T>
        struct type_traits
        {
            typedef T* pointer;
            typedef const T* const_pointer;
            typedef T& reference;
            typedef const T& const_reference;
            typedef T& param_type;
            typedef const T& const_param_type;

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
            typedef T* param_type;
            typedef const T* const_param_type;

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
                return pointer(LMALLOC(sizeof(value_type)));
            }

            inline static pointer allocate(size_type count)
            {
                return pointer(LMALLOC(count*sizeof(value_type)));
            }

            inline static void deallocate(pointer ptr)
            {
                LFREE_RAW(ptr);
            }

            inline static void construct(pointer ptr)
            {
                LPLACEMENT_NEW(static_cast<void*>(ptr)) value_type;
            }

            inline static void construct(pointer ptr, const_reference ref)
            {
                LPLACEMENT_NEW(static_cast<void*>(ptr)) value_type(ref);
            }

            inline static void destruct(pointer ptr)
            {
                LASSERT(NULL != ptr);
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
                return ref;
            }

            inline static const_pointer address(const_reference ref)
            {
                return ref;
            }

            inline static size_type max_size()
            {
                size_type count = (size_type)(-1)/sizeof(value_type);
                return ((0<count)? count : 1);
            }

            inline static pointer allocate()
            {
                return pointer(LMALLOC(sizeof(value_type)));
            }

            inline static pointer allocate(size_type count)
            {
                return pointer(LMALLOC(count*sizeof(value_type)));
            }

            inline static void deallocate(pointer ptr)
            {
                LFREE_RAW(ptr);
            }

            inline static void construct(pointer ptr)
            {
                LPLACEMENT_NEW(static_cast<void*>(ptr)) value_type;
            }

            inline static void construct(pointer ptr, const_reference ref)
            {
                LPLACEMENT_NEW(static_cast<void*>(ptr)) value_type(const_cast<reference>(ref));
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


    template<typename BitmapType>
    struct HashMapHopInfo
    {
        typedef u32 size_type;
        typedef BitmapType bitmap_type;
        static const size_type bitmap_size = sizeof(bitmap_type);
        static const size_type bitmap_count = bitmap_size*8 - 1;

        bool isEmpty() const
        {
            return (0 == (hop_&0x01U));
        }

        void setEmpty()
        {
            hop_ &= ~0x01U;
        }

        bool isOccupy() const
        {
            return (0 != (hop_&0x01U));
        }

        void setOccupy()
        {
            hop_ |= 0x01U;
        }

        bitmap_type getHop() const
        {
            return (hop_>>1);
        }

        void setHop(bitmap_type hop)
        {
            hop_ = (hop<<1) | (hop_&0x01U);
        }

        void setHopFlag(size_type pos)
        {
            hop_ |= (0x01U<<(pos+1));
        }

        void clearHopFlag(size_type pos)
        {
            hop_ &= ~(0x01U<<(pos+1));
        }

        bool checkHopFlag(size_type pos) const
        {
            return 0 != (hop_ & (0x01U<<(pos+1)));
        }

        void clear()
        {
            hop_ = 0;
        }

        bitmap_type hop_;
    };

    template<class Key, class Value>
    struct HashMapKeyValuePair
    {
        typedef Key key_type;
        typedef Value value_type;

        Key key_;
        Value value_;
    };

    template<class Key, class Value>
    struct HashMapHashKeyValueTuple
    {
        typedef Key key_type;
        typedef Value value_type;

        static const u32 HashMask = 0x7FFFFFFFU;
        static const u32 OccupyFlag = 0x80000000U;

        void clear(){ hash_ = 0;}
        bool isOccupy() const{ return 0 != (hash_&OccupyFlag);}
        void setOccupy(){ hash_ |= OccupyFlag;}
        void resetOccupy(){ hash_ &= HashMask;}

        s32 next_;
        u32 hash_;
        Key key_;
        Value value_;
    };

    //-----------------------------------------------------------------------------
    //---
    //--- HashMap
    //---
    //-----------------------------------------------------------------------------
    template<class Key,
        class Value,
        class KeyAllocator=hash_detail::AllocatorNew<Key>,
        class ValueAllocator=hash_detail::AllocatorNew<Value>,
        class KeyValueAllocator=hash_detail::AllocatorNew<HashMapHashKeyValueTuple<Key, Value> > >
    class HashMap
    {
    public:
        typedef Key key_type;
        typedef Value value_type;
        typedef HashMapHashKeyValueTuple<Key, Value> keyvalue_type;

        typedef HashMap<Key, Value, KeyAllocator, ValueAllocator, KeyValueAllocator> this_type;

        typedef u32 size_type;

        typedef hash_detail::type_traits<value_type> value_traits;
        typedef typename value_traits::pointer value_pointer;
        typedef typename value_traits::const_pointer const_value_pointer;
        typedef typename value_traits::reference value_reference;
        typedef typename value_traits::const_reference const_value_reference;
        typedef typename value_traits::pointer pointer;
        typedef typename value_traits::const_pointer const_pointer;
        typedef typename value_traits::reference reference;
        typedef typename value_traits::const_reference const_reference;
        typedef typename value_traits::param_type value_param_type;
        typedef typename value_traits::const_param_type const_value_param_type;


        typedef hash_detail::type_traits<key_type> key_traits;
        typedef typename key_traits::pointer key_pointer;
        typedef typename key_traits::const_pointer const_key_pointer;
        typedef typename key_traits::reference key_reference;
        typedef typename key_traits::const_reference const_key_reference;
        typedef typename key_traits::param_type key_param_type;
        typedef typename key_traits::const_param_type const_key_param_type;

        typedef keyvalue_type* keyvalue_pointer;
        typedef const keyvalue_type* const_keyvalue_pointer;
        typedef keyvalue_type& keyvalue_reference;
        typedef const keyvalue_type& const_keyvalue_reference;

        typedef KeyAllocator key_allocator;
        typedef hash_detail::ArrayAllocator<key_allocator> key_array_allocator;

        typedef ValueAllocator value_allocator;
        typedef hash_detail::ArrayAllocator<value_allocator> value_array_allocator;

        typedef KeyValueAllocator keyvalue_allocator;
        typedef hash_detail::ArrayAllocator<keyvalue_allocator> keyvalue_array_allocator;

        HashMap()
            :capacity_(0)
            ,size_(0)
            ,empty_(0)
            ,freeList_(-1)
            ,buckets_(NULL)
            ,keyvalues_(NULL)
        {}

        explicit HashMap(size_type capacity)
            :capacity_(0)
            ,size_(0)
            ,empty_(0)
            ,freeList_(-1)
            ,buckets_(NULL)
            ,keyvalues_(NULL)
        {
            create(capacity);
        }

        ~HashMap()
        {
            destroy();
        }

        void initialize(size_type capacity)
        {
            destroy();
            create(capacity);
        }

        size_type capacity() const
        {
            return capacity_;
        }

        size_type size() const
        {
            return size_;
        }

        void clear();

        bool valid(size_type pos) const
        {
            return (pos<capacity_);
        }

        size_type find(const_key_param_type key) const
        {
            return (0<capacity_)? find_(key, calcHash_(key)) : end();
        }

        bool insert(const_key_param_type key, const_value_param_type value);

        void erase(const_key_param_type key);
        void eraseAt(size_type pos);
        void swap(this_type& rhs);

        size_type begin() const;

        size_type end() const
        {
            return capacity_;
        }

        size_type next(size_type pos) const;

        reference getValue(size_type pos)
        {
            LASSERT(0<=pos && pos<capacity_);
            return keyvalues_[pos].value_;
        }

        const_reference getValue(size_type pos) const
        {
            LASSERT(0<=pos && pos<capacity_);
            return keyvalues_[pos].value_;
        }

        key_reference getKey(size_type pos)
        {
            LASSERT(0<=pos && pos<capacity_);
            return keyvalues_[pos].key_;
        }

        const_key_reference getKey(size_type pos) const
        {
            LASSERT(0<=pos && pos<capacity_);
            return keyvalues_[pos].key_;
        }

    private:
        HashMap(const HashMap&);
        HashMap& operator=(const HashMap&);

        size_type calcHash_(const_key_param_type key) const
        {
            return calcHash(key) & keyvalue_type::HashMask;
        }

        void expand();

        size_type find_(const_key_param_type key, size_type hash) const;
        void erase_(size_type pos, u32 rawHash);

        void create(size_type capacity);
        void destroy();

        size_type capacity_;
        size_type size_;
        s32 empty_;
        s32 freeList_;
        s32* buckets_;
        keyvalue_pointer keyvalues_;
    };


    template<class Key, class Value, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HashMap<Key, Value, KeyAllocator, ValueAllocator, KeyValueAllocator>::clear()
    {
        for(size_type i=0; i<capacity_; ++i){
            buckets_[i] = -1;
            //if(keyvalues_[i].isOccupy()){
            //    value_allocator::destruct(&keyvalues_[i].value_);
            //}
            key_allocator::destruct(&keyvalues_[i].key_);
            value_allocator::destruct(&keyvalues_[i].value_);
            keyvalues_[i].next_ = -1;
            keyvalues_[i].clear();
        }
        size_ = 0;
        empty_ = 0;
        freeList_ = -1;
    }

    template<class Key, class Value, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    typename HashMap<Key, Value, KeyAllocator, ValueAllocator, KeyValueAllocator>::size_type
        HashMap<Key, Value, KeyAllocator, ValueAllocator, KeyValueAllocator>::find_(const_key_param_type key, size_type hash) const
    {
        s32 bucketPos = hash % capacity_;
        hash |= keyvalue_type::OccupyFlag;
        for(s32 i=buckets_[bucketPos]; 0<=i; i=keyvalues_[i].next_){
            if(hash == keyvalues_[i].hash_ && key == keyvalues_[i].key_){
                return i;
            }
        }
        return end();
    }

    template<class Key, class Value, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    bool HashMap<Key, Value, KeyAllocator, ValueAllocator, KeyValueAllocator>::insert(const_key_param_type key, const_value_param_type value)
    {
        size_type hash = calcHash_(key);
        if(0<capacity_ && find_(key, hash) != end()){
            return false;
        }

        s32 entryPos;
        if(freeList_<0){
            for(;;){
                entryPos = (empty_<static_cast<s32>(capacity_))? empty_++ : -1;
                if(0<=entryPos){
                    break;
                }
                expand();
            }
        }else{
            entryPos = freeList_;
            freeList_ = keyvalues_[freeList_].next_;
        }

        s32 bucketPos = hash % capacity_;
        keyvalues_[entryPos].next_ = (buckets_[bucketPos]<0)? -1 : buckets_[bucketPos];
        buckets_[bucketPos] = entryPos;
        keyvalues_[entryPos].hash_ = hash | keyvalue_type::OccupyFlag;
        keyvalues_[entryPos].key_ = key;
        value_allocator::construct(&keyvalues_[entryPos].value_, value);
        ++size_;
        return true;
    }

    template<class Key, class Value, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HashMap<Key, Value, KeyAllocator, ValueAllocator, KeyValueAllocator>::erase(const_key_param_type key)
    {
        if(capacity_<=0){
            return;
        }
        u32 rawHash = calcHash_(key);
        size_type pos = find_(key, rawHash);
        if(!valid(pos) || !keyvalues_[pos].isOccupy()){
            return;
        }
        erase_(pos, rawHash);
    }

    template<class Key, class Value, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HashMap<Key, Value, KeyAllocator, ValueAllocator, KeyValueAllocator>::eraseAt(size_type pos)
    {
        if(capacity_<=0){
            return;
        }
        if(!valid(pos) || !keyvalues_[pos].isOccupy()){
            return;
        }
        u32 rawHash = calcHash_(getKey(pos));
        erase_(pos, rawHash);
    }

    template<class Key, class Value, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HashMap<Key, Value, KeyAllocator, ValueAllocator, KeyValueAllocator>::erase_(size_type pos, u32 rawHash)
    {
        s32 bucketPos = rawHash % capacity_;

        value_allocator::destruct(&keyvalues_[pos].value_);
        key_allocator::destruct(&keyvalues_[pos].key_);

        s32 spos = static_cast<s32>(pos);
        if(spos == buckets_[bucketPos]){
            buckets_[bucketPos] = keyvalues_[pos].next_;
        }else{
            s32 p;
            for(p=buckets_[bucketPos]; keyvalues_[p].next_ != spos; p=keyvalues_[p].next_){
#if _DEBUG
                if(keyvalues_[p].next_<0){
                    LASSERT(false);
                }
#endif
            }
            keyvalues_[p].next_ = keyvalues_[pos].next_;
        }
        keyvalues_[pos].next_ = freeList_;
        freeList_ = pos;
        keyvalues_[pos].clear();
        --size_;
    }

    template<class Key, class Value, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HashMap<Key, Value, KeyAllocator, ValueAllocator, KeyValueAllocator>::swap(this_type& rhs)
    {
        lcore::swap(capacity_, rhs.capacity_);
        lcore::swap(size_, rhs.size_);
        lcore::swap(empty_, rhs.empty_);
        lcore::swap(freeList_, rhs.freeList_);
        lcore::swap(buckets_, rhs.buckets_);
        lcore::swap(keyvalues_, rhs.keyvalues_);
    }


    template<class Key, class Value, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    typename HashMap<Key, Value, KeyAllocator, ValueAllocator, KeyValueAllocator>::size_type
        HashMap<Key, Value, KeyAllocator, ValueAllocator, KeyValueAllocator>::begin() const
    {
        for(size_type i=0; i<capacity_; ++i){
            if(keyvalues_[i].isOccupy()){
                return i;
            }
        }
        return end();
    }


    template<class Key, class Value, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    typename HashMap<Key, Value, KeyAllocator, ValueAllocator, KeyValueAllocator>::size_type
        HashMap<Key, Value, KeyAllocator, ValueAllocator, KeyValueAllocator>::next(size_type pos) const
    {
        for(size_type i=pos+1; i<capacity_; ++i){
            if(keyvalues_[i].isOccupy()){
                return i;
            }
        }
        return end();
    }

    template<class Key, class Value, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HashMap<Key, Value, KeyAllocator, ValueAllocator, KeyValueAllocator>::expand()
    {
        this_type tmp;
        tmp.create(capacity_+1);


        for(size_type i=0; i<capacity_; ++i){
            if(keyvalues_[i].isOccupy()){
#if _DEBUG
                if(false == tmp.insert(keyvalues_[i].key_, keyvalues_[i].value_)){
                    LASSERT(false);
                }
#else
                tmp.insert(keyvalues_[i].key_, keyvalues_[i].value_);
#endif
            }
        }
        tmp.swap(*this);
    }

    template<class Key, class Value, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HashMap<Key, Value, KeyAllocator, ValueAllocator, KeyValueAllocator>::create(size_type capacity)
    {
        LASSERT(NULL == keyvalues_);

        capacity_ = hash_detail::next_prime(capacity);
        buckets_ = LNEW s32[capacity_];
        keyvalues_ = keyvalue_array_allocator::allocate(capacity_);
        for(size_type i=0; i<capacity_; ++i){
            buckets_[i] = -1;
            keyvalues_[i].next_ = -1;
            keyvalues_[i].clear();
        }
        size_ = 0;
        empty_ = 0;
        freeList_ = -1;
    }

    template<class Key, class Value, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HashMap<Key, Value, KeyAllocator, ValueAllocator, KeyValueAllocator>::destroy()
    {
        for(size_type i=0; i<capacity_; ++i){
            //if(keyvalues_[i].isOccupy()){
            //    value_allocator::destruct(&keyvalues_[i].value_);
            //}
            key_allocator::destruct(&keyvalues_[i].key_);
            value_allocator::destruct(&keyvalues_[i].value_);
        }
        capacity_ = size_ = 0;
        empty_ = 0;
        freeList_ = -1;
        LDELETE_ARRAY(buckets_);
        keyvalue_array_allocator::deallocate(keyvalues_);
        keyvalues_ = NULL;
    }


    //-----------------------------------------------------------------------------
    //---
    //--- HopscotchHashMap
    //---
    //-----------------------------------------------------------------------------
    template<
        class Key,
        class Value,
        typename BitmapType=u32,
        class HopInfoAllocator=hash_detail::AllocatorNew<HashMapHopInfo<BitmapType> >,
        class KeyAllocator=hash_detail::AllocatorNew<Key>,
        class ValueAllocator=hash_detail::AllocatorNew<Value>,
        class KeyValueAllocator=hash_detail::AllocatorNew<HashMapKeyValuePair<Key, Value> >
    >
    class HopscotchHashMap
    {
    public:
        typedef Key key_type;
        typedef Value value_type;
        typedef HashMapHopInfo<BitmapType> hopinfo_type;
        typedef HashMapKeyValuePair<Key, Value> keyvalue_type;

        typedef HopscotchHashMap<Key, Value, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator> this_type;

        typedef u32 size_type;

        typedef hash_detail::type_traits<value_type> value_traits;
        typedef typename value_traits::pointer value_pointer;
        typedef typename value_traits::const_pointer const_value_pointer;
        typedef typename value_traits::reference value_reference;
        typedef typename value_traits::const_reference const_value_reference;
        typedef typename value_traits::pointer pointer;
        typedef typename value_traits::const_pointer const_pointer;
        typedef typename value_traits::reference reference;
        typedef typename value_traits::const_reference const_reference;
        typedef typename value_traits::param_type value_param_type;
        typedef typename value_traits::const_param_type const_value_param_type;


        typedef hash_detail::type_traits<key_type> key_traits;
        typedef typename key_traits::pointer key_pointer;
        typedef typename key_traits::const_pointer const_key_pointer;
        typedef typename key_traits::reference key_reference;
        typedef typename key_traits::const_reference const_key_reference;
        typedef typename key_traits::param_type key_param_type;
        typedef typename key_traits::const_param_type const_key_param_type;

        typedef keyvalue_type* keyvalue_pointer;
        typedef const keyvalue_type* const_keyvalue_pointer;
        typedef keyvalue_type& keyvalue_reference;
        typedef const keyvalue_type& const_keyvalue_reference;

        typedef KeyAllocator key_allocator;
        typedef hash_detail::ArrayAllocator<key_allocator> key_array_allocator;

        typedef ValueAllocator value_allocator;
        typedef hash_detail::ArrayAllocator<value_allocator> value_array_allocator;

        typedef KeyValueAllocator keyvalue_allocator;
        typedef hash_detail::ArrayAllocator<keyvalue_allocator> keyvalue_array_allocator;


        typedef hopinfo_type* hopinfo_pointer;
        typedef const hopinfo_type* const_hopinfo_pointer;
        typedef typename hopinfo_type::bitmap_type bitmap_type;
        static const size_type bitmap_size = hopinfo_type::bitmap_size;
        static const size_type bitmap_count = hopinfo_type::bitmap_count;

        static const size_type InsertRange = 8*bitmap_count;

        typedef HopInfoAllocator hopinfo_allocator;
        typedef hash_detail::ArrayAllocator<hopinfo_allocator> hopinfo_array_allocator;

        HopscotchHashMap()
            :capacity_(0)
            ,size_(0)
            ,hopinfoes_(NULL)
            ,keyvalues_(NULL)
        {}

        explicit HopscotchHashMap(size_type capacity)
            :size_(0)
            ,hopinfoes_(NULL)
            ,keyvalues_(NULL)
        {
            create(capacity);
        }

        ~HopscotchHashMap()
        {
            destroy();
        }

        void initialize(size_type capacity)
        {
            destroy();
            create(capacity);
        }

        size_type capacity() const
        {
            return capacity_;
        }

        size_type size() const
        {
            return size_;
        }

        void clear();

        bool valid(size_type pos) const
        {
            return (pos<capacity_);
        }

        size_type find(const_key_param_type key) const
        {
            return (0<capacity_)? find_(key, calcHash_(key)) : end();
        }

        bool insert(const_key_param_type key, const_value_param_type value)
        {
            if(capacity_ <= 0){
                expand();
                return insert_(key, value, calcHash_(key));
            }
            size_type hash = calcHash_(key);
            size_type pos = find_(key, hash);
            if(pos != end()){
                return false;
            }
            return insert_(key, value, hash);
        }

        void erase(const_key_param_type key);
        void eraseAt(size_type pos);
        void swap(this_type& rhs);

        size_type begin() const;

        size_type end() const
        {
            return capacity_;
        }

        size_type next(size_type pos) const;

        reference getValue(size_type pos)
        {
            LASSERT(0<=pos && pos<capacity_);
            return keyvalues_[pos].value_;
        }

        const_reference getValue(size_type pos) const
        {
            LASSERT(0<=pos && pos<capacity_);
            return keyvalues_[pos].value_;
        }

        key_reference getKey(size_type pos)
        {
            LASSERT(0<=pos && pos<capacity_);
            return keyvalues_[pos].key_;
        }

        const_key_reference getKey(size_type pos) const
        {
            LASSERT(0<=pos && pos<capacity_);
            return keyvalues_[pos].key_;
        }
    private:
        HopscotchHashMap(const HopscotchHashMap&);
        HopscotchHashMap& operator=(const HopscotchHashMap&);

        bool isEmpty(size_type pos) const
        {
            return hopinfoes_[pos].isEmpty();
        }

        bool isOccupy(size_type pos) const
        {
            return hopinfoes_[pos].isOccupy();
        }

        size_type calcHash_(const_key_param_type key) const
        {
            return calcHash(key) % capacity_;
        }

        void moveEmpty(size_type& pos, size_type& distance);
        void expand();

        size_type find_(const_key_param_type key, size_type hash) const;
        bool insert_(const_key_param_type key, const_value_param_type value, size_type hash);

        void create(size_type capacity);
        void destroy();

        size_type capacity_;
        size_type size_;

        hopinfo_pointer hopinfoes_;
        keyvalue_pointer keyvalues_;
    };


    template<class Key, class Value, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::clear()
    {
        for(size_type i=0; i<capacity_; ++i){
            if(hopinfoes_[i].isOccupy()){
                keyvalue_allocator::destruct(&keyvalues_[i]);
            }
            hopinfoes_[i].clear();
        }
        size_ = 0;
    }

    template<class Key, class Value, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    typename HopscotchHashMap<Key, Value, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::size_type
        HopscotchHashMap<Key, Value, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::find_(const_key_param_type key, size_type hash) const
    {
        size_type pos = hash;

        bitmap_type info = (pos<capacity_)? hopinfoes_[pos].getHop() : 0;

        while(0 != info){
            if(info & 0x01U){
                LASSERT(isOccupy(pos));
                if(keyvalues_[pos].key_ == key){
                    return pos;
                }
            }
            info >>= 1;
            ++pos;
            pos %= capacity_;
        }

        return end();
    }

    template<class Key, class Value, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    bool HopscotchHashMap<Key, Value, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::insert_(const_key_param_type key, const_value_param_type value, size_type hash)
    {
        size_type pos;
        size_type d;
        for(;;){
            size_type range = (capacity_ < InsertRange) ? capacity_ : InsertRange;
            d = 0;
            pos = hash;
            do{
                if(hopinfoes_[pos].isEmpty()){
                    break;
                }
                ++pos;
                pos %= capacity_;
                ++d;
            } while(d < range);

            if(range <= d){
                expand();
                hash = calcHash_(key);
                continue;
            }

            size_type size = (bitmap_count < capacity_) ? bitmap_count : capacity_;
            while(size <= d){
                moveEmpty(pos, d);
            }
            if(end() == pos){
                expand();
                hash = calcHash_(key);
                continue;
            }
            LASSERT(hopinfoes_[pos].isEmpty());
            LASSERT(!hopinfoes_[hash].checkHopFlag(d));
            break;
        }

        hopinfoes_[pos].setOccupy();

        key_allocator::construct(&keyvalues_[pos].key_, key);
        value_allocator::construct(&keyvalues_[pos].value_, value);
        hopinfoes_[hash].setHopFlag(d);
        ++size_;
        return true;
    }

    template<class Key, class Value, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::moveEmpty(size_type& pos, size_type& distance)
    {
        size_type size = (bitmap_count<capacity_)? bitmap_count : capacity_;
        size_type offset = size-1;
        size_type n = (offset<=pos)? pos-offset : capacity_-offset+pos;

        for(s32 i=offset; 0<=i; --i){
            bitmap_type hop = hopinfoes_[n].getHop();
            for(s32 j=0; j<=i; ++j){
                if(hop & (0x01U<<j)){
                    size_type next_pos = (n+j) % capacity_;
                    keyvalue_allocator::construct(&keyvalues_[pos], keyvalues_[next_pos]);
                    keyvalue_allocator::destruct(&keyvalues_[next_pos]);
                    hopinfoes_[pos].setOccupy();
                    hopinfoes_[next_pos].setEmpty();

                    hop &= ~(0x01U<<j);
                    hop |= (0x01U<<i);
                    hopinfoes_[n].setHop(hop);
                    pos = next_pos;
                    distance -= (i-j);
                    return;
                }
            }
            ++n;
            n %= capacity_;
        }
        pos = end();
        distance = 0;
    }

    template<class Key, class Value, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::erase(const_key_param_type key)
    {
        if(capacity_<=0){
            return;
        }

        size_type hash = calcHash_(key);
        size_type pos = find_(key, hash);

        if(pos == end()){
            return;
        }

        hopinfoes_[pos].setEmpty();
        keyvalue_allocator::destruct(&keyvalues_[pos]);

        size_type d = (hash<=pos)? pos-hash : (capacity_-hash + pos);
        hopinfoes_[hash].clearHopFlag(d);
        --size_;
    }

    template<class Key, class Value, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::eraseAt(size_type pos)
    {
        LASSERT(valid(pos));

        size_type hash = calcHash_(getKey(pos));

        hopinfoes_[pos].setEmpty();
        keyvalue_allocator::destruct(&keyvalues_[pos]);

        size_type d = (hash<=pos)? pos-hash : (capacity_-hash + pos);
        hopinfoes_[hash].clearHopFlag(d);
        --size_;
    }

    template<class Key, class Value, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::swap(this_type& rhs)
    {
        lcore::swap(capacity_, rhs.capacity_);
        lcore::swap(size_, rhs.size_);
        lcore::swap(hopinfoes_, rhs.hopinfoes_);
        lcore::swap(keyvalues_, rhs.keyvalues_);
    }


    template<class Key, class Value, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    typename HopscotchHashMap<Key, Value, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::size_type
        HopscotchHashMap<Key, Value, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::begin() const
    {
            for(size_type i=0; i<end(); ++i){
                if(!isEmpty(i)){
                    return i;
                }
            }
            return end();
        }


    template<class Key, class Value, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    typename HopscotchHashMap<Key, Value, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::size_type
        HopscotchHashMap<Key, Value, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::next(size_type pos) const
    {
        for(size_type i=pos+1; i<end(); ++i){
            if(!isEmpty(i)){
                return i;
            }
        }
        return end();
    }

    template<class Key, class Value, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::expand()
    {
        this_type tmp;
        tmp.create(capacity_+1);

        size_type itr = begin();
        while(itr != end()){
            keyvalue_type& keyvalue = keyvalues_[itr];
            if(false == tmp.insert_(keyvalue.key_, keyvalue.value_, tmp.calcHash_(keyvalue.key_))){
                LASSERT(false);
            }

            itr = next(itr);
        }
        tmp.swap(*this);
    }

    template<class Key, class Value, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::create(size_type capacity)
    {
        LASSERT(NULL == hopinfoes_);
        LASSERT(NULL == keyvalues_);

        capacity_ = hash_detail::next_prime(capacity);
        hopinfoes_ = hopinfo_array_allocator::allocate(capacity_);
        for(size_type i=0; i<capacity_; ++i){
            hopinfoes_[i].clear();
        }

        keyvalues_ = keyvalue_array_allocator::allocate(capacity_);
    }

    template<class Key, class Value, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::destroy()
    {
        clear();
        hopinfo_array_allocator::deallocate(hopinfoes_);
        keyvalue_array_allocator::deallocate(keyvalues_);

        capacity_ = size_ = 0;
        hopinfoes_ = NULL;
        keyvalues_ = NULL;
    }
}
#endif //INC_LCORE_HASHMAP_H__
