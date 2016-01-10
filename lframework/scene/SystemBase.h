#ifndef INC_LSCENE_SYSTEMBASE_H__
#define INC_LSCENE_SYSTEMBASE_H__
/**
@file SystemBase.h
@author t-sakai
@date 2014/10/08 create
*/
#include "lscene.h"

#include <lcore/allocator/ObjectPool.h>
#include <lgraphics/api/QueryRef.h>

#include "Scene.h"
#include "ConstantBuffer.h"
#include "FrameSyncQuery.h"
#include "TransientBuffer.h"
#include "DynamicBuffer.h"
#include "RenderQueue.h"
#include "render/ShaderID.h"
#include "render/AnimObject.h"
#include "render/Object.h"

namespace lscene
{
    class SystemBase;
    class ObjectFactoryBase;
    class Resource;
    class Resources;

namespace lfile
{
    class FileSystemBase;
}

    //--------------------------------------------------
    //---
    //--- InstanceHolder
    //---
    //--------------------------------------------------
    template<class T>
    class InstanceHolder
    {
    public:
        typedef T target_type;
        typedef InstanceHolder<T> this_type;

        static target_type& getInstance(){ return *instance_;}

        template<class U>
        static U& getInstance(){ return dynamic_cast<U&>(*instance_);}

        static bool valid(){ return NULL != instance_;}

    private:
        friend T;

        InstanceHolder(const this_type&);
        this_type& operator=(const this_type&);

        static void set(target_type* ptr)
        {
            if(NULL != instance_){
                LIME_DELETE(instance_);
            }
            instance_ = ptr;
        }
        static target_type* instance_;
    };

    template<class T>
    typename InstanceHolder<T>::target_type*
        InstanceHolder<T>::instance_ = NULL;

    //--------------------------------------------------
    //---
    //--------------------------------------------------
    typedef InstanceHolder<SystemBase> SystemInstance;
    typedef FrameSyncQuery<NumSyncFrames> FrameSyncQueryType;
    typedef TransientBuffer<lgraphics::VertexBufferRef, VertexBufferCreator> TransientVertexBuffer;
    typedef TransientVertexBuffer::allocated_chunk_type TransientVertexBufferRef;

    typedef TransientBuffer<lgraphics::IndexBufferRef, IndexBufferCreator> TransientIndexBuffer;
    typedef TransientIndexBuffer::allocated_chunk_type TransientIndexBufferRef;

    typedef lscene::DynamicBuffer<lgraphics::VertexBufferRef, lscene::VertexBufferCreator> DynamicVertexBuffer;
    typedef lscene::DynamicBuffer<lgraphics::IndexBufferRef, lscene::IndexBufferCreator> DynamicIndexBuffer;

    enum Locale
    {
        Locale_SystemDefault =0,
    };

    //--------------------------------------------------
    //---
    //--- SystemBase
    //---
    //--------------------------------------------------
    class SystemBase
    {
    public:
        struct InitParam
        {
            InitParam()
                :allocFunc_(NULL)
                ,allocFuncDebug_(NULL)
                ,freeFunc_(NULL)
                ,alignedAllocFunc_(NULL)
                ,alignedAllocFuncDebug_(NULL)
                ,alignedFreeFunc_(NULL)
                ,eventQueryPoolPageSize_(1024)
                ,numConstantTables_(3)
                ,locale_(Locale_SystemDefault)
                ,numVS_(lrender::ShaderVS_Num)
                ,numGS_(lrender::ShaderGS_Num)
                ,numPS_(lrender::ShaderPS_Num)
                ,numCS_(lrender::ShaderCS_Num)
                ,numDS_(lrender::ShaderDS_Num)
                ,numHS_(lrender::ShaderHS_Num)
                ,objectFactory_(NULL)
            {}

            lcore::AllocFunc allocFunc_;
            lcore::AllocFuncDebug allocFuncDebug_;
            lcore::FreeFunc freeFunc_;
            lcore::AlignedAllocFunc alignedAllocFunc_;
            lcore::AlignedAllocFuncDebug alignedAllocFuncDebug_;
            lcore::AlignedFreeFunc alignedFreeFunc_;
            u32 eventQueryPoolPageSize_;
            s32 numConstantTables_;
            s32 locale_;
            s32 numVS_;
            s32 numGS_;
            s32 numPS_;
            s32 numCS_;
            s32 numDS_;
            s32 numHS_;
            ObjectFactoryBase* objectFactory_;
        };

        virtual ~SystemBase();

        bool initialize(const InitParam& param);
        void terminate();

        inline SceneParam& getDefaultParameter();

        inline void begin(lgraphics::ContextRef& context);
        inline void end(lgraphics::ContextRef& context);

        bool openFileSystem(const Char* path, bool isVirtual);
        inline lfile::FileSystemBase* getFileSystem();
        inline ObjectFactoryBase* getObjectFactory();

        inline Scene& getScene();
        inline FrameSyncQueryType& getFrameSync();
        inline ConstantBuffer& getConstantBuffer();
        inline TransientVertexBuffer& getVertexBuffer();
        inline TransientIndexBuffer& getIndexBuffer();

        s8 indexToPass(s8 index) const;

        /**
        @brief SJIS -> UTF16ïœä∑
        @return ê¨å˜Ç»ÇÁïœä∑ÇµÇΩï∂éöêîÅAé∏îsÇ»ÇÁ-1
        */
        s32 MBSToWCSSharedBuffer(const WChar*& dst, const Char* src);

        void clearResource(s32 category);
        void clearResources();

        Resource* getResource(s32 category, s32 type, const Char* path);
        template<class T>
        inline T* getResourceTyped(s32 category, const Char* path);

        Resource* getResource(s32 category, s32 type, const Char* path, const SceneParam& param);
        template<class T>
        inline T* getResourceTyped(s32 category, const Char* path, const SceneParam& param);

        void removeResource(s32 category, Resource* resource);
    protected:
        SystemBase();
        SystemBase(const SystemBase&);
        SystemBase& operator=(const SystemBase&);

        void destroyAll();
        void initializeDefaultRenderPath(RenderQueue& queue);

        void initializeSceneManager();
        void terminateSceneManager();

        SceneParam defaultParameter_;
        lfile::FileSystemBase* fileSystem_;
        ObjectFactoryBase* objectFactory_;
        Resources* resources_;

        Scene scene_;
        FrameSyncQueryType frameSync_;
        ConstantBuffer constantBuffer_;

        TransientVertexBuffer transientVertexBuffer_;
        TransientIndexBuffer transientIndexBuffer_;

        s32 wcsSize_;
        WChar* wcsSharedBuffer_;
    };

    inline SceneParam& SystemBase::getDefaultParameter()
    {
        return defaultParameter_;
    }

    inline void SystemBase::begin(lgraphics::ContextRef& context)
    {
        frameSync_.begin(context);
        constantBuffer_.begin();
        transientVertexBuffer_.begin();
        transientIndexBuffer_.begin();
    }

    inline void SystemBase::end(lgraphics::ContextRef& context)
    {
        frameSync_.end(context);
    }

    inline lfile::FileSystemBase* SystemBase::getFileSystem()
    {
        return fileSystem_;
    }

    inline ObjectFactoryBase* SystemBase::getObjectFactory()
    {
        return objectFactory_;
    }

    inline Scene& SystemBase::getScene()
    {
        return scene_;
    }

    inline FrameSyncQueryType& SystemBase::getFrameSync()
    {
        return frameSync_;
    }

    inline ConstantBuffer& SystemBase::getConstantBuffer()
    {
        return constantBuffer_;
    }

    inline TransientVertexBuffer& SystemBase::getVertexBuffer()
    {
        return transientVertexBuffer_;
    }

    inline TransientIndexBuffer& SystemBase::getIndexBuffer()
    {
        return transientIndexBuffer_;
    }

    template<class T>
    inline T* SystemBase::getResourceTyped(s32 category, const Char* path)
    {
        return getResourceTyped<T>(category, path, defaultParameter_);
    }

    template<class T>
    inline T* SystemBase::getResourceTyped(s32 category, const Char* path, const SceneParam& param)
    {
        Resource* resource = getResource(category, T::type_id, path, param);
        LASSERT(resource->getType() == T::type_id);
        return reinterpret_cast<T*>(resource);
    }
}
#endif //INC_LSCENE_SYSTEMBASE_H__
