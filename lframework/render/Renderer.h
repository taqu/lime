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
#include <lgraphics/BufferCreator.h>
#include <lgraphics/ShaderRef.h>
#include "RenderQueue.h"
#include "RenderContext.h"
#include "ShadowMap.h"

namespace lfw
{
    struct RendererInitParam;

    typedef lgfx::TransientBuffer<lgfx::VertexBufferRef, lgfx::VertexBufferCreator> TransientVertexBuffer;
    typedef TransientVertexBuffer::allocated_chunk_type TransientVertexBufferRef;

    typedef lgfx::TransientBuffer<lgfx::IndexBufferRef, lgfx::IndexBufferCreator> TransientIndexBuffer;
    typedef TransientIndexBuffer::allocated_chunk_type TransientIndexBufferRef;

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

        void resetCamera();

        inline lgfx::FrameSyncQuery& getFrameSync();
        inline lgfx::ConstantBufferTableSet& getConstantBuffer();

        inline TransientVertexBuffer& getVertexBuffer();
        inline TransientIndexBuffer& getIndexBuffer();

        inline RenderContext& getRenderContext();

        void begin();
        void update();
    private:
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        typedef lcore::Array<RenderQueue> RenderQueueArray;

        void traverseComponents();
        void draw();

        lcore::BitSet32 flags_;
        lgfx::FrameSyncQuery frameSync_;
        lgfx::ConstantBufferTableSet constantBufferTableSet_;
        TransientVertexBuffer transientVertexBuffer_;
        TransientIndexBuffer transientIndexBuffer_;

        RenderContext renderContext_;
        RenderQueueArray renderQueuePerCamera_;
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

    inline RenderContext& Renderer::getRenderContext()
    {
        return renderContext_;
    }
}
#endif //INC_LFRAMEWORK_RENDERER__
