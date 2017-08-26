#ifndef INC_LFRAMEWORK_H__
#define INC_LFRAMEWORK_H__
/**
@file lframework.h
@author t-sakai
@date 2016/05/08 create
*/
#include <lcore/lcore.h>
#include "Config.h"

namespace lmath
{
    class Vector4;
    class Matrix44;
}

namespace lfw
{
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;
    using lcore::s64;
    using lcore::u8;
    using lcore::u16;
    using lcore::u32;
    using lcore::u64;
    using lcore::f32;
    using lcore::f64;
    using lcore::Char;
    using lcore::lsize_t;

    static const s32 ComputeShader_NumThreads_Shift = LFW_CONFIG_COMPUTESHADER_NUMTHREADS_BITSHIFT;
    static const s32 ComputeShader_NumThreads = (0x01<<ComputeShader_NumThreads_Shift);
    static const u32 ComputeShader_NumThreads_Mask = ComputeShader_NumThreads-1;
    inline s32 padSizeForComputeShader(s32 size)
    {
        return (size + ComputeShader_NumThreads_Mask)&~ComputeShader_NumThreads_Mask;
    }

#ifdef LFW_CONFIG_ENABLE_GUI
#ifdef _DEBUG
#define LFW_ENABLE_GUI
#else
#ifndef LFW_CONFIG_ENABLE_ONLYDEBUG_GUI
#define LFW_ENABLE_GUI
#endif
#endif
#endif

    enum Layer
    {
        Layer_Default = 0x01U<<0,
        Layer_Default2D = 0x01U<<1,
    };

    enum ResourceType
    {
        ResourceType_Bytes,
        ResourceType_Texture2D,
        ResourceType_Texture3D,
        ResourceType_Model,
        ResourceType_Animation,
    };

    enum LightType
    {
        LightType_Direction =0,
        LightType_Point,
        LightType_Spot,
        LightType_Area,
    };

    enum RenderPath
    {
        RenderPath_Shadow =0,
        RenderPath_Opaque,
        RenderPath_Transparent,
        RenderPath_UI,
        RenderPath_Num,
    };

    enum ClearType
    {
        ClearType_None =0,
        ClearType_Depth,
        ClearType_Color,
        ClearType_SkyBox,
    };

    enum ShadowType
    {
        ShadowType_None =0,
        ShadowType_ShadowMap,
    };

    enum DeferredRT
    {
        DeferredRT_Albedo =0,
        DeferredRT_Specular,
        DeferredRT_Normal,
        DeferredRT_Velocity,
        DeferredRT_Depth,
        DeferredRT_Num,
    };

    enum CollisionType
    {
        CollisionType_Sphere = 0,
        CollisionType_Ray,
        CollisionType_AABB,
        CollisionType_Capsule,
        CollisionType_Num,
    };

    enum CollisionInfoType
    {
        CollisionInfoType_NormalDepth,
        CollisionInfoType_ClosestPoint,
    };

    static const u32 RendererFlag_ResetCamera = 0x01U<<0;

    extern const Char* ImageExtension_DDS;
    extern const Char* ImageExtension_PNG;
    extern const Char* ImageExtension_BMP;
    extern const Char* ImageExtension_TGA;

    /**
    @brief 16の倍数のサイズのデータコピー
    @param dst ... 
    @param src ... 
    @param size ... 16の倍数
    */
    void copySize16(void* dst, const void* src, s32 size);

    /**
    @brief 16の倍数のサイズのデータコピー
    @param dst ... 16バイトアライン
    @param src ... 
    @param size ... 16の倍数
    */
    void copyAlignedDst16(void* dst, const void* src, s32 size);

    /**
    @brief 16の倍数のサイズのデータコピー
    @param dst ... 
    @param src ... 16バイトアライン
    @param size ... 16の倍数
    */
    void copyAlignedSrc16(void* dst, const void* src, s32 size);

    /**
    @brief 16の倍数のサイズのデータコピー
    @param dst ... 16バイトアライン
    @param src ... 16バイトアライン
    @param size ... 16の倍数
    */
    void copyAlignedDstAlignedSrc16(void* dst, const void* src, s32 size);

    /**
    @brief
    @param dst ... 16バイトアライン
    @param src ... 
    */
    void copyAlignedDst16(lmath::Vector4& dst, const lmath::Vector4& src);

    /**
    @brief マトリックスコピー
    @param dst ... 16バイトアライン
    @param src ... 
    */
    void copyAlignedDst16(lmath::Matrix44& dst, const lmath::Matrix44& src);

    u64 calcHash(const Char* path);

    //------------------------------------------------------------
    //---
    //--- ArrayPODAligned
    //---
    //------------------------------------------------------------
    template<class T, s32 IncSize>
    class ArrayPODAligned
    {
    public:
        static const s32 INCSIZE = IncSize;
        typedef ArrayPODAligned<T, IncSize> this_type;
        typedef u16 size_type;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef T value_type;

        ArrayPODAligned();
        explicit ArrayPODAligned(size_type capacity);
        ~ArrayPODAligned();

        size_type size() const{ return size_;}
        size_type capacity() const{ return capacity_;}
        void clear(){ size_ = 0;}

        void push_back(const value_type& x);
        void pop_back();
        void removeAt(s32 index);

        const value_type& operator[](s32 index) const{ return data_[index];}
        value_type& operator[](s32 index){ return data_[index];}

        iterator begin(){ return data_;}
        const_iterator begin() const{ return (const_iterator)(data_);}

        iterator end(){ return data_ + size_;}
        const_iterator end() const{ return (const_iterator)(data_ + size_);}

        void swap(this_type& rhs);
        void reserve(size_type capacity);
        void expand(size_type size);
    private:
        ArrayPODAligned(const this_type&) = delete;
        ArrayPODAligned& operator=(const this_type&) = delete;

        void expand();

        size_type capacity_;
        size_type size_;
        value_type* data_;
    };

    template<class T, s32 IncSize>
    ArrayPODAligned<T, IncSize>::ArrayPODAligned()
        :capacity_(0)
        ,size_(0)
        ,data_(NULL)
    {
    }

    template<class T, s32 IncSize>
    ArrayPODAligned<T, IncSize>::ArrayPODAligned(size_type capacity)
        :capacity_(capacity)
        ,size_(0)
    {
        LASSERT(0<=capacity_);
        data_ = static_cast<value_type*>( LALIGNED_MALLOC(sizeof(value_type)*capacity_, lcore::SSE_ALIGN) );
    }

    template<class T, s32 IncSize>
    ArrayPODAligned<T, IncSize>::~ArrayPODAligned()
    {
        LALIGNED_FREE(data_, lcore::SSE_ALIGN);
    }

    template<class T, s32 IncSize>
    void ArrayPODAligned<T, IncSize>::expand()
    {
        s32 capacity = capacity_ + INCSIZE;
        LASSERT(capacity<=0xFFFFU);
        LASSERT(size_<capacity);

        //新しいバッファ確保
        value_type* data = static_cast<value_type*>(LALIGNED_MALLOC(sizeof(value_type)*capacity, lcore::SSE_ALIGN));
        lcore::memcpy(data, data_, sizeof(value_type)*size_);
        LALIGNED_FREE(data_, lcore::SSE_ALIGN);
        capacity_ = static_cast<size_type>(capacity);
        data_ = data;
    }

    template<class T, s32 IncSize>
    void ArrayPODAligned<T, IncSize>::push_back(const value_type& x)
    {
        if(capacity_<=size_){
            expand();
        }
        data_[size_] = x;
        ++size_;
    }

    template<class T, s32 IncSize>
    void ArrayPODAligned<T, IncSize>::pop_back()
    {
        if(size_<=0){
            return;
        }
        --size_;
    }

    template<class T, s32 IncSize>
    void ArrayPODAligned<T, IncSize>::removeAt(s32 index)
    {
        LASSERT(0<=index && index<size_);
        for(s32 i=index+1; i<size_; ++i){
            data_[i-1] = data_[i];
        }
        --size_;
    }

    template<class T, s32 IncSize>
    void ArrayPODAligned<T, IncSize>::swap(this_type& rhs)
    {
        lcore::swap(capacity_, rhs.capacity_);
        lcore::swap(size_, rhs.size_);
        lcore::swap(data_, rhs.data_);
    }

    template<class T, s32 IncSize>
    void ArrayPODAligned<T, IncSize>::reserve(size_type capacity)
    {
        if(capacity<=capacity_){
            return;
        }

        //新しいバッファ確保
        value_type* data = static_cast<value_type*>( LALIGNED_MALLOC(sizeof(value_type)*capacity, lcore::SSE_ALIGN) );
        lcore::memcpy(data, data_, sizeof(value_type)*size_);
        LALIGNED_FREE(data_, lcore::SSE_ALIGN);
        capacity_ = capacity;
        data_ = data;
    }

    template<class T, s32 IncSize>
    void ArrayPODAligned<T, IncSize>::expand(size_type size)
    {
        s32 newSize = lcore::minimum(static_cast<s32>(size_) + size, 0xFFFF);
        if(capacity_<newSize){
            s32 capacity = capacity_;
            while(capacity<newSize){
                capacity += INCSIZE;
            }
            capacity = lcore::minimum(capacity, 0xFFFF);
            reserve(static_cast<size_type>(capacity));
        }
        size_ = static_cast<size_type>(newSize);
    }
}
#endif //INC_LFRAMEWORK_H__
