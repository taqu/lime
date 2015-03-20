#ifndef INC_LCORE_HASHMAP_H__
#define INC_LCORE_HASHMAP_H__
/**
@file HashMap.h
@author t-sakai
@date 2015/03/13 create
*/
#include "Hash.h"
#include "CLibrary.h"

namespace lcore
{
    struct StringWrapper
    {
        StringWrapper()
            :str_(NULL)
        {}

        explicit StringWrapper(const Char* str, u32 length)
            :length_(length)
            ,str_(str)
        {}

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

        static StringWrapper empty_;

        u32 length_;
        const Char* str_;
    };

    template<>
    struct hasher<StringWrapper>
    {
        inline static size_t calc(const StringWrapper& t)
        {
            const u8* v = reinterpret_cast<const u8*>(t.str_);
            return calc_hash(v, t.length_);
        }
    };

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

    //-----------------------------------------------------------------------------
    //---
    //--- HashMap
    //---
    //-----------------------------------------------------------------------------
    template<class Key,
    class Value,
    class Hasher = lcore::hasher<Key>,
    class KeyAllocator=hash_detail::AllocatorNew<Key>,
    class ValueAllocator=hash_detail::AllocatorNew<Value>,
    class KeyValueAllocator=hash_detail::AllocatorNew<HashMapKeyValuePair<Key, Value> > >
    class HashMap
    {
    public:
        typedef Key key_type;
        typedef Value value_type;
        typedef HashMapKeyValuePair<Key, Value> keyvalue_type;

        typedef HashMap<Key, Value, Hasher, KeyAllocator, ValueAllocator, KeyValueAllocator> this_type;

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
        typedef typename value_traits::param_tye value_param_type;
        typedef typename value_traits::const_param_tye const_value_param_type;


        typedef hash_detail::type_traits<key_type> key_traits;
        typedef typename key_traits::pointer key_pointer;
        typedef typename key_traits::const_pointer const_key_pointer;
        typedef typename key_traits::reference key_reference;
        typedef typename key_traits::const_reference const_key_reference;
        typedef typename key_traits::param_tye key_param_type;
        typedef typename key_traits::const_param_tye const_key_param_type;

        typedef keyvalue_type* keyvalue_pointer;
        typedef const keyvalue_type* const_keyvalue_pointer;
        typedef keyvalue_type& keyvalue_reference;
        typedef const keyvalue_type& const_keyvalue_reference;

        typedef Hasher hasher;

        typedef KeyAllocator key_allocator;
        typedef hash_detail::ArrayAllocator<key_allocator> key_array_allocator;

        typedef ValueAllocator value_allocator;
        typedef hash_detail::ArrayAllocator<value_allocator> value_array_allocator;

        typedef KeyValueAllocator keyvalue_allocator;
        typedef hash_detail::ArrayAllocator<keyvalue_allocator> keyvalue_array_allocator;

        explicit HashMap(size_type capacity, const_key_param_type emptyKey, const_key_param_type deleteKey)
            :size_(0)
            , emptyKey_(emptyKey)
            , deleteKey_(deleteKey)
        {
            capacity = powerOfTwo(capacity);
            create(capacity);
        }

        ~HashMap()
        {
            destroy();
        }

        void setEmptyKey(const_key_reference emptyKey)
        {
            emptyKey_ = emptyKey;
        }

        void setDeleteKey(const_key_reference deleteKey)
        {
            deleteKey_ = deleteKey;
        }

        size_type powerOfTwo(size_type v)
        {
            size_type ret = 16;
            while(ret<v){
                ret <<= 1;
            }
            return ret;
        }

        void initialize(size_type capacity, const_key_param_type emptyKey, const_key_param_type deleteKey)
        {
            destroy();
            capacity = powerOfTwo(capacity);
            setEmptyKey(emptyKey);
            setDeleteKey(deleteKey);
            create(capacity);
        }

        size_type capacity() const
        {
            return capacity_;
        }

        size_type calcHash(const_key_param_type key) const
        {
            return hasher::calc(key) & (capacity_-1);
        }

        void clear();

        bool valid(size_type pos) const
        {
            return (pos<capacity_);
        }

        size_type find(const_key_param_type key) const
        {
            return find_(key, calcHash(key));
        }

        bool insert(const_key_param_type key, const_value_param_type value);

        void erase(size_type pos);
        void swap(this_type& rhs);

        size_type size() const
        {
            return size_;
        }

        size_type begin() const;

        size_type end() const
        {
            return capacity_;
        }

        size_type next(size_type pos) const;

        void expand();

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

        explicit HashMap(const_key_param_type emptyKey, const_key_param_type deleteKey)
            :capacity_(0)
            , size_(0)
            , emptyKey_(emptyKey)
            , deleteKey_(deleteKey)
            , keyvalues_(NULL)
        {
        }

        bool isEmpty(size_type pos) const
        {
            return keyvalues_[pos].key_ == emptyKey_;
        }

        bool isDeleted(size_type pos) const
        {
            return keyvalues_[pos].key_ == deleteKey_;
        }

        size_type findEmptyPosition(size_type hash) const;
        size_type find_(const_key_param_type key, size_type hash) const;

        void create(size_type capacity);
        void destroy();

        size_type capacity_;
        size_type size_;

        key_type emptyKey_;
        key_type deleteKey_;
        keyvalue_pointer keyvalues_;
    };


    template<class Key, class Value, class Hasher, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HashMap<Key, Value, Hasher, KeyAllocator, ValueAllocator, KeyValueAllocator>::clear()
    {
        for(size_type i=0; i<capacity_; ++i){
            if(isEmpty(i)){
                continue;
            }
            if(!isDeleted(i)){
                value_allocator::destruct(&keyvalues_[i].value_);
            }
            keyvalues_[i].key_ = emptyKey_;
        }
        size_ = 0;
    }


    template<class Key, class Value, class Hasher, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    typename HashMap<Key, Value, Hasher, KeyAllocator, ValueAllocator, KeyValueAllocator>::size_type
        HashMap<Key, Value, Hasher, KeyAllocator, ValueAllocator, KeyValueAllocator>::findEmptyPosition(size_type hash) const
    {
            if(capacity_<=size_){
                return end();
            }
            size_type pos = hash;
            size_type n = pos;
            size_type mask = capacity_-1;
            do{
                if(isEmpty(n)){
                    return n;
                }
                if(isDeleted(n)){
                    return n;
                }
                ++n;
                n &= mask;
            } while(n != pos);

            return end();
        }

    template<class Key, class Value, class Hasher, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    typename HashMap<Key, Value, Hasher, KeyAllocator, ValueAllocator, KeyValueAllocator>::size_type
        HashMap<Key, Value, Hasher, KeyAllocator, ValueAllocator, KeyValueAllocator>::find_(const_key_param_type key, size_type hash) const
    {
            size_type pos = hash;
            size_type n = pos;
            size_type mask = capacity_-1;

            do{
                if(isEmpty(n)){
                    return end();
                }

                if(!isDeleted(n) && key == keyvalues_[n].key_){
                    return n;
                }
                ++n;
                n &= mask;
            } while(n != pos);

            return end();
        }

    template<class Key, class Value, class Hasher, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    bool HashMap<Key, Value, Hasher, KeyAllocator, ValueAllocator, KeyValueAllocator>::insert(const_key_param_type key, const_value_param_type value)
    {
        size_type hash = calcHash(key);
        size_type pos = find_(key, hash);
        if(pos != end()){
            return false;
        }

        for(;;){
            pos = findEmptyPosition(hash);
            if(pos == end()){
                expand();
                hash = calcHash(key);
            } else{
                break;
            }
        }

        keyvalues_[pos].key_ = key;
        value_allocator::construct(&keyvalues_[pos].value_, value);
        ++size_;
        return true;
    }

    template<class Key, class Value, class Hasher, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HashMap<Key, Value, Hasher, KeyAllocator, ValueAllocator, KeyValueAllocator>::erase(size_type pos)
    {
        if(!valid(pos)){
            return;
        }
        if(isEmpty(pos) || isDeleted(pos)){
            return;
        }
        value_allocator::destruct(&keyvalues_[pos].value_);
        keyvalues_[pos].key_ = deleteKey_;
        --size_;
    }

    template<class Key, class Value, class Hasher, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HashMap<Key, Value, Hasher, KeyAllocator, ValueAllocator, KeyValueAllocator>::swap(this_type& rhs)
    {
        lcore::swap(capacity_, rhs.capacity_);
        lcore::swap(size_, rhs.size_);
        lcore::swap(emptyKey_, rhs.emptyKey_);
        lcore::swap(deleteKey_, rhs.deleteKey_);
        lcore::swap(keyvalues_, rhs.keyvalues_);
    }


    template<class Key, class Value, class Hasher, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    typename HashMap<Key, Value, Hasher, KeyAllocator, ValueAllocator, KeyValueAllocator>::size_type
        HashMap<Key, Value, Hasher, KeyAllocator, ValueAllocator, KeyValueAllocator>::begin() const
    {
            for(size_type i=0; i<end(); ++i){
                if(!isEmpty(i) && !isDeleted(i)){
                    return i;
                }
            }
            return end();
        }


    template<class Key, class Value, class Hasher, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    typename HashMap<Key, Value, Hasher, KeyAllocator, ValueAllocator, KeyValueAllocator>::size_type
        HashMap<Key, Value, Hasher, KeyAllocator, ValueAllocator, KeyValueAllocator>::next(size_type pos) const
    {
            for(size_type i=pos+1; i<end(); ++i){
                if(!isEmpty(i) && !isDeleted(i)){
                    return i;
                }
            }
            return end();
        }

    template<class Key, class Value, class Hasher, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HashMap<Key, Value, Hasher, KeyAllocator, ValueAllocator, KeyValueAllocator>::expand()
    {
        size_type capacity = capacity_ << 1;
        this_type tmp(emptyKey_, deleteKey_);
        tmp.create(capacity);

        size_type itr = begin();
        while(itr != end()){
            keyvalue_type& keyvalue = keyvalues_[itr];
            if(false == tmp.insert(keyvalue.key_, keyvalue.value_)){
                LASSERT(false);
            }

            itr = next(itr);
        }
        tmp.swap(*this);
    }

    template<class Key, class Value, class Hasher, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HashMap<Key, Value, Hasher, KeyAllocator, ValueAllocator, KeyValueAllocator>::create(size_type capacity)
    {
        LASSERT(NULL == keyvalues_);

        capacity_ = capacity;
        keyvalues_ = keyvalue_array_allocator::allocate(capacity_);
        for(size_type i=0; i<capacity_; ++i){
            key_allocator::construct(&keyvalues_[i].key_, emptyKey_);
        }
    }

    template<class Key, class Value, class Hasher, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HashMap<Key, Value, Hasher, KeyAllocator, ValueAllocator, KeyValueAllocator>::destroy()
    {
        for(size_type i=0; i<capacity_; ++i){
            if(!isEmpty(i) && !isDeleted(i)){
                value_allocator::destruct(&keyvalues_[i].value_);
            }
            key_allocator::destruct(&keyvalues_[i].key_);
        }
        keyvalue_array_allocator::deallocate(keyvalues_);

        capacity_ = size_ = 0;
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
        class Hasher = lcore::hasher<Key>,
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

        typedef HopscotchHashMap<Key, Value, Hasher, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator> this_type;

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
        typedef typename value_traits::param_tye value_param_type;
        typedef typename value_traits::const_param_tye const_value_param_type;


        typedef hash_detail::type_traits<key_type> key_traits;
        typedef typename key_traits::pointer key_pointer;
        typedef typename key_traits::const_pointer const_key_pointer;
        typedef typename key_traits::reference key_reference;
        typedef typename key_traits::const_reference const_key_reference;
        typedef typename key_traits::param_tye key_param_type;
        typedef typename key_traits::const_param_tye const_key_param_type;

        typedef keyvalue_type* keyvalue_pointer;
        typedef const keyvalue_type* const_keyvalue_pointer;
        typedef keyvalue_type& keyvalue_reference;
        typedef const keyvalue_type& const_keyvalue_reference;

        typedef Hasher hasher;

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
            capacity = powerOfTwo(capacity);
            create(capacity);
        }

        ~HopscotchHashMap()
        {
            destroy();
        }

        size_type powerOfTwo(size_type v)
        {
            size_type ret = 16;
            while(ret<v){
                ret <<= 1;
            }
            return ret;
        }

        void initialize(size_type capacity)
        {
            destroy();
            capacity = powerOfTwo(capacity);
            create(capacity);
        }

        size_type capacity() const
        {
            return capacity_;
        }

        size_type calcHash(const_key_param_type key) const
        {
            return hasher::calc(key) & (capacity_-1);
        }

        void clear();

        bool valid(size_type pos) const
        {
            return (pos<capacity_);
        }

        size_type find(const_key_param_type key) const
        {
            return find_(key, calcHash(key));
        }

        bool insert(const_key_param_type key, const_value_param_type value);
        void moveEmpty(size_type& pos, size_type& distance);

        void erase(const_key_param_type key);
        void swap(this_type& rhs);

        size_type size() const
        {
            return size_;
        }

        size_type begin() const;

        size_type end() const
        {
            return capacity_;
        }

        size_type next(size_type pos) const;

        void expand();

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

        size_type find_(const_key_param_type key, size_type hash) const;

        void create(size_type capacity);
        void destroy();

        size_type capacity_;
        size_type size_;

        hopinfo_pointer hopinfoes_;
        keyvalue_pointer keyvalues_;
    };


    template<class Key, class Value, class Hasher, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HopscotchHashMap<Key, Value, Hasher, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::clear()
    {
        for(size_type i=0; i<capacity_; ++i){
            if(hopinfoes_[i].isOccupy()){
                keyvalue_allocator::destruct(&keyvalues_[i]);
            }
            hopinfoes_[i].clear();
        }
        size_ = 0;
    }

    template<class Key, class Value, class Hasher, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    typename HopscotchHashMap<Key, Value, Hasher, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::size_type
        HopscotchHashMap<Key, Value, Hasher, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::find_(const_key_param_type key, size_type hash) const
    {
        size_type pos = hash;
        size_type mask = capacity_-1;

        bitmap_type info = hopinfoes_[pos].getHop();

        while(0!=info){
            if(info & 0x01U){
                LASSERT(isOccupy(pos));
                if(keyvalues_[pos].key_ == key){
                    return pos;
                }
            }
            info >>= 1;
            ++pos;
            pos &= mask;
        }
        return end();
    }

    template<class Key, class Value, class Hasher, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    bool HopscotchHashMap<Key, Value, Hasher, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::insert(const_key_param_type key, const_value_param_type value)
    {
        size_type hash = calcHash(key);
        size_type pos = find_(key, hash);
        if(pos != end()){
            return false;
        }

        size_type mask = capacity_ - 1;
        size_type range = (capacity_<InsertRange)? capacity_ : InsertRange;
        size_type d = 0;
        pos = hash;
        do{
            if(hopinfoes_[pos].isEmpty()){
                break;
            }
            ++pos;
            pos &= mask;
            ++d;
        }while(d<range);

        if(range<=d){
            expand();
            return insert(key, value);
        }

        size_type size = (bitmap_count<capacity_)? bitmap_count : capacity_;
        while(size<=d){
            moveEmpty(pos, d);
        }
        if(end() == pos){
            expand();
            return insert(key, value);
        }

        LASSERT(hopinfoes_[pos].isEmpty());
        LASSERT(!hopinfoes_[hash].checkHopFlag(d));

        hopinfoes_[pos].setOccupy();
        
        key_allocator::construct(&keyvalues_[pos].key_, key);
        value_allocator::construct(&keyvalues_[pos].value_, value);
        hopinfoes_[hash].setHopFlag(d);
        ++size_;
        return true;
    }

    template<class Key, class Value, class Hasher, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HopscotchHashMap<Key, Value, Hasher, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::moveEmpty(size_type& pos, size_type& distance)
    {
        size_type size = (bitmap_count<capacity_)? bitmap_count : capacity_;
        size_type offset = size-1;
        size_type n = (offset<=pos)? pos-offset : capacity_-offset+pos;
        size_type mask = capacity_ - 1;

        for(size_type i=offset; 0<i; --i){
            bitmap_type hop = hopinfoes_[n].getHop();
            for(size_type j=0; j<=i; ++j){
                if(hop & (0x01U<<j)){
                    size_type next_pos = (n+j) & mask;
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
            n &= mask;
        }
        pos = end();
        distance = 0;
    }

    template<class Key, class Value, class Hasher, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HopscotchHashMap<Key, Value, Hasher, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::erase(const_key_param_type key)
    {
        size_type hash = calcHash(key);
        size_type pos = find_(key, hash);

        if(pos == end()){
            return;
        }

        hopinfoes_[pos].setEmpty();
        keyvalue_allocator::destruct(&keyvalues_[pos]);

        size_type d = (hash<=pos)? pos-hash : (capacity_-hash + pos);
        hopinfoes_[hash].clearHopFlag(d);
    }

    template<class Key, class Value, class Hasher, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HopscotchHashMap<Key, Value, Hasher, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::swap(this_type& rhs)
    {
        lcore::swap(capacity_, rhs.capacity_);
        lcore::swap(size_, rhs.size_);
        lcore::swap(hopinfoes_, rhs.hopinfoes_);
        lcore::swap(keyvalues_, rhs.keyvalues_);
    }


    template<class Key, class Value, class Hasher, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    typename HopscotchHashMap<Key, Value, Hasher, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::size_type
        HopscotchHashMap<Key, Value, Hasher, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::begin() const
    {
            for(size_type i=0; i<end(); ++i){
                if(!isEmpty(i)){
                    return i;
                }
            }
            return end();
        }


    template<class Key, class Value, class Hasher, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    typename HopscotchHashMap<Key, Value, Hasher, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::size_type
        HopscotchHashMap<Key, Value, Hasher, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::next(size_type pos) const
    {
        for(size_type i=pos+1; i<end(); ++i){
            if(!isEmpty(i)){
                return i;
            }
        }
        return end();
    }

    template<class Key, class Value, class Hasher, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HopscotchHashMap<Key, Value, Hasher, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::expand()
    {
        LASSERT(0<capacity_);
        size_type capacity = capacity_ << 1;
        this_type tmp;
        tmp.create(capacity);

        size_type itr = begin();
        while(itr != end()){
            keyvalue_type& keyvalue = keyvalues_[itr];
            if(false == tmp.insert(keyvalue.key_, keyvalue.value_)){
                LASSERT(false);
            }

            itr = next(itr);
        }
        tmp.swap(*this);
    }

    template<class Key, class Value, class Hasher, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HopscotchHashMap<Key, Value, Hasher, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::create(size_type capacity)
    {
        LASSERT(NULL == hopinfoes_);
        LASSERT(NULL == keyvalues_);

        capacity_ = capacity;
        hopinfoes_ = hopinfo_array_allocator::allocate(capacity_);
        for(size_type i=0; i<capacity_; ++i){
            hopinfoes_[i].clear();
        }

        keyvalues_ = keyvalue_array_allocator::allocate(capacity_);
    }

    template<class Key, class Value, class Hasher, typename BitmapType, class HopInfoAllocator, class KeyAllocator, class ValueAllocator, class KeyValueAllocator>
    void HopscotchHashMap<Key, Value, Hasher, BitmapType, HopInfoAllocator, KeyAllocator, ValueAllocator, KeyValueAllocator>::destroy()
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
