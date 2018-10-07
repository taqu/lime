#ifndef INC_LFRAMEWORK_HANDLEBAEDALLOCATOR_H_
#define INC_LFRAMEWORK_HANDLEBAEDALLOCATOR_H_
/**
@file HandleBasedAllocator.h
@author t-sakai
@date 2016/08/12 create
*/
#include "lframework.h"

namespace lfw
{
    class HandleBasedAllocator
    {
    public:
//#define LFRAMEWORK_HANDLEBAEDALLOCATOR_DEBUG
        static const s32 Invalid = -1;
        static const s32 PageSize = 8*1024;
        static const s32 MinSize = 16;
        static const u32 MinMask = 15;
        static const u32 MinShift = 4;
        static const s32 MaxSize = 128;
        static const u32 MaxShift = 7;
        static const s32 TableSize = MaxSize/MinSize;

        static const s32 NumFreeAlloc = 16;

        struct Handle
        {
            Handle()
                :size_(0)
                ,offset_(Invalid)
            {}

            Handle(s32 size, s32 offset)
                :size_(size)
                ,offset_(offset)
            {}

            s32 size() const{ return size_;}
            s32 offset() const{ return offset_;}
        private:
            friend class HandleBasedAllocator;

            s32 size_;
            s32 offset_;
        };

        HandleBasedAllocator(u32 pageSize = PageSize);
        ~HandleBasedAllocator();

        Handle allocate(s32 size);
        inline void deallocate(Handle handle)
        {
            deallocate(handle.size(), handle.offset());
        }
        void deallocate(s32 size, s32 offset);

        template<class T>
        inline const T* get(const Handle& handle) const;
        template<class T>
        inline const T* getMaybeNull(const Handle& handle) const;

        template<class T>
        inline T* get(const Handle& handle);
        template<class T>
        inline T* getMaybeNull(const Handle& handle);

        template<class T>
        inline const T* get(s32 size, s32 offset) const;
        template<class T>
        inline const T* getMaybeNull(s32 size, s32 offset) const;

        template<class T>
        inline T* get(s32 size, s32 offset);
        template<class T>
        inline T* getMaybeNull(s32 size, s32 offset);

        void gatherFragments();

        s32 countTable(s32 size) const;
    private:
        HandleBasedAllocator(const HandleBasedAllocator&);
        HandleBasedAllocator& operator=(const HandleBasedAllocator&);

        void setDebug(s32 size, s32 offset);
        bool checkDebug(s32 size, s32 offset) const;

        struct Link
        {
            s32 next_;
        };

        inline const Link* getLink(s32 offset) const
        {
            return reinterpret_cast<const Link*>(block_+offset);
        }

        inline Link* getLink(s32 offset)
        {
            return reinterpret_cast<Link*>(block_+offset);
        }

        inline const Link* getLinkMaybeNull(s32 offset) const
        {
            return (0<=offset)? reinterpret_cast<const Link*>(block_+offset) : NULL;
        }

        inline Link* getLinkMaybeNull(s32 offset)
        {
            return (0<=offset)? reinterpret_cast<Link*>(block_+offset) : NULL;
        }

        inline u32 getDiff(Link* link0, Link* link1)
        {
            return static_cast<u32>(reinterpret_cast<u8*>(link0)-reinterpret_cast<u8*>(link1));
        }

        inline void reservemmap();
        inline void commitmmap();
        inline void freemmap();

        void expand();
        void link(s32 index, Link* link);

        u32 reservedSize_;
        u32 pageSize_;
        u32 allocationGranularity_;
        Link table_[TableSize];
        s32 capacityFreeAlloc_;
        void** freeAllocs_;

        u32 size_;
        u8* block_;
    };

#ifndef LFRAMEWORK_HANDLEBAEDALLOCATOR_DEBUG
    template<class T>
    inline const T* HandleBasedAllocator::get(const Handle& handle) const
    {
        LASSERT(0<=handle.offset());
        return (handle.size()<=MaxSize)
            ? reinterpret_cast<const T*>(block_ + handle.offset())
            : static_cast<const T*>(freeAllocs_[handle.offset()]);
    }

    template<class T>
    inline const T* HandleBasedAllocator::getMaybeNull(const Handle& handle) const
    {
        return (0<=handle.offset())
            ? ((handle.size()<=MaxSize) ? reinterpret_cast<const T*>(block_ + handle.offset()) : static_cast<const T*>(freeAllocs_[handle.offset()]))
            : NULL;
    }

    template<class T>
    inline T* HandleBasedAllocator::get(const Handle& handle)
    {
        LASSERT(0<=handle.offset());
        return (handle.size()<=MaxSize)
            ? reinterpret_cast<T*>(block_ + handle.offset())
            : static_cast<T*>(freeAllocs_[handle.offset()]);
    }

    template<class T>
    inline T* HandleBasedAllocator::getMaybeNull(const Handle& handle)
    {
        return (0<=handle.offset())
            ? ((handle.size()<=MaxSize) ? reinterpret_cast<T*>(block_ + handle.offset()) : static_cast<T*>(freeAllocs_[handle.offset()]))
            : NULL;
    }

    template<class T>
    inline const T* HandleBasedAllocator::get(s32 size, s32 offset) const
    {
        LASSERT(0<=size);
        LASSERT(0<=offset);
        return (size<=MaxSize)
            ? reinterpret_cast<const T*>(block_ + offset)
            : static_cast<const T*>(freeAllocs_[offset]);
    }

    template<class T>
    inline const T* HandleBasedAllocator::getMaybeNull(s32 size, s32 offset) const
    {
        LASSERT(0<=size);
        return (0<=offset)
            ? ((size<=MaxSize) ? reinterpret_cast<const T*>(block_ + offset) : static_cast<const T*>(freeAllocs_[offset]))
            : NULL;
    }

    template<class T>
    inline T* HandleBasedAllocator::get(s32 size, s32 offset)
    {
        LASSERT(0<=size);
        LASSERT(0<=offset);
        return (size<=MaxSize)
            ? reinterpret_cast<T*>(block_ + offset)
            : static_cast<T*>(freeAllocs_[offset]);
    }

    template<class T>
    inline T* HandleBasedAllocator::getMaybeNull(s32 size, s32 offset)
    {
        LASSERT(0<=size);
        return (0<=offset)
            ? ((size<=MaxSize) ? reinterpret_cast<T*>(block_ + offset) : static_cast<T*>(freeAllocs_[offset]))
            : NULL;
    }

#else
    template<class T>
    inline const T* HandleBasedAllocator::get(const Handle& handle) const
    {
        return get<const T*>(handle.size(), handle.offset());
    }

    template<class T>
    inline const T* HandleBasedAllocator::getMaybeNull(const Handle& handle) const
    {
        return getMaybeNull<const T*>(handle.size(), handle.offset());
    }

    template<class T>
    inline T* HandleBasedAllocator::get(const Handle& handle)
    {
        return get<T*>(handle.size(), handle.offset());
    }

    template<class T>
    inline T* HandleBasedAllocator::getMaybeNull(const Handle& handle)
    {
        return getMaybeNull<T*>(handle.size(), handle.offset());
    }

    template<class T>
    inline const T* HandleBasedAllocator::get(s32 size, s32 offset) const
    {
        LASSERT(0<=size);
        LASSERT(0<=offset);
        size += 4;
        return (size<=MaxSize)
            ? reinterpret_cast<const T*>(reinterpret_cast<const u8*>(block_ + offset)+4)
            : reinterpret_cast<const T*>(static_cast<const u8*>(freeAllocs_[offset])+4);
    }

    template<class T>
    inline const T* HandleBasedAllocator::getMaybeNull(s32 size, s32 offset) const
    {
        LASSERT(0<=size);
        size += 4;
        return (0<=offset)
            ? ((size<=MaxSize) ? reinterpret_cast<const T*>(reinterpret_cast<const u8*>(block_ + offset)+4) : reinterpret_cast<const T*>(static_cast<const u8*>(freeAllocs_[offset])+4))
            : NULL;
    }

    template<class T>
    inline T* HandleBasedAllocator::get(s32 size, s32 offset)
    {
        LASSERT(0<=size);
        LASSERT(0<=offset);
        size += 4;
        return (size<=MaxSize)
            ? reinterpret_cast<T*>(reinterpret_cast<u8*>(block_ + offset)+4)
            : reinterpret_cast<T*>(static_cast<u8*>(freeAllocs_[offset])+4);
    }

    template<class T>
    inline T* HandleBasedAllocator::getMaybeNull(s32 size, s32 offset)
    {
        LASSERT(0<=size);
        size += 4;
        return (0<=offset)
            ? ((size<=MaxSize) ? reinterpret_cast<T*>(reinterpret_cast<u8*>(block_ + offset)+4) : reinterpret_cast<T*>(static_cast<u8*>(freeAllocs_[offset])+4))
            : NULL;
    }
#endif
}
#endif //INC_LFRAMEWORK_HANDLEBAEDALLOCATOR_H_
