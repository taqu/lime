#ifndef INC_LFRAMEWORK_HANDLEBAEDALLOCATOR_H__
#define INC_LFRAMEWORK_HANDLEBAEDALLOCATOR_H__
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
        static const s32 Invalid = -1;
        static const u32 PageSize = 8*1024;
        static const u32 MinSize = 16;
        static const u32 MinMask = 15;
        static const u32 MinShift = 4;
        static const u32 MaxSize = 128;
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

        Handle allocate(u32 size);
        void deallocate(Handle handle);

        template<class T>
        inline const T* get(const Handle& handle) const;
        template<class T>
        inline const T* getMaybeNull(const Handle& handle) const;

        template<class T>
        inline T* get(const Handle& handle);
        template<class T>
        inline T* getMaybeNull(const Handle& handle);

        template<class T>
        inline const T* get(s32 offset) const;
        template<class T>
        inline const T* getMaybeNull(s32 offset) const;

        template<class T>
        inline T* get(s32 offset);
        template<class T>
        inline T* getMaybeNull(s32 offset);

        void gatherFragments();

        s32 countTable(u32 size) const;
    private:
        HandleBasedAllocator(const HandleBasedAllocator&);
        HandleBasedAllocator& operator=(const HandleBasedAllocator&);

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

    template<class T>
    inline const T* HandleBasedAllocator::get(const Handle& handle) const
    {
        LASSERT(0<=handle.offset());
        return static_cast<const T*>(block_ + handle.offset());
    }

    template<class T>
    inline const T* HandleBasedAllocator::getMaybeNull(const Handle& handle) const
    {
        return (0<=handle.offset())
            ? static_cast<const T*>(block_ + handle.offset())
            : NULL;
    }

    template<class T>
    inline T* HandleBasedAllocator::get(const Handle& handle)
    {
        LASSERT(0<=handle.offset());
        return static_cast<T*>(block_ + handle.offset());
    }

    template<class T>
    inline T* HandleBasedAllocator::getMaybeNull(const Handle& handle)
    {
        return (0<=handle.offset())
            ? reinterpret_cast<T*>(block_ + handle.offset())
            : NULL;
    }

    template<class T>
    inline const T* HandleBasedAllocator::get(s32 offset) const
    {
        LASSERT(0<=offset);
        return reinterpret_cast<const T*>(block_ + offset);
    }

    template<class T>
    inline const T* HandleBasedAllocator::getMaybeNull(s32 offset) const
    {
        return (0<=offset)
            ? reinterpret_cast<const T*>(block_ + offset)
            : NULL;
    }

    template<class T>
    inline T* HandleBasedAllocator::get(s32 offset)
    {
        LASSERT(0<=offset);
        return reinterpret_cast<T*>(block_ + offset);
    }

    template<class T>
    inline T* HandleBasedAllocator::getMaybeNull(s32 offset)
    {
        return (0<=offset)
            ? reinterpret_cast<T*>(block_ + offset)
            : NULL;
    }
}
#endif //INC_LFRAMEWORK_HANDLEBAEDALLOCATOR_H__
