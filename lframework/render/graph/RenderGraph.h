#ifndef INC_LFW_RENDERGRAPH_H__
#define INC_LFW_RENDERGRAPH_H__
/**
@file RenderGraph.h
@author t-sakai
@date 2017/06/21 create
*/
#include "lframework.h"
#include <lcore/Array.h>
#include <lcore/HashMap.h>
#include <lgraphics/TextureRef.h>

namespace lfw
{
namespace graph
{
    static const s32 MaxViews = 4;

    enum ResourceType
    {
        ResourceType_2D = 0,
        ResourceType_3D = 1,
        ResourceType_Buffer = 2,
    };

    enum Default
    {
        Default_Clear = 0,
        Default_White,
        Default_Black,
    };

    //------------------------------------------------------
    //---
    //--- ResourceTable
    //---
    //------------------------------------------------------
    /**
    @brief ハッシュ値を外側で管理するハッシュテーブル
    */
    template<class T>
    class ResourceHashMap
    {
    public:
        //template<class T>
        //class has_copy
        //{
        //private:
        //    template<class U>
        //    static auto check(U& u0, const U& u1) -> decltype(static_cast<void>(U::copy(u0,u1)), lcore::true_type());
        //    static auto check(...) -> decltype(lcore::false_type());

        //public:
        //    typedef decltype(check(lcore::declval<T&>(), lcore::declval<const T&>())) type;
        //    static bool const value = type::value;
        //};

        //LSTATIC_ASSERT(lcore::is_pod<T>::value, "T must be POD type");
        //LSTATIC_ASSERT(has_copy<T>::value, "T must have copy");

        typedef ResourceHashMap<T> this_type;
        static const s16 DefaultCapacity = 32;
        static const s16 MaxCapacity = 0x7FFF;

        ResourceHashMap();
        explicit ResourceHashMap(s16 capacity);
        ResourceHashMap(this_type&& rhs);
        ~ResourceHashMap();

        void clear();
        s16 add(u32 hash);
        void remove(u32 hash);

        /**
        @brief 要素を検索
        @return 見つからなければend()
        */
        s16 find(u32 hash) const;
        inline const T& get(s16 index) const;
        inline T& get(s16 index);
        inline u32 getHash(s16 index) const;
        s16 begin() const;
        s16 next(s16 index) const;
        inline s16 end() const;

        void swap(this_type& rhs);
        this_type& operator=(this_type&& rhs);
    private:
        ResourceHashMap(const this_type&) = delete;
        this_type& operator=(const this_type&) = delete;

        struct Entry
        {
            void clear()
            {
                next_ = -1;
                entry_ = -1;
                flag_ = 0;
                hash_ = 0;
            }

            s16 next_;
            s16 entry_;
            u32 flag_;
            u32 hash_;
            T resource_;
        };

        s16 find(u32 hash, s16 pos) const;

        void create(s16 capacity);
        void expand();

        s16 capacity_;
        s16 size_;
        s16 empty_;
        s16 freeList_;
        Entry* entries_;
    };

    template<class T>
    ResourceHashMap<T>::ResourceHashMap()
        :size_(0)
        ,empty_(0)
        ,freeList_(-1)
        ,entries_(NULL)
    {
        create(DefaultCapacity);
    }

    template<class T>
    ResourceHashMap<T>::ResourceHashMap(s16 capacity)
        :size_(0)
        ,entries_(NULL)
    {
        LASSERT(0<capacity);
        create(capacity);
    }

    template<class T>
    ResourceHashMap<T>::ResourceHashMap(this_type&& rhs)
        :capacity_(rhs.capacity_)
        ,size_(rhs.size_)
        ,empty_(rhs.empty_)
        ,freeList_(rhs.freeList_)
        ,entries_(rhs.entries_)
    {
        rhs.capacity_ = 0;
        rhs.size_ = 0;
        rhs.empty_ = 0;
        rhs.freeList_ = -1;
        rhs.entries_ = NULL;
    }

    template<class T>
    ResourceHashMap<T>::~ResourceHashMap()
    {
        clear();
        LFREE(entries_);
        capacity_ = 0;
    }

    template<class T>
    void ResourceHashMap<T>::clear()
    {
        for(s16 i=0; i<capacity_;++i){
            if(0 != entries_[i].flag_){
                entries_[i].resource_.~T();
            }
            entries_[i].clear();
        }
        size_ = 0;
        empty_ = 0;
        freeList_ = -1;
    }

    template<class T>
    s16 ResourceHashMap<T>::add(u32 hash)
    {
        LASSERT(0<capacity_ && capacity_<=MaxCapacity);
        s16 bucketPos = hash % capacity_;
        s16 pos = find(hash, bucketPos);
        if(pos != end()){
            return pos;
        }

        s16 entryPos;
        if(freeList_<0){
            for(;;){
                if(capacity_<=empty_){
                    expand();
                    bucketPos = hash % capacity_;
                }else{
                    entryPos = empty_;
                    ++empty_;
                    break;
                }
            }
        }else{
            entryPos = freeList_;
            freeList_ = entries_[freeList_].next_;
        }

        entries_[entryPos].next_ = entries_[bucketPos].entry_;
        entries_[bucketPos].entry_ = entryPos;
        entries_[entryPos].hash_ = hash;
        entries_[entryPos].flag_ = 1;
        LPLACEMENT_NEW(&entries_[entryPos].resource_) T;
        ++size_;
        return entryPos;
    }

    template<class T>
    void ResourceHashMap<T>::remove(u32 hash)
    {
        LASSERT(0<capacity_);

        s16 bucketPos = hash % capacity_;
        s16 pos = find(hash, bucketPos);
        if(end()==pos){
            return;
        }

        if(pos == entries_[bucketPos].entry_){
            entries_[bucketPos].entry_ = entries_[pos].next_;
        }else{
            s16 p;
            for(p=entries_[bucketPos].entry_; entries_[p].next_ != pos; p=entries_[p].next_){
            }
            entries_[p].next_ = entries_[pos].next_;
        }
        entries_[pos].resource_.~T();
        entries_[pos].clear();
        entries_[pos].next_ = freeList_;
        freeList_ = pos;
        --size_;
    }

    template<class T>
    s16 ResourceHashMap<T>::find(u32 hash) const
    {
        LASSERT(0<capacity_);
        return find(hash, hash%capacity_);
    }

    template<class T>
    s16 ResourceHashMap<T>::find(u32 hash, s16 pos) const
    {
        for(s16 i=entries_[pos].entry_; 0<=i; i=entries_[i].next_){
            if(hash == entries_[i].hash_){
                return i;
            }
        }
        return end();
    }

    template<class T>
    inline const T& ResourceHashMap<T>::get(s16 index) const
    {
        LASSERT(0<=index && index<capacity_);
        return entries_[index].resource_;
    }

    template<class T>
    inline T& ResourceHashMap<T>::get(s16 index)
    {
        LASSERT(0<=index && index<capacity_);
        return entries_[index].resource_;
    }

    template<class T>
    inline u32 ResourceHashMap<T>::getHash(s16 index) const
    {
        return entries_[index].hash_;
    }

    template<class T>
    s16 ResourceHashMap<T>::begin() const
    {
        for(s16 i=0; i<capacity_;++i){
            if(entries_[i].flag_ != 0){
                return i;
            }
        }
        return end();
    }

    template<class T>
    s16 ResourceHashMap<T>::next(s16 index) const
    {
        for(s16 i=index+1; i<capacity_;++i){
            if(entries_[i].flag_ != 0){
                return i;
            }
        }
        return end();
    }

    template<class T>
    inline s16 ResourceHashMap<T>::end() const
    {
        return capacity_;
    }

    template<class T>
    void ResourceHashMap<T>::swap(this_type& rhs)
    {
        lcore::swap(capacity_, rhs.capacity_);
        lcore::swap(size_, rhs.size_);
        lcore::swap(empty_, rhs.empty_);
        lcore::swap(freeList_, rhs.freeList_);
        lcore::swap(entries_, rhs.entries_);
    }

    template<class T>
    void ResourceHashMap<T>::create(s16 capacity)
    {
        LASSERT(0<=capacity && capacity<=MaxCapacity);
        LASSERT(NULL == entries_);

        capacity_ = static_cast<s16>( lcore::hash_detail::next_prime(static_cast<u32>(capacity)) );
        entries_ = reinterpret_cast<Entry*>(LMALLOC(sizeof(Entry)*capacity_));
        for(s16 i=0; i<capacity_; ++i){
            entries_[i].clear();
        }
        size_ = 0;
        empty_ = 0;
        freeList_ = -1;
    }

    template<class T>
    void ResourceHashMap<T>::expand()
    {
        LASSERT(capacity_<=MaxCapacity);
        this_type resourceHashMap(capacity_+1);

        for(s16 i=0; i<capacity_;++i){
            for(s16 j=entries_[i].entry_; 0<=j; j=entries_[j].next_){
                s16 pos = resourceHashMap.add(entries_[j].hash_);
                LASSERT(pos != resourceHashMap.end());
                resourceHashMap.get(pos) = lcore::move(entries_[j].resource_);
            }
        }
        resourceHashMap.swap(*this);
    }

    template<class T>
    typename ResourceHashMap<T>::this_type& ResourceHashMap<T>::operator=(this_type&& rhs)
    {
        if(this != &rhs){
            this->~ResourceHashMap();
            swap(rhs);
        }
        return *this;
    }

    //------------------------------------------------------
    //---
    //--- ResourceParam
    //---
    //------------------------------------------------------
    struct ResourceParam
    {
        u32 calcHash() const;

        u16 type_;
        u16 depth_;
        u16 width_;
        u16 height_;
        u16 mipLevels_;
        u16 arraySize_;
        u32 format_;
        u32 bind_;
        u32 structureByteStride_;
        u32 misc_;

        static ResourceParam create2D(
            u16 width,
            u16 height,
            u16 mipLevels,
            u16 arraySize,
            u32 format,
            u32 bind,
            u32 misc=0);

        static ResourceParam create3D(
            u16 width,
            u16 height,
            u16 depth,
            u16 mipLevels,
            u16 arraySize,
            u32 format,
            u32 bind,
            u32 misc=0);

        static ResourceParam createBuffer(
            u32 size,
            u32 bind,
            u32 structureByteStride,
            u32 misc=0);

        //static ResourceParam createDepthStencil(
        //    u32 width,
        //    u32 height,
        //    u32 format,
        //    u32 bind,
        //    u32 misc=0);

    };

    bool operator==(const ResourceParam& x0, const ResourceParam& x1);

    //------------------------------------------------------
    //---
    //--- ViewParam
    //---
    //------------------------------------------------------
    enum ViewType
    {
        ViewType_SRV =0,
        ViewType_RTV,
        ViewType_DSV,
        ViewType_UAV,
    };

    struct ViewParam
    {
        u32 calcHash() const;

        s32 type_;
        union
        {
            lgfx::SRVDesc srv_;
            lgfx::RTVDesc rtv_;
            lgfx::DSVDesc dsv_;
            lgfx::UAVDesc uav_;
        };
    };

    bool operator==(const ViewParam& x0, const ViewParam& x1);

    struct Resource
    {
        void clear()
        {
            numViews_ = 0;
        }
        ResourceParam param_;
        lgfx::ResourceRef resource_;
        s32 numViews_;
        ViewParam viewParams_[MaxViews];
        lgfx::ViewRef views_[MaxViews];
    };

    //---------------------------------------------------------
    //---
    //--- RenderGraph
    //---
    //---------------------------------------------------------
    class RenderGraph
    {
    public:
        static u32 hashID(const Char* name);

        RenderGraph();
        RenderGraph(RenderGraph&& rhs);
        ~RenderGraph();

        void clear();
        void clearShared();

        bool getTemporary(lgfx::ViewRef& view, const ResourceParam& resParam, const ViewParam& viewParam);
        bool setShared(u32 id, lgfx::ViewRef& view);
        bool setShared(u32 id, lgfx::ViewRef&& view);
        lgfx::ViewRef getShared(u32 id);
        bool existShared(u32 id);
        RenderGraph& operator=(RenderGraph&& rhs);
    private:
        RenderGraph(const RenderGraph&) = delete;
        RenderGraph& operator=(const RenderGraph&) = delete;

        bool createResource(lgfx::ResourceRef& resource, const ResourceParam& resParam);
        bool createView(lgfx::ViewRef& view, lgfx::ResourceRef& resource, const ViewParam& viewParam);

        ResourceHashMap<Resource> resourceMap_;
        ResourceHashMap<lgfx::ViewRef> viewMap_;
    };
}
}
#endif //INC_LFW_RENDERGRAPH_H__
