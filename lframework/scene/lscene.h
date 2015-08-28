#ifndef INC_LSCENE_H__
#define INC_LSCENE_H__
/**
@file lscene.h
@author t-sakai
@date 2010/11/21 create
*/
#include <lcore/lcore.h>
#include <lcore/String.h>

namespace lscene
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
    using lcore::WChar;

    using lcore::lsize_t;

    static const u32 MemoryAlign = 16;
    static const u32 MaxNameSize = 24;

    static const s32 FrameSyncFlag_None = 0;
    static const s32 FrameSyncFlag_InGPUUse = 1;

    static const s32 NumSyncFrames = 8;
    static const s32 MaskSyncFrames = NumSyncFrames-1;

    static const s32 MaxCascades = 4;

    inline static s32 calcPrevFrame(s32 frame)
    {
        --frame;
        return (frame<0)? MaskSyncFrames : frame;
    }

    inline static s32 calcNextFrame(s32 frame)
    {
        return (frame+1)&MaskSyncFrames;
    }

    static const u32 MaxNodeNameBufferSize = 16;
    typedef lcore::String<MaxNodeNameBufferSize> NameString;

    static const u32 MaxResourceNameBufferSize = 16;

    static const u32 MaxFileNameBufferSize = 64;

#define LSCENE_ADDREF(ptr) if(ptr){(ptr)->addRef();}
#define LSCENE_RELEASE(ptr) if(ptr){(ptr)->release();(ptr)=NULL;}

    template<class T>
    void move(T*& dst, T*& src)
    {
        if(dst){
            dst->release();
        }
        dst=src;
        if(dst){
            dst->addRef();
        }
    }

    struct SceneAllocator
    {
        static inline void* malloc(u32 size)
        {
            return malloc_(size);
        }

        static inline void* malloc(u32 size, const char* file, int line)
        {
            return mallocDebug_(size, file, line);
        }

        static inline void free(void* mem)
        {
            free_(mem);
        }

        static inline void* malloc(u32 size, u32 alignment)
        {
            return alignedMalloc_(size, alignment);
        }

        static inline void* malloc(u32 size, u32 alignment, const char* file, int line)
        {
            return alignedMallocDebug_(size, alignment, file, line);
        }

        static inline void free(void* mem, u32 alignment)
        {
            alignedFree_(mem, alignment);
        }

        static lcore::AllocFunc malloc_;
        static lcore::AllocFuncDebug mallocDebug_;
        static lcore::FreeFunc free_;

        static lcore::AlignedAllocFunc alignedMalloc_;
        static lcore::AlignedAllocFuncDebug alignedMallocDebug_;
        static lcore::AlignedFreeFunc alignedFree_;
    };

#if defined(_DEBUG)
#define LSCENE_MALLOC(size) lscene::SceneAllocator::malloc(size, __FILE__, __LINE__)
#define LSCENE_FREE(mem) {lscene::SceneAllocator::free((mem));(mem)=NULL;}
#define LSCENE_ALIGNED_MALLOC(size, align) lscene::SceneAllocator::malloc(size, align, __FILE__, __LINE__)
#define LSCENE_ALIGNED_FREE(mem, align) {lscene::SceneAllocator::free((mem), (align));(mem)=NULL;}
#define LSCENE_NEW new(__FILE__, __LINE__)
#define LSCENE_PLACEMENT_NEW(ptr) new(ptr)
#define LSCENE_DELETE(ptr) {delete (ptr); (ptr) = NULL;}
#define LSCENE_DELETE_NO_NULL(ptr) delete (ptr)
#define LSCENE_DELETE_ARRAY(ptr) {delete[] (ptr); (ptr) = NULL;}
#else
#define LSCENE_MALLOC(size) lscene::SceneAllocator::malloc(size)
#define LSCENE_FREE(mem) {lscene::SceneAllocator::free((mem));(mem)=NULL;}
#define LSCENE_ALIGNED_MALLOC(size, align) lscene::SceneAllocator::malloc(size, align)
#define LSCENE_ALIGNED_FREE(mem, align) {lscene::SceneAllocator::free((mem), (align));(mem)=NULL;}
#define LSCENE_NEW new
#define LSCENE_PLACEMENT_NEW(ptr) new(ptr)
#define LSCENE_DELETE(ptr) {delete (ptr); (ptr) = NULL;}
#define LSCENE_DELETE_NO_NULL(ptr) delete (ptr)
#define LSCENE_DELETE_ARRAY(ptr) {delete[] (ptr); (ptr) = NULL;}
#endif

    struct NullObject
    {
    };

    template<class Allocator, class Base>
    class ObjectAllocator : public Base
    {
    public:
        static void* operator new(lsize_t size)
        {
            return Allocator::malloc(size);
        }

        static void operator delete(void* ptr)
        {
            Allocator::free(ptr);
        }

        static void* operator new[](lsize_t size)
        {
            return Allocator::malloc(size);
        }

        static void operator delete[](void* ptr)
        {
            Allocator::free(ptr);
        }

        static void* operator new(std::size_t size, const char* file, int line)
        {
            return Allocator::malloc(size, file, line);
        }

        static void operator delete(void* ptr, const char* /*file*/, int /*line*/)
        {
            Allocator::free(ptr);
        }

        static void* operator new[](std::size_t size, const char* file, int line)
        {
            return Allocator::malloc(size, file, line);
        }

        static void operator delete[](void* ptr, const char* /*file*/, int /*line*/)
        {
            Allocator::free(ptr);
        }
    };

    typedef ObjectAllocator<SceneAllocator, NullObject> RenderableAllocator;
    typedef ObjectAllocator<SceneAllocator, NullObject> ModuleAllocator;
    typedef ObjectAllocator<SceneAllocator, NullObject> ObjectFactoryAllocator;
    typedef ObjectAllocator<SceneAllocator, NullObject> FileSystemAllocator;
    typedef ObjectAllocator<SceneAllocator, NullObject> ResourceAllocator;
    typedef ObjectAllocator<SceneAllocator, NullObject> ResourcesAllocator;
    typedef ObjectAllocator<SceneAllocator, NullObject> ShaderManagerAllocator;
    typedef ObjectAllocator<SceneAllocator, NullObject> InputLayoutManagerAllocator;
    typedef ObjectAllocator<SceneAllocator, NullObject> CollideManagerAllocator;
    typedef ObjectAllocator<SceneAllocator, NullObject> ColliderAllocator;

    struct MappedSubresource
    {
        void* offsetData_;
        s32 offsetInBytes_;
        s32 sizeInBytes_;

        template<class T>
        T* getData(){ return reinterpret_cast<T*>(offsetData_);}
    };

    void copySize16Times(void* dst, const void* src, s32 size);
    void copyAlign16DstOnlySize16Times(void* dst, const void* src, s32 size);
    void copyAlign16Size16Times(void* dst, const void* src, s32 size);

    static const s32 MaxPaths = 32;
    enum Path
    {
        Path_Shadow =4,
        Path_Opaque = 8,
        Path_Transparent = 12,
        Path_MotionVelocity = 16,
        Path_Effect = 20,
        Path_2D = 24,
        Path_Num = 6,
    };

    enum NodeType
    {
        NodeType_Base =100,
        NodeType_Transform,
        NodeType_Object,
        NodeType_AnimObject,
        NodeType_ObjectMotion,
        NodeType_AnimObjectMotion,
        NodeType_StaticObject,
        NodeType_User =1000,
    };

    static const s32 DefaultSceneConstantVSAttachIndex = 0;
    static const s32 DefaultSceneConstantHSAttachIndex = 0;
    static const s32 DefaultSceneConstantDSAttachIndex = 0;
    static const s32 DefaultSceneConstantGSAttachIndex = 0;
    static const s32 DefaultSceneConstantPSAttachIndex = 0;

    //static const s32 DefaultMaterialConstantPSAttachIndex = 1;
    //static const s32 DefaultNodeConstantVSAttachIndex = 1;
    //static const s32 DefaultNodeConstantMatricesVSAttachIndex = 2;


    static const s32 NumResourceCategory = 2;
    static const s32 InitialResourceMapSize = 64;

    enum ResourceType
    {
        ResourceType_Unknown = 0,
        ResourceType_Object,
        ResourceType_AnimObject,
        ResourceType_AnimClip,
        ResourceType_Texture2D,
        ResourceType_Texture3D,
        ResourceType_TextureCube,
        ResourceType_User = 1000,
    };

    enum NodeFlag
    {
        NodeFlag_None = 0,
        NodeFlag_Update = (0x01U<<0),
        NodeFlag_Render = (0x01U<<1),
        NodeFlag_Transform = (0x01U<<2),

        NodeFlag_CastShadow = (0x01U<<16),
        NodeFlag_Solid = (0x01U<<17),
        NodeFlag_Transparent = (0x01U<<18),
        NodeFlag_RenderAttributes = NodeFlag_CastShadow|NodeFlag_Solid|NodeFlag_Transparent,
    };
}
#endif //INC_LSCENE_H__
