#ifndef INC_LFRAMEWORK_RENDERER__
#define INC_LFRAMEWORK_RENDERER__
/**
@file Renderer.h
@author t-sakai
@date 2016/11/10 create
*/
#include "../lframework.h"
#include <lcore/Array.h>
#include <lgraphics/FrameSyncQuery.h>
#include <lgraphics/VertexBufferRef.h>
#include <lgraphics/IndexBufferRef.h>
#include <lgraphics/ConstantBufferTableSet.h>
#include <lgraphics/TransientBuffer.h>
#include <lgraphics/DynamicBuffer.h>
#include <lgraphics/BufferCreator.h>
#include "RenderQueue.h"
#include "RenderContext.h"
#include "DepthStencil.h"
#include "ShadowMap.h"

namespace lfw
{
    struct RendererInitParam;

    typedef lgfx::TransientBuffer<lgfx::VertexBufferRef, lgfx::VertexBufferCreator> TransientVertexBuffer;
    typedef TransientVertexBuffer::allocated_chunk_type TransientVertexBufferRef;

    typedef lgfx::TransientBuffer<lgfx::IndexBufferRef, lgfx::IndexBufferCreator> TransientIndexBuffer;
    typedef TransientIndexBuffer::allocated_chunk_type TransientIndexBufferRef;

    typedef lgfx::DynamicBuffer<lgfx::VertexBufferRef, lgfx::VertexBufferCreator> DynamicVertexBuffer;
    typedef lgfx::DynamicBuffer<lgfx::IndexBufferRef, lgfx::IndexBufferCreator> DynamicIndexBuffer;

    class ComponentRenderer;

    class Renderer
    {
    public:
        struct Entry
        {
            f32 depth_;
            ComponentRenderer* component_;
        };

        Renderer();
        ~Renderer();

        void initialize(const RendererInitParam& initParam);
        void terminate();

        inline lgfx::FrameSyncQuery& getFrameSync();
        inline lgfx::ConstantBufferTableSet& getConstantBuffer();

        inline TransientVertexBuffer& getVertexBuffer();
        inline TransientIndexBuffer& getIndexBuffer();

        void update();
    private:
        Renderer(const Renderer&);
        Renderer& operator=(const Renderer&);

        typedef lcore::ArrayPOD<RenderQueue> RenderQueueArray;
        void traverseComponents();
        void draw();

        lgfx::FrameSyncQuery frameSync_;
        lgfx::ConstantBufferTableSet constantBufferTableSet_;
        TransientVertexBuffer transientVertexBuffer_;
        TransientIndexBuffer transientIndexBuffer_;
        RenderContext renderContext_;

        RenderQueueArray renderQueuePerCamera_;
        ShadowMap shadowMap_;
        DepthStencil shadowMapDepthStencil_;
    };

    inline lgfx::FrameSyncQuery& Renderer::getFrameSync()
    {
        return frameSync_;
    }

    inline lgfx::ConstantBufferTableSet& Renderer::getConstantBuffer()
    {
        return constantBufferTableSet_;
    }

    inline TransientVertexBuffer& Renderer::getVertexBuffer()
    {
        return transientVertexBuffer_;
    }

    inline TransientIndexBuffer& Renderer::getIndexBuffer()
    {
        return transientIndexBuffer_;
    }
}
#endif //INC_LFRAMEWORK_RENDERER__
