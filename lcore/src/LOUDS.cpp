/**
@file LOUDS.cpp
@author t-sakai
@date 2016/08/02 create
*/
#include "LOUDS.h"

namespace lcore
{
namespace
{
    template<class T>
    inline s32 lower(s32 N, const T* v, const T& x)
    {
        s32 first=0;
        s32 n = N;
        while(1<n){
            s32 d = n>>1;
            s32 m = first+d;
            if(v[m]<x){
                first = m;
                n -= d;
            }else{
                n = d;
            }
        }
        return (N<=first)? N-1 : first;
    }

    template<class T>
    inline s32 lower(s32 N, const T* v, s32 BlockSize, const T& x)
    {
        s32 first=0;
        s32 n = N;
        while(1<n){
            s32 d = n>>1;
            s32 m = first+d;
            s32 blockSize = m*BlockSize;
            if((blockSize-v[m])<x){
                first = m;
                n -= d;
            }else{
                n = d;
            }
        }
        return (N<=first)? N-1 : first;
    }
}


    //------------------------------------------------------------
    //---
    //------------------------------------------------------------
    ByteTrie::ByteTrie()
    {
        node_type* node = tree_.getRoot();
        node->setLabel(0);
    }

    ByteTrie::~ByteTrie()
    {
        for(s32 i=0; i<values_.size(); ++i){
            LFREE(values_[i]);
        }
        values_.clear();
    }

    bool ByteTrie::add(const Char* key, const Char* value)
    {
        LASSERT(NULL != key);
        LASSERT(NULL != value);

        node_type* node = tree_.getRoot();
        while(CharNull != *key){
            node_type* next = NULL;
            for(s32 i=0; i<node->getNumChildren(); ++i){
                node_type* child = tree_.get(node->getChild(i));
                if(child->getLabel() == *key){
                    next = child;
                    break;
                }
            }
            if(NULL == next){
                next = tree_.add(node);
                next->setLabel(*key);
            }
            node = next;
            ++key;
        }

        lsize_t len = strlen(value) + 1;
        Char* newValue = (Char*)LMALLOC(sizeof(Char)*len);
        lcore::memcpy(newValue, value, sizeof(Char)*len);

        s32 index = node->getIndex();
        if(0<=index){
            LFREE(values_[index]);
            values_[index] = newValue;
        }else{
            index = values_.size();
            values_.push_back(newValue);
        }
        node->setIndex(index);
        return true;
    }

    const Char* ByteTrie::find(const Char* key) const
    {
        LASSERT(NULL != key);
        const node_type* node = tree_.getRoot();

        while(CharNull != *key){
            const node_type* next = NULL;
            for(s32 i=0; i<node->getNumChildren(); ++i){
                const node_type* child = tree_.get(node->getChild(i));
                if(child->getLabel() == *key){
                    next = child;
                    break;
                }
            }
            if(NULL == next){
                return NULL;
            }
            node = next;
            ++key;
        }
        return (0<=node->getIndex())? values_[node->getIndex()] : NULL;
    }

    //------------------------------------------------------------
    //---
    //------------------------------------------------------------
    FullyIndexableDictionary::FullyIndexableDictionary()
        :sizeInBits_(0)
        ,size_(0)
        ,bits_(NULL)
        ,sizeL_(0)
        ,L1_(NULL)
        ,sizeS_(0)
        ,S1_(NULL)
    {
    }

    FullyIndexableDictionary::FullyIndexableDictionary(u32 sizeInBits)
        :sizeL_(0)
        ,L1_(NULL)
        ,sizeS_(0)
        ,S1_(NULL)
    {
        u32 sizeInBytes = (sizeInBits<UnitBits)? UnitBytes : ((sizeInBits>>3) + UnitBytes)&~(UnitBytes-1);
        sizeInBits_ = sizeInBits;
        size_ = sizeInBytes;
        bits_ = static_cast<u8*>(LMALLOC(sizeInBytes));
        lcore::memset(bits_, 0, sizeInBytes);
    }

    FullyIndexableDictionary::~FullyIndexableDictionary()
    {
        LFREE(L1_);
        LFREE(bits_);
    }

    void FullyIndexableDictionary::clear()
    {
        lcore::memset(bits_, 0, size_);
    }

    void FullyIndexableDictionary::build()
    {
        sizeL_ = (sizeInBits_+(LSize-1))>>LShift;
        sizeS_ = NumSInL*sizeL_;
        LFREE(L1_);
        L1_ = static_cast<u32*>(LMALLOC(sizeL_*sizeof(u32) + sizeS_*sizeof(u8)));
        S1_ = reinterpret_cast<u8*>(L1_+sizeL_);

        L1_[0] = 0;
        u8* b = bits_;
        for(u32 l=1; l<sizeL_; ++l,b+=32){
            u32 count = popcount(reinterpret_cast<u32*>(b));
            L1_[l] = L1_[l-1] + count;
        }

        for(u32 l=0; l<sizeL_; ++l){
            u32 ss = l*NumSInL;
            S1_[ss] = 0;
            ++ss;
            for(u32 s=1; s<NumSInL; ++s,++ss){
                u8 count = populationCount(bits_[ss-1]);
                S1_[ss] = S1_[ss-1] + count;
            }
        }
    }

    u32 FullyIndexableDictionary::access(u32 index) const
    {
        u32 i = index>>3;
        LASSERT(0<=i && i<size_);
        u32 bit = 0x01U << (index&7);
        return (bit & bits_[i]);
    }

    void FullyIndexableDictionary::set(u32 index)
    {
        u32 i = index>>3;
        LASSERT(0<=i && i<size_);
        u32 bit = 0x01U << (index&7);
        bits_[i] |= bit;
    }

    void FullyIndexableDictionary::reset(u32 index)
    {
        u32 i = index>>3;
        LASSERT(0<=i && i<size_);
        u32 bit = 0x01U << (index&7);
        bits_[i] &= ~bit;
    }

    void FullyIndexableDictionary::add(u32 index, bool flag)
    {
        u32 i = index>>3;
        if(size_<=i){
            u32 oldSizeInBytes = size_;
            u32 sizeInBytes = i;
            sizeInBytes = (sizeInBytes+UnitBytes)&~(UnitBytes-1);
            u8* bits = static_cast<u8*>(LMALLOC(sizeInBytes));
            lcore::memset(bits+oldSizeInBytes, 0, sizeInBytes-oldSizeInBytes);
            lcore::memcpy(bits, bits_, oldSizeInBytes);
            LFREE(bits_);
            size_ = sizeInBytes;
            bits_ = static_cast<u8*>(bits);
        }
        if(sizeInBits_<=index){
            sizeInBits_ = index+1;
        }
        u32 bit = 0x01U << (index&7);
        if(flag){
            bits_[i] |= bit;
        } else{
            bits_[i] &= ~bit;
        }
    }

    u32 FullyIndexableDictionary::rank_one(s32 i) const
    {
        LASSERT(0<=i && i<=(s32)sizeInBits());
        u32 b = i>>3;
        u32 l = i>>LShift;
        u32 r0 = i&(LSize-1);
        u32 s = r0>>SShift;
        u32 r1 = r0&(SSize-1);
        s += l*NumSInL;

        u32 mask = (0x01U<<r1)-1;
        return L1_[l]+S1_[s]+populationCount(bits_[b]&mask);
    }

    u32 FullyIndexableDictionary::rank_zero(s32 i) const
    {
        LASSERT(0<=i && i<=(s32)sizeInBits());
        u32 b = i>>3;
        u32 l = i>>LShift;
        u32 r0 = i&(LSize-1);
        u32 s0 = r0>>SShift;
        u32 r1 = r0&(SSize-1);
        u32 s = s0 + l*NumSInL;

        u32 mask = (0x01U<<r1)-1;
        u32 L = (LSize)*l-L1_[l];
        u32 S = (SSize)*s0-S1_[s];
        return L+S+populationCount((~bits_[b])&mask);
    }

    u32 FullyIndexableDictionary::select_one(s32 i) const
    {
        ++i;
        u32 l = lower(sizeL_, L1_, (u32)i);
        i -= L1_[l];
        LASSERT(i<=255);
        s32 offset = NumSInL*l;
        const u8* S = S1_ + offset;
        u32 s = lower(NumSInL, S, static_cast<u8>(i));
        i -= S[s];
        offset += s;

        u8 b = bits_[offset];
        for(s32 j=0; j<SSize; ++j){
            if(b&0x01U){
                --i;
                if(i==0){
                    return (offset<<3)+j;
                }
            }
            b>>=1;
        }
        return sizeInBits();
    }

    u32 FullyIndexableDictionary::select_zero(s32 i) const
    {
        ++i;
        u32 l = lower(sizeL_, L1_, LSize, (u32)i);
        i -= L1_[l];
        LASSERT(i<=255);
        s32 offset = NumSInL*l;
        const u8* S = S1_ + offset;
        u32 s = lower(NumSInL, S, SSize, static_cast<u8>(i));
        i -= S[s];
        offset += s;

        u8 b = bits_[offset];
        for(s32 j=0; j<SSize; ++j){
            if(0 == (b&0x01U)){
                --i;
                if(i==0){
                    return (offset<<3)+j;
                }
            }
            b>>=1;
        }
        return sizeInBits();
    }

    u32 FullyIndexableDictionary::rank_one_brute_force(s32 i) const
    {
        LASSERT(0<=i && i<=(s32)sizeInBits());
        u32 count=0;
        for(s32 j=0; j<i; ++j){
            if(access(j)){
                ++count;
            }
        }
        return count;
    }

    u32 FullyIndexableDictionary::rank_zero_brute_force(s32 i) const
    {
        LASSERT(0<=i && i<=(s32)sizeInBits());
        u32 count=0;
        for(s32 j=0; j<i; ++j){
            if(0==access(j)){
                ++count;
            }
        }
        return count;
    }

    u32 FullyIndexableDictionary::select_one_brute_force(s32 i) const
    {
        ++i;
        s32 count=0;
        u32 size = sizeInBits();
        for(u32 j=0; j<size; ++j){
            if(access(j)){
                ++count;
                if(count == i){
                    return j;
                }
            }
        }
        return size;
    }

    u32 FullyIndexableDictionary::select_zero_brute_force(s32 i) const
    {
        ++i;
        s32 count=0;
        u32 size = sizeInBits();
        for(u32 j=0; j<size; ++j){
            if(0==access(j)){
                ++count;
                if(count == i){
                    return j;
                }
            }
        }
        return size;
    }

    u32 FullyIndexableDictionary::popcount(const u32 v[8]) const
    {
        u32 count = 0;
        for(u32 i=0; i<8; ++i){
            count += populationCount(v[i]);
        }
        return count;
    }

    //------------------------------------------------------------
    //---
    //------------------------------------------------------------
    bool LOUDS::TraverseTrie::operator()(s32 index, const ByteTrie::node_type& node)
    {
        const Char* value = trie_.getValue(node);

        louds_.labels_.push_back(node.getLabel());
        louds_.values_.push_back(NULL);

        if(NULL != value){
            lsize_t len = strlen(value);
            louds_.values_[index] = (Char*)LMALLOC(sizeof(Char)*(len+1));
            lcore::memcpy(louds_.values_[index], value, len+1);
        }

        for(s32 i=0; i<node.getNumChildren(); ++i,++index){
            louds_.fid_.add(true);
        }
        louds_.fid_.add(false);
        return true;
    }

    LOUDS::LOUDS()
    {
    }

    LOUDS::~LOUDS()
    {
        clear();
    }

    void LOUDS::build(const ByteTrie& trie)
    {
        clear();
        TraverseTrie traverseTrie(*this, trie);
        trie.traverse(traverseTrie);
        fid_.build();
    }

    const Char* LOUDS::find(const Char* key) const
    {
        LASSERT(NULL != key);
        static const u32 Invalid = (u32)-1;
        u32 bit = 0;
        u32 index = 0;

        while(CharNull != *key){
            if(fid_.sizeInBits()<=bit){
                return NULL;
            }
            u32 next=Invalid;
            index = fid_.rank_one(bit);
            while(0!=fid_.access(bit)){
                ++index;
                if(labels_[index] == *key){
                    next = bit;
                    break;
                }
                ++bit;
            }
            if(Invalid == next){
                return NULL;
            }
            bit = fid_.select_zero(index-1)+1;
            ++key;
        }
        return values_[index];
    }

    void LOUDS::clear()
    {
        for(s32 i=0; i<values_.size(); ++i){
            LFREE(values_[i]);
        }
        labels_.clear();
        values_.clear();
        fid_.clear();
    }
}
