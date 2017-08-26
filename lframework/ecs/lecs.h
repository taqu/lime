#ifndef INC_LFRAMEWORK_LECS_H__
#define INC_LFRAMEWORK_LECS_H__
/**
@file lecs.h
@author t-sakai
@date 2016/05/15 create
*/
#include "../lframework.h"
#include <lcore/LString.h>
#include <lcore/HandleTable.h>

namespace lfw
{
    typedef lcore::HandleTable<u32,16> HandleTable;
    typedef HandleTable::handle_type Handle;

    static const u32 NameBufferSize = 16;
    typedef lcore::StaticString<NameBufferSize> NameString;

    static const s32 DefaultComponentManagerCapacity = 32;
    static const s32 DefaultComponentManagerExpandSize = 32;

    static const u32 Flag_CreateRequiredComponentManagers = 0x01U<<0;

    //----------------------------------------------------------
    //---
    //--- ID
    //---
    //----------------------------------------------------------
    struct ID
    {
        static const u32 InvalidValue = (u32)-1;
        static const u32 CategoryBits = 8;
        static const u32 CategoryMask = 0xFFU;
        static const u32 CategoryShift = 24;
        static const u32 IndexBits = 16;
        static const u32 IndexMask = 0xFFFFU;

        static ID construct()
        {
            return {InvalidValue};
        }

        static ID construct(u32 value)
        {
            return {value};
        }

        static ID construct(u8 category, u16 index)
        {
            return {(category<<CategoryShift) | (index&IndexMask)};
        }

        //ID()
        //    :value_(InvalidValue)
        //{}

        //explicit ID(u32 value)
        //    :value_(value)
        //{}

        //ID(u8 category, u16 index)
        //    :value_((category<<CategoryShift) | (index&IndexMask))
        //{}

        inline static ID InvalidID()
        {
            return ID();
        }

        ID& operator=(u32 x)
        {
            value_ = x;
            return *this;
        }

        u8 category() const
        {
            return static_cast<u8>((value_>>CategoryShift)&CategoryMask);
        }

        u16 index() const
        {
            return static_cast<u16>(value_&IndexMask);
        }

        bool operator==(ID rhs) const
        {
            return value_ == rhs.value_;
        }

        bool operator!=(ID rhs) const
        {
            return value_ != rhs.value_;
        }

        bool operator==(u32 rhs) const
        {
            return value_ == rhs;
        }

        bool operator!=(u32 rhs) const
        {
            return value_ != rhs;
        }

        bool valid() const
        {
            return InvalidValue != value_;
        }

        void clear()
        {
            value_ = InvalidValue;
        }

        u32 value_;
    };

    //----------------------------------------------------------
    //---
    //--- IDTable
    //---
    //----------------------------------------------------------
    class IDTable
    {
    public:
        static const s32 MaxCapacity = ID::IndexMask;
        typedef lcore::InsecureHandleTable<u16> table_type;
        typedef table_type::handle_type handle_type;

        IDTable()
        {}

        IDTable(s32 capacity, s32 expandSize)
            :handles_(capacity, expandSize)
        {}

        ~IDTable()
        {}

        s32 capacity() const{ return handles_.capacity();}
        s32 size() const{ return handles_.size();}

        ID create(u8 category)
        {
            LASSERT(handles_.size()<MaxCapacity);
            handle_type handle = handles_.create();
            return ID::construct(category, handle);
        }

        void destroy(ID id)
        {
            handles_.destroy(id.index());
        }

        void clear()
        {
            handles_.clear();
        }

        void swap(IDTable& rhs)
        {
            handles_.swap(rhs.handles_);
        }
    private:
        IDTable(const IDTable&);
        IDTable& operator=(const IDTable&);

        table_type handles_;
    };

    //----------------------------------------------------------
    //---
    //--- IndexArray
    //---
    //----------------------------------------------------------
    /**
    キャパシティ管理は外部に委ねる. リサイズ時にデータコピーしない.
    */
    class IndexArray
    {
    public:
        IndexArray()
            :size_(0)
            ,indices_(NULL)
        {}

        ~IndexArray()
        {
            LDELETE_ARRAY(indices_);
        }

        s32 size() const
        {
            return size_;
        }

        void clear()
        {
            size_ = 0;
        }

        void resize(s32 size)
        {
            LDELETE_ARRAY(indices_);
            indices_ = LNEW u16[size];
        }

        void add(u16 value)
        {
            indices_[size_] = value;
            ++size_;
        }

        u16 operator[](s32 index) const
        {
            return indices_[index];
        }

        u16& operator[](s32 index)
        {
            return indices_[index];
        }
    private:
        IndexArray(const IndexArray&);
        IndexArray& operator=(const IndexArray&);

        s32 size_;
        u16* indices_;
    };

    //----------------------------------------------------------
    //---
    //--- Command
    //---
    //----------------------------------------------------------
    struct Operation
    {
        enum Type
        {
            Type_Create =0,
            Type_Destroy,
            Type_Max,
        };

        bool operator==(const Operation& rhs) const
        {
            return id_ == rhs.id_;
        }

        s32 type() const{return type_;}
        const ID& id() const{return id_;}

        s32 type_;
        ID id_;
    };

    struct ComponentOperation
    {
        enum Type
        {
            Type_Add =0,
            Type_Destroy,
            Type_Max,
        };

        bool operator==(const ComponentOperation& rhs) const
        {
            return component_ == rhs.component_;
        }

        s32 type_;
        Handle entityHandle_;
        ID component_;
    };

    //----------------------------------------------------------
    //---
    //--- TypeTraits
    //---
    //----------------------------------------------------------
    template<class T>
    struct TypeTraits
    {
        typedef T& reference_type;
        typedef const T& const_reference_type;
    };

    template<class T>
    struct TypeTraits<const T>
    {
        typedef const T& reference_type;
        typedef const T& const_reference_type;
    };

    //----------------------------------------------------------
    //---
    //--- RandomAccess
    //---
    //----------------------------------------------------------
    template<class T>
    struct RandomAccess
    {
    public:
        RandomAccess(s32 size, T* values)
            :size_(size)
            ,values_(values)
        {}

        s32 size() const{ return size_;}
        typename TypeTraits<T>::const_reference_type operator[](s32 index) const{ return values_[index];}
        typename TypeTraits<T>::reference_type operator[](s32 index) { return values_[index];}
    private:
        s32 size_;
        T* values_;
    };

    //----------------------------------------------------------
    //---
    //--- FilterAccess
    //---
    //----------------------------------------------------------
    template<class T, class U>
    struct FilterAccess
    {
    public:
        FilterAccess(s32 size, T* values, U func);
        inline bool end() const;
        FilterAccess& operator++();

        inline typename TypeTraits<T>::const_reference_type get() const;
        inline typename TypeTraits<T>::reference_type get();
    private:
        s32 size_;
        T* values_;
        U func_;
        s32 current_;
    };

    template<class T, class U>
    FilterAccess<T,U>::FilterAccess(s32 size, T* values, U func)
        :size_(size)
        ,values_(values)
        ,func_(func)
        ,current_(size)
    {
        LASSERT(0<=size_);
        for(s32 i=0; i<size_; ++i){
            if(func(values[i])){
                current_ = i;
                break;
            }
        }
    }

    template<class T, class U>
    inline bool FilterAccess<T,U>::end() const
    {
        return size_<=current_;
    }

    template<class T, class U>
    FilterAccess<T,U>& FilterAccess<T,U>::operator++()
    {
        for(s32 i=current_+1; i<size_; ++i){
            if(func(values[i])){
                current_ = i;
                return *this;
            }
        }
        current_ = size_;
        return *this;
    }

    template<class T, class U>
    inline typename TypeTraits<T>::const_reference_type FilterAccess<T,U>::get() const
    {
        LASSERT(!end());
        return values_[current_];
    }

    template<class T, class U>
    inline typename TypeTraits<T>::reference_type FilterAccess<T,U>::get()
    {
        return values_[current_];
    }


    //----------------------------------------------------------
    //---
    //----------------------------------------------------------
    template<class T>
    inline void expand(T*& ptr, s32 size, s32 newSize)
    {
        T* newPtr = LNEW T[newSize];
        lcore::memcpy(newPtr, ptr, sizeof(T)*size);
        LDELETE_ARRAY(ptr);
        ptr = newPtr;
    }

    template<class T>
    inline s32 indexOf(s32 size, const T* begin, const T* ptr)
    {
        lcore::ptrdiff_t diff = ptr-begin;
        s32 index = static_cast<s32>(diff/sizeof(T));
        LASSERT(0<=index && index<size);
        return index;
    }

    class ComponentSubType;

    typedef RandomAccess<const ID> IDConstAccess;

    enum ECSCategory
    {
        ECSCategory_None =0,
        ECSCategory_Logical,
        ECSCategory_Geometric,
        ECSCategory_Renderer,
        ECSCategory_SceneElement,
        ECSCategory_Behavior,
        ECSCategory_User,
        ECSCategory_Max = ECSCategory_User,
    };

    enum ECSType
    {
        ECSType_None =0,
        ECSType_Logical,
        ECSType_Geometric,
        ECSType_Renderer,
        ECSType_Camera,
        ECSType_Light,
        ECSType_Behavior,
        ECSType_Canvas,
        ECSType_CanvasElement,
        ECSType_User,
        ECSType_Max = ECSType_User,
    };

    enum BehaviorFlag
    {
        EntityFlag_UpdateSelf = 0x01U<<0,
        EntityFlag_RenderSelf = 0x01U<<1,
        EntityFlag_Update = 0x01U<<2,
        EntityFlag_Render = 0x01U<<3,

        EntityFlag_Default = EntityFlag_UpdateSelf | EntityFlag_RenderSelf | EntityFlag_Update | EntityFlag_Render,
    };
}
#endif //INC_LFRAMEWORK_LECS_H__
