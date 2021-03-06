#ifndef INC_LCORE_HASHMAP_H_
#define INC_LCORE_HASHMAP_H_
/**
@file HashMap.h
@author t-sakai
@date 2015/03/13 create
*/
#include "lcore.h"
#include "xxHash.h"

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

    namespace hash_detail
    {
        template<class T>
        inline u32 calcHash(const T& x)
        {
            return xxHash32(reinterpret_cast<const u8*>(&x), sizeof(T));
        }

        inline u32 calcHash(const StringWrapper& x)
        {
            return xxHash32(reinterpret_cast<const u8*>(x.str_), x.length_);
        }
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

        template<typename T>
        u32 log2(T n)
        {
            T x=1;
            u32 p=0;
            while(x<n){
                x <<= 1;
                ++p;
            }
            return p;
        }
    }

    struct HashMapHash
    {
        static const u32 HashMask = 0x7FFFFFFFU;
        static const u32 OccupyFlag = 0x80000000U;

        typedef u32 size_type;

        void clear(){ hash_ = 0;}
        bool isEmpty() const{ return 0 == (hash_&OccupyFlag);}
        bool isOccupy() const{ return 0 != (hash_&OccupyFlag);}
        void setOccupy(){ hash_ |= OccupyFlag;}
        void setOccupy(size_type hash){ hash_ = OccupyFlag|hash;}
        void setEmpty(){ hash_ &= HashMask;}

        size_type hash_;
    };

    template<class Key, class Value>
    struct HashMapKeyValuePair
    {
        typedef Key key_type;
        typedef Value value_type;

        Key key_;
        Value value_;
    };

    struct HashMapKeyBucket
    {
        static const u32 HashMask = 0x7FFFFFFFU;
        static const u32 OccupyFlag = 0x80000000U;

        void clear(){ hash_ = 0;}
        bool isOccupy() const{ return 0 != (hash_&OccupyFlag);}
        void setOccupy(){ hash_ |= OccupyFlag;}
        void setEmpty(){ hash_ &= HashMask;}

        s32 index_;
        s32 next_;
        u32 hash_;
    };

    //-----------------------------------------------------------------------------
    //---
    //--- HashMap
    //---
    //-----------------------------------------------------------------------------
    template<class Key, class Value, class MemoryAllocator=DefaultAllocator>
    class HashMap
    {
    public:
        static const u32 Align = 4;
        static const u32 AlignMask = Align-1;

        typedef Key key_type;
        typedef Value value_type;
        typedef HashMapKeyBucket bucket_type;
        typedef MemoryAllocator memory_allocator;

        typedef HashMap<Key, Value, MemoryAllocator> this_type;

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

        HashMap();
        explicit HashMap(size_type capacity);
        ~HashMap();

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
            return values_[pos];
        }

        const_reference getValue(size_type pos) const
        {
            LASSERT(0<=pos && pos<capacity_);
            return values_[pos];
        }

        key_reference getKey(size_type pos)
        {
            LASSERT(0<=pos && pos<capacity_);
            return keys_[pos];
        }

        const_key_reference getKey(size_type pos) const
        {
            LASSERT(0<=pos && pos<capacity_);
            return keys_[pos];
        }

    private:
        HashMap(const HashMap&) = delete;
        HashMap& operator=(const HashMap&) = delete;

        size_type calcHash_(const_key_param_type key) const
        {
            return hash_detail::calcHash(key) & bucket_type::HashMask;
        }

        static inline size_type align(size_type x)
        {
            return (x+AlignMask)&(~AlignMask);
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
        bucket_type* buckets_;
        key_pointer keys_;
        value_pointer values_;
    };

    template<class Key, class Value, class MemoryAllocator>
    HashMap<Key, Value, MemoryAllocator>::HashMap()
        :capacity_(0)
        ,size_(0)
        ,empty_(0)
        ,freeList_(-1)
        ,buckets_(NULL)
        ,keys_(NULL)
        ,values_(NULL)
    {}

    template<class Key, class Value, class MemoryAllocator>
    HashMap<Key, Value, MemoryAllocator>::HashMap(size_type capacity)
        :capacity_(0)
        ,size_(0)
        ,empty_(0)
        ,freeList_(-1)
        ,buckets_(NULL)
        ,keys_(NULL)
        ,values_(NULL)
    {
        create(capacity);
    }

    template<class Key, class Value, class MemoryAllocator>
    HashMap<Key, Value, MemoryAllocator>::~HashMap()
    {
        destroy();
    }

    template<class Key, class Value, class MemoryAllocator>
    void HashMap<Key, Value, MemoryAllocator>::clear()
    {
        for(size_type i=0; i<capacity_; ++i){
            if(buckets_[i].isOccupy()){
                keys_[i].~key_type();
                values_[i].~value_type();
            }
            buckets_[i].index_ = -1;
            buckets_[i].next_ = -1;
            buckets_[i].clear();
        }
        size_ = 0;
        empty_ = 0;
        freeList_ = -1;
    }

    template<class Key, class Value, class MemoryAllocator>
    typename HashMap<Key, Value, MemoryAllocator>::size_type
        HashMap<Key, Value, MemoryAllocator>::find_(const_key_param_type key, size_type hash) const
    {
        s32 bucketPos = hash % capacity_;
        hash |= bucket_type::OccupyFlag;
        for(s32 i=buckets_[bucketPos].index_; 0<=i; i=buckets_[i].next_){
#if 1
            if(hash == buckets_[i].hash_ && key == keys_[i]){
                return i;
            }
#else
            if(key == keys_[i]){
                return i;
            }
#endif
        }
        return end();
    }

    template<class Key, class Value, class MemoryAllocator>
    bool HashMap<Key, Value, MemoryAllocator>::insert(const_key_param_type key, const_value_param_type value)
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
            freeList_ = buckets_[freeList_].next_;
        }

        s32 bucketPos = hash % capacity_;
        buckets_[entryPos].next_ = (buckets_[bucketPos].index_<0)? -1 : buckets_[bucketPos].index_;
        buckets_[bucketPos].index_ = entryPos;
        buckets_[entryPos].hash_ = hash | bucket_type::OccupyFlag;
        construct(&keys_[entryPos], key);
        construct(&values_[entryPos], value);
        ++size_;
        return true;
    }

    template<class Key, class Value, class MemoryAllocator>
    void HashMap<Key, Value, MemoryAllocator>::erase(const_key_param_type key)
    {
        if(capacity_<=0){
            return;
        }
        u32 rawHash = calcHash_(key);
        size_type pos = find_(key, rawHash);
        if(!valid(pos) || !buckets_[pos].isOccupy()){
            return;
        }
        erase_(pos, rawHash);
    }

    template<class Key, class Value, class MemoryAllocator>
    void HashMap<Key, Value, MemoryAllocator>::eraseAt(size_type pos)
    {
        if(capacity_<=0){
            return;
        }
        if(!valid(pos) || !buckets_[pos].isOccupy()){
            return;
        }
        u32 rawHash = calcHash_(getKey(pos));
        erase_(pos, rawHash);
    }

    template<class Key, class Value, class MemoryAllocator>
    void HashMap<Key, Value, MemoryAllocator>::erase_(size_type pos, u32 rawHash)
    {
        s32 bucketPos = rawHash % capacity_;

        keys_[pos].~key_type();
        values_[pos].~value_type();

        s32 spos = static_cast<s32>(pos);
        if(spos == buckets_[bucketPos].index_){
            buckets_[bucketPos].index_ = buckets_[pos].next_;
        }else{
            s32 p;
            for(p=buckets_[bucketPos].index_; buckets_[p].next_ != spos; p=buckets_[p].next_){
#if _DEBUG
                if(buckets_[p].next_<0){
                    LASSERT(false);
                }
#endif
            }
            buckets_[p].next_ = buckets_[pos].next_;
        }
        buckets_[pos].next_ = freeList_;
        freeList_ = pos;
        buckets_[pos].clear();
        --size_;
    }

    template<class Key, class Value, class MemoryAllocator>
    void HashMap<Key, Value, MemoryAllocator>::swap(this_type& rhs)
    {
        lcore::swap(capacity_, rhs.capacity_);
        lcore::swap(size_, rhs.size_);
        lcore::swap(empty_, rhs.empty_);
        lcore::swap(freeList_, rhs.freeList_);
        lcore::swap(buckets_, rhs.buckets_);
        lcore::swap(keys_, rhs.keys_);
        lcore::swap(values_, rhs.values_);
    }


    template<class Key, class Value, class MemoryAllocator>
    typename HashMap<Key, Value, MemoryAllocator>::size_type
        HashMap<Key, Value, MemoryAllocator>::begin() const
    {
        for(size_type i=0; i<capacity_; ++i){
            if(buckets_[i].isOccupy()){
                return i;
            }
        }
        return end();
    }


    template<class Key, class Value, class MemoryAllocator>
    typename HashMap<Key, Value, MemoryAllocator>::size_type
        HashMap<Key, Value, MemoryAllocator>::next(size_type pos) const
    {
        for(size_type i=pos+1; i<capacity_; ++i){
            if(buckets_[i].isOccupy()){
                return i;
            }
        }
        return end();
    }

    template<class Key, class Value, class MemoryAllocator>
    void HashMap<Key, Value, MemoryAllocator>::expand()
    {
        this_type tmp;
        tmp.create(capacity_+1);


        for(size_type i=0; i<capacity_; ++i){
            if(buckets_[i].isOccupy()){
#if _DEBUG
                if(false == tmp.insert(keys_[i], values_[i])){
                    LASSERT(false);
                }
#else
                tmp.insert(keys_[i], values_[i]);
#endif
            }
        }
        tmp.swap(*this);
    }

    template<class Key, class Value, class MemoryAllocator>
    void HashMap<Key, Value, MemoryAllocator>::create(size_type capacity)
    {
        LASSERT(NULL == buckets_);
        LASSERT(NULL == keys_);
        LASSERT(NULL == values_);

        capacity_ = hash_detail::next_prime(capacity);

        size_type size_buckets = align(sizeof(bucket_type)*capacity_);
        size_type size_keys = align(sizeof(key_type)*capacity_);
        size_type size_values = sizeof(value_type)*capacity_;
        size_type total_size = size_buckets + size_keys + size_values;

        u8* mem = reinterpret_cast<u8*>(LALLOCATOR_MALLOC(memory_allocator, total_size));
        buckets_ = reinterpret_cast<bucket_type*>(mem);
        keys_ = reinterpret_cast<key_pointer>(mem+size_buckets);
        values_ = reinterpret_cast<value_pointer>(mem+size_buckets+size_keys);

        for(size_type i=0; i<capacity_; ++i){
            buckets_[i].index_ = -1;
            buckets_[i].next_ = -1;
            buckets_[i].clear();
        }
        size_ = 0;
        empty_ = 0;
        freeList_ = -1;
    }

    template<class Key, class Value, class MemoryAllocator>
    void HashMap<Key, Value, MemoryAllocator>::destroy()
    {
        clear();
        capacity_ = 0;
        LALLOCATOR_FREE(memory_allocator, buckets_);
        keys_ = NULL;
        values_ = NULL;
    }


    //-----------------------------------------------------------------------------
    //---
    //--- HopscotchHashMap
    //---
    //-----------------------------------------------------------------------------
#if 1
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

    template<class Key, class Value, typename BitmapType=u32, class MemoryAllocator=DefaultAllocator>
    class HopscotchHashMap
    {
    public:
        static const u32 Align = 4;
        static const u32 AlignMask = Align-1;

        typedef HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator> this_type;

        typedef Key key_type;
        typedef Value value_type;
        typedef HashMapHopInfo<BitmapType> hopinfo_type;
        typedef MemoryAllocator memory_allocator;

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

        typedef hopinfo_type* hopinfo_pointer;
        typedef const hopinfo_type* const_hopinfo_pointer;
        typedef typename hopinfo_type::bitmap_type bitmap_type;

        static const size_type bitmap_size = hopinfo_type::bitmap_size;
        static const size_type bitmap_count = hopinfo_type::bitmap_count;

        static const size_type InsertRange = 8*bitmap_count;

        HopscotchHashMap();
        explicit HopscotchHashMap(size_type capacity);
        ~HopscotchHashMap();

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
            return values_[pos];
        }

        const_reference getValue(size_type pos) const
        {
            LASSERT(0<=pos && pos<capacity_);
            return values_[pos];
        }

        key_reference getKey(size_type pos)
        {
            LASSERT(0<=pos && pos<capacity_);
            return keys_[pos];
        }

        const_key_reference getKey(size_type pos) const
        {
            LASSERT(0<=pos && pos<capacity_);
            return keys_[pos];
        }
    private:
        HopscotchHashMap(const HopscotchHashMap&) = delete;
        HopscotchHashMap& operator=(const HopscotchHashMap&) = delete;

        bool isEmpty(size_type pos) const
        {
            return hopinfoes_[pos].isEmpty();
        }

        bool isOccupy(size_type pos) const
        {
            return hopinfoes_[pos].isOccupy();
        }

        inline static size_type calcHash_(const_key_param_type key)
        {
            return hash_detail::calcHash(key);
        }

        inline size_type hashToPos_(size_type hash) const
        {
            return hash%capacity_;
        }

        static inline size_type align_(size_type x)
        {
            return (x+AlignMask)&(~AlignMask);
        }

        inline size_type clamp_(size_type x) const
        {
            return (x<capacity_)? x : x-capacity_;
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
        key_pointer keys_;
        value_pointer values_;
    };

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::HopscotchHashMap()
        :capacity_(0)
        ,size_(0)
        ,hopinfoes_(NULL)
        ,keys_(NULL)
        ,values_(NULL)
    {}

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::HopscotchHashMap(size_type capacity)
        :size_(0)
        ,hopinfoes_(NULL)
        ,keys_(NULL)
        ,values_(NULL)
    {
        create(capacity);
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::~HopscotchHashMap()
    {
        destroy();
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::clear()
    {
        for(size_type i=0; i<capacity_; ++i){
            if(isOccupy(i)){
                keys_[i].~key_type();
                values_[i].~value_type();
            }
            hopinfoes_[i].clear();
        }
        size_ = 0;
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    bool HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::insert(const_key_param_type key, const_value_param_type value)
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

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    typename HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::size_type
        HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::find_(const_key_param_type key, size_type hash) const
    {
        LASSERT(0<capacity_);
        size_type pos = hashToPos_(hash);
        LASSERT(pos<capacity_);
        bitmap_type info = hopinfoes_[pos].getHop();

        size_type d = 0;
        while(0 != info){
            if(info & 0x01U){
                size_type p = clamp_(pos+d);
                LASSERT(isOccupy(p));
                if(keys_[p] == key){
                    return p;
                }
            }
            info >>= 1;
            ++d;
        }

        return end();
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    bool HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::insert_(const_key_param_type key, const_value_param_type value, size_type hash)
    {
        size_type startPos = hashToPos_(hash);
        size_type pos;
        size_type d;
        for(;;){
            size_type range = (capacity_ < InsertRange) ? capacity_ : InsertRange;
            d = 0;
            pos = startPos;
            do{
                if(hopinfoes_[pos].isEmpty()){
                    break;
                }
                pos = clamp_(pos+1);
                ++d;
            } while(d < range);

            if(range <= d){
                expand();
                startPos = hashToPos_(hash);
                continue;
            }

            size_type size = (bitmap_count < capacity_) ? bitmap_count : capacity_;
            while(size <= d){
                moveEmpty(pos, d);
            }
            if(end() == pos){
                expand();
                startPos = hashToPos_(hash);
                continue;
            }
            LASSERT(hopinfoes_[pos].isEmpty());
            LASSERT(!hopinfoes_[startPos].checkHopFlag(d));
            break;
        }

        hopinfoes_[pos].setOccupy();
        construct(&keys_[pos], key);
        construct(&values_[pos], value);
        hopinfoes_[startPos].setHopFlag(d);
        ++size_;
        return true;
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::moveEmpty(size_type& pos, size_type& distance)
    {
        size_type size = (bitmap_count<capacity_)? bitmap_count : capacity_;
        size_type offset = size-1;
        size_type n = (offset<=pos)? pos-offset : capacity_-offset+pos;

        for(s32 i=offset; 0<=i; --i){
            bitmap_type hop = hopinfoes_[n].getHop();
            for(s32 j=0; j<=i; ++j){
                if(hop & (0x01U<<j)){
                    size_type next_pos = clamp_(n+j);
                    construct(&keys_[pos], lcore::move(keys_[next_pos]));
                    construct(&values_[pos], lcore::move(values_[next_pos]));
                    keys_[next_pos].~key_type();
                    values_[next_pos].~value_type();
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
            n = clamp_(n+1);
        }
        pos = end();
        distance = 0;
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::erase(const_key_param_type key)
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
        keys_[pos].~key_type();
        values_[pos].~value_type();

        hash = hashToPos_(hash);
        size_type d = (hash<=pos)? pos-hash : (capacity_-hash + pos);
        hopinfoes_[hash].clearHopFlag(d);
        --size_;
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::eraseAt(size_type pos)
    {
        LASSERT(valid(pos));

        size_type hash = calcHash_(getKey(pos));

        hopinfoes_[pos].setEmpty();
        keys_[pos].~key_type();
        values_[pos].~value_type();

        hash = hashToPos_(hash);
        size_type d = (hash<=pos)? pos-hash : (capacity_-hash + pos);
        hopinfoes_[hash].clearHopFlag(d);
        --size_;
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::swap(this_type& rhs)
    {
        lcore::swap(capacity_, rhs.capacity_);
        lcore::swap(size_, rhs.size_);
        lcore::swap(hopinfoes_, rhs.hopinfoes_);
        lcore::swap(keys_, rhs.keys_);
        lcore::swap(values_, rhs.values_);
    }


    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    typename HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::size_type
        HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::begin() const
    {
        for(size_type i=0; i<end(); ++i){
            if(isOccupy(i)){
                return i;
            }
        }
        return end();
    }


    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    typename HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::size_type
        HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::next(size_type pos) const
    {
        for(size_type i=pos+1; i<end(); ++i){
            if(isOccupy(i)){
                return i;
            }
        }
        return end();
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::expand()
    {
        this_type tmp;
        tmp.create(capacity_+1);

        for(size_type i=0; i<capacity_; ++i){
            if(isOccupy(i)){
                tmp.insert_(keys_[i], values_[i], tmp.calcHash_(keys_[i]));
            }
        }
        tmp.swap(*this);
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::create(size_type capacity)
    {
        LASSERT(NULL == hopinfoes_);
        LASSERT(NULL == keys_);
        LASSERT(NULL == values_);

        capacity_ = hash_detail::next_prime(capacity);

        size_type size_infoes = align_(sizeof(hopinfo_type)*capacity_);
        size_type size_keys = align_(sizeof(key_type)*capacity_);
        size_type size_values = sizeof(value_type)*capacity_;
        size_type total_size = size_infoes + size_keys + size_values;

        u8* mem = reinterpret_cast<u8*>(LALLOCATOR_MALLOC(memory_allocator, total_size));
        hopinfoes_ = reinterpret_cast<hopinfo_pointer>(mem);
        keys_ = reinterpret_cast<key_pointer>(mem+size_infoes);
        values_ = reinterpret_cast<value_pointer>(mem+size_infoes+size_keys);

        lcore::memset(hopinfoes_, 0, size_infoes);
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::destroy()
    {
        clear();
        LALLOCATOR_FREE(memory_allocator, hopinfoes_);
        capacity_ = size_ = 0;
        hopinfoes_ = NULL;
        keys_ = NULL;
        values_ = NULL;
    }

#else
    template<typename BitmapType>
    struct HashMapHopInfo
    {
        typedef u32 size_type;
        typedef BitmapType bitmap_type;
        static const size_type bitmap_size = sizeof(bitmap_type);
        static const size_type bitmap_count = bitmap_size*8;

        bitmap_type getHop() const
        {
            return hop_;
        }

        void setHop(bitmap_type hop)
        {
            hop_ = hop;
        }

        void setHopFlag(size_type pos)
        {
            hop_ |= (0x01U<<pos);
        }

        void clearHopFlag(size_type pos)
        {
            hop_ &= ~(0x01U<<pos);
        }

        bool checkHopFlag(size_type pos) const
        {
            return 0 != (hop_ & (0x01U<<pos));
        }

        void clear()
        {
            hop_ = 0;
        }

        bitmap_type hop_;
    };

    template<class Key, class Value, typename BitmapType=u32, class MemoryAllocator=DefaultAllocator>
    class HopscotchHashMap
    {
    public:
        static const u32 Align = 4;
        static const u32 AlignMask = Align-1;

        typedef HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator> this_type;
        typedef Key key_type;
        typedef Value value_type;
        typedef HashMapHopInfo<BitmapType> hopinfo_type;
        typedef HashMapHash hash_type;
        typedef MemoryAllocator memory_allocator;

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

        typedef hopinfo_type* hopinfo_pointer;
        typedef const hopinfo_type* const_hopinfo_pointer;
        typedef typename hopinfo_type::bitmap_type bitmap_type;

        typedef HashMapHash* hash_pointer;
        static const size_type bitmap_size = hopinfo_type::bitmap_size;
        static const size_type bitmap_count = hopinfo_type::bitmap_count;

        static const size_type InsertRangeFactor = 8;

        HopscotchHashMap();
        explicit HopscotchHashMap(size_type capacity);
        ~HopscotchHashMap();

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
            return (pos<padded_capacity_);
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
            return padded_capacity_;
        }

        size_type next(size_type pos) const;

        reference getValue(size_type pos)
        {
            LASSERT(0<=pos && pos<padded_capacity_);
            return values_[pos];
        }

        const_reference getValue(size_type pos) const
        {
            LASSERT(0<=pos && pos<padded_capacity_);
            return values_[pos];
        }

        key_reference getKey(size_type pos)
        {
            LASSERT(0<=pos && pos<padded_capacity_);
            return keys_[pos];
        }

        const_key_reference getKey(size_type pos) const
        {
            LASSERT(0<=pos && pos<padded_capacity_);
            return keys_[pos];
        }
    private:
        HopscotchHashMap(const HopscotchHashMap&) = delete;
        HopscotchHashMap& operator=(const HopscotchHashMap&) = delete;

        bool isEmpty(size_type pos) const
        {
            return hashes_[pos].isEmpty();
        }

        bool isOccupy(size_type pos) const
        {
            return hashes_[pos].isOccupy();
        }

        inline static size_type calcHash_(const_key_param_type key)
        {
            return hash_detail::calcHash(key);
        }

        inline size_type hashToPos(size_type hash) const
        {
            return hash%capacity_;
        }

        static inline size_type align(size_type x)
        {
            return (x+AlignMask)&(~AlignMask);
        }

        void moveEmpty(size_type& pos, size_type& distance);
        void expand();

        size_type find_(const_key_param_type key, size_type hash) const;
        bool insert_(const_key_param_type key, const_value_param_type value, size_type hash);

        void create(size_type capacity);
        void destroy();

        size_type capacity_;
        size_type padded_capacity_;
        size_type size_;

        hopinfo_pointer hopinfoes_;
        hash_pointer hashes_;
        key_pointer keys_;
        value_pointer values_;
    };

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::HopscotchHashMap()
        :capacity_(0)
        ,padded_capacity_(0)
        ,size_(0)
        ,hopinfoes_(NULL)
        ,hashes_(NULL)
        ,keys_(NULL)
        ,values_(NULL)
    {}

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::HopscotchHashMap(size_type capacity)
        :size_(0)
        ,hopinfoes_(NULL)
        ,hashes_(NULL)
        ,keys_(NULL)
        ,values_(NULL)
    {
        create(capacity);
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::~HopscotchHashMap()
    {
        destroy();
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::clear()
    {
        for(size_type i=0; i<padded_capacity_; ++i){
            if(isOccupy(i)){
                keys_[i].~key_type();
                values_[i].~value_type();
            }
            hopinfoes_[i].clear();
            hashes_[i].clear();
        }
        size_ = 0;
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    bool HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::insert(const_key_param_type key, const_value_param_type value)
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

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    typename HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::size_type
        HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::find_(const_key_param_type key, size_type hash) const
    {
        LASSERT(0<capacity_);
        size_type pos = hashToPos(hash);
        bitmap_type info = hopinfoes_[pos].getHop();
        hash |= hash_type::OccupyFlag;
        while(0 != info){
            LASSERT(pos<padded_capacity_);
            if(hashes_[pos].hash_ == hash){
                LASSERT(isOccupy(pos));
                if(keys_[pos] == key){
                    return pos;
                }
            }
            info >>= 1;
            ++pos;
        }

        return end();
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    bool HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::insert_(const_key_param_type key, const_value_param_type value, size_type hash)
    {
        const size_type InsertRange = InsertRangeFactor*bitmap_count;

        size_type startPos = hashToPos(hash);
        size_type pos;
        size_type d;
        for(;;){
            size_type range = (padded_capacity_ < InsertRange) ? padded_capacity_ : InsertRange;
            if(padded_capacity_<(range+startPos)){
                range = padded_capacity_-startPos;
            }
            d = 0;
            pos = startPos;
            do{
                LASSERT(pos<padded_capacity_);
                if(isEmpty(pos)){
                    break;
                }
                ++pos;
                ++d;
            } while(d < range);

            if(range <= d){
                expand();
                startPos = hashToPos(hash);
                continue;
            }

            LASSERT(!hopinfoes_[pos].checkHopFlag(0));
            LASSERT(startPos == (pos-d));
            LASSERT((startPos+d)<padded_capacity_);
            while(bitmap_count <= d){
                LASSERT(startPos <= ((s64)pos-d));
                moveEmpty(pos, d);
            }
            if(end() == pos){
                expand();
                startPos = hashToPos(hash);
                continue;
            }
            LASSERT(isEmpty(pos));
            break;
        }
        LASSERT(pos<padded_capacity_);
        hashes_[pos].setOccupy(hash);
        construct(&keys_[pos], key);
        construct(&values_[pos], value);
        hopinfoes_[startPos].setHopFlag(d);
        ++size_;
        return true;
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::moveEmpty(size_type& pos, size_type& distance)
    {
        LASSERT(bitmap_count<=distance);
        size_type offset = bitmap_count - 1;
        size_type n = pos - offset;

        for(s32 i=offset; 0<i; --i){
            LASSERT(n<padded_capacity_);
            bitmap_type hop = hopinfoes_[n].getHop();
            for(s32 j=0; j<=i; ++j){
                if(hop & (0x01U<<j)){
                    size_type next_pos = n+j;
                    LASSERT(next_pos<padded_capacity_);
                    //construct(&keys_[pos], keys_[next_pos]);
                    //construct(&values_[pos], values_[next_pos]);
                    construct(&keys_[pos], lcore::move(keys_[next_pos]));
                    construct(&values_[pos], lcore::move(values_[next_pos]));
                    hashes_[pos] = lcore::move(hashes_[next_pos]);
                    keys_[next_pos].~key_type();
                    values_[next_pos].~value_type();
                    hashes_[next_pos].clear();

                    hopinfoes_[n].clearHopFlag(j);
                    hopinfoes_[n].setHopFlag(i);

                    LASSERT(next_pos<pos);
                    distance -= pos-next_pos;
                    pos = next_pos;
                    return;
                }
            }
            ++n;
        }
        pos = end();
        distance = 0;
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::erase(const_key_param_type key)
    {
        if(capacity_<=0){
            return;
        }

        size_type hash = calcHash_(key);
        size_type pos = find_(key, hash);

        if(pos == end()){
            return;
        }

        hashes_[pos].clear();
        keys_[pos].~key_type();
        values_[pos].~value_type();

        size_type hopPos = hashToPos(hash);
        LASSERT(hopPos<=pos);
        //size_type d = (hopPos<=pos)? pos-hopPos : (padded_capacity_-hopPos + pos);
        size_type d = pos-hopPos;
        hopinfoes_[hopPos].clearHopFlag(d);
        --size_;
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::eraseAt(size_type pos)
    {
        LASSERT(valid(pos));

        size_type hash = calcHash_(getKey(pos));

        hashes_[pos].clear();
        keys_[pos].~key_type();
        values_[pos].~value_type();

        size_type hopPos = hashToPos(hash);
        LASSERT(hopPos<=pos);
        //size_type d = (hopPos<=pos)? pos-hopPos : (padded_capacity_-hopPos + pos);
        size_type d = pos-hopPos;
        hopinfoes_[hopPos].clearHopFlag(d);
        --size_;
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::swap(this_type& rhs)
    {
        lcore::swap(capacity_, rhs.capacity_);
        lcore::swap(padded_capacity_, rhs.padded_capacity_);
        lcore::swap(size_, rhs.size_);
        lcore::swap(hopinfoes_, rhs.hopinfoes_);
        lcore::swap(hashes_, rhs.hashes_);
        lcore::swap(keys_, rhs.keys_);
        lcore::swap(values_, rhs.values_);
    }


    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    typename HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::size_type
        HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::begin() const
    {
            for(size_type i=0; i<end(); ++i){
                if(isOccupy(i)){
                    return i;
                }
            }
            return end();
        }


    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    typename HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::size_type
        HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::next(size_type pos) const
    {
        for(size_type i=pos+1; i<end(); ++i){
            if(isOccupy(i)){
                return i;
            }
        }
        return end();
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::expand()
    {
        this_type tmp;
        tmp.create(capacity_+1);

        for(size_type i=0; i<padded_capacity_; ++i){
            if(isOccupy(i)){
                tmp.insert_(keys_[i], values_[i], tmp.calcHash_(keys_[i]));
            }
        }
        tmp.swap(*this);
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::create(size_type capacity)
    {
        LASSERT(NULL == hopinfoes_);
        LASSERT(NULL == keys_);
        LASSERT(NULL == values_);

        capacity_ = hash_detail::next_prime(capacity);
        padded_capacity_ = capacity_ + hash_detail::log2(capacity_)*InsertRangeFactor;

        size_type size_infoes = align(sizeof(hopinfo_type)*padded_capacity_);
        size_type size_hashes = align(sizeof(HashMapHash)*padded_capacity_);
        size_type size_keys = align(sizeof(key_type)*padded_capacity_);
        size_type size_values = sizeof(value_type)*padded_capacity_;
        size_type total_size = size_infoes + size_hashes + size_keys + size_values;

        u8* mem = reinterpret_cast<u8*>(LALLOCATOR_MALLOC(memory_allocator, total_size));
        hopinfoes_ = reinterpret_cast<hopinfo_pointer>(mem);
        hashes_ = reinterpret_cast<hash_pointer>(mem+size_infoes);
        keys_ = reinterpret_cast<key_pointer>(mem+size_infoes+size_hashes);
        values_ = reinterpret_cast<value_pointer>(mem+size_infoes+size_hashes+size_keys);

        for(size_type i=0; i<padded_capacity_; ++i){
            hopinfoes_[i].clear();
            hashes_[i].clear();
        }
    }

    template<class Key, class Value, typename BitmapType, class MemoryAllocator>
    void HopscotchHashMap<Key, Value, BitmapType, MemoryAllocator>::destroy()
    {
        clear();
        LALLOCATOR_FREE(memory_allocator, hopinfoes_);
        capacity_ = padded_capacity_ = size_ = 0;
        hashes_ = NULL;
        keys_ = NULL;
        values_ = NULL;
    }
#endif

    //----------------------------------------------------------------
    //---
    //---
    //---
    //----------------------------------------------------------------
    struct RHDistance
    {
        typedef s8 distance_type;

        distance_type distance_;

        inline distance_type distance() const
        {
            return distance_;
        }

        inline void set(distance_type d)
        {
            distance_ = d;
        }

        inline bool isEmpty() const
        {
            return distance_<0;
        }

        inline bool isOccupy() const
        {
            return 0<=distance_;
        }

        inline void setEmpty()
        {
            distance_ = -1;
        }
    };
}
#endif //INC_LCORE_HASHMAP_H_

