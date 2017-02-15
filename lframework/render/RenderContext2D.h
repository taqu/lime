#ifndef INC_LFRAMEWORK_RENDERCONTEXT2D_H__
#define INC_LFRAMEWORK_RENDERCONTEXT2D_H__
/**
@file RenderContext.h
@author t-sakai
@date 2016/12/21 create
*/
#include "../lframework.h"
#include <lcore/Array.h>
#include <lmath/lmath.h>
#include <lgraphics/VertexBufferRef.h>
#include <lgraphics/IndexBufferRef.h>
#include <lgraphics/InputLayoutRef.h>
#include <lgraphics/ConstantBufferRef.h>
#include <lgraphics/ShaderRef.h>
#include <lgraphics/BlendStateRef.h>
#include <lgraphics/SamplerStateRef.h>
#include <lgraphics/DynamicBuffer.h>
#include <lgraphics/BufferCreator.h>

namespace lmath
{
    class Vector4;
}

namespace lgfx
{
    class ContextRef;
    class FrameSyncQuery;
    class ConstantBufferTableSet;
}

namespace lfw
{
    class ComponentRenderer2D;

    class RenderContext2D
    {
    public:
        typedef lgfx::DynamicBuffer<lgfx::VertexBufferRef, lgfx::VertexBufferCreator> DynamicVertexBuffer;
        typedef lgfx::DynamicBuffer<lgfx::IndexBufferRef, lgfx::IndexBufferCreator> DynamicIndexBuffer;

        struct Constant2D
        {
            f32 invScreenWidth2_;
            f32 invScreenHeight2_;
            f32 top_;
            f32 left_;
        };

        struct Vertex
        {
            f32 position_[2];
            u32 abgr_;
            u16 texcoord_[2];
        };

        enum Shader
        {
            Shader_PS =0,
            Shader_Num,
        };

        RenderContext2D();
        ~RenderContext2D();

        void initialize(
            lgfx::ContextRef* context,
            lgfx::ConstantBufferTableSet* constantBufferTableSet,
            lgfx::FrameSyncQuery* frameSyncQuery,
            s32 left,
            s32 top,
            s32 screenWidth,
            s32 screenHeight,
            u16 maxVerticesPerFrame,
            u16 maxIndicesPerFrame);

        void terminate();

        void resize(s32 left, s32 top, s32 screenWidth, s32 screenHeight);

        void begin();
        void end();

        inline s32 getLeft() const;
        inline s32 getTop() const;
        inline s32 getScreenWidth() const;
        inline s32 getScreenHeight() const;

        inline lgfx::ContextRef& getContext();
        inline void setContext(lgfx::ContextRef* context);

        inline lgfx::ConstantBufferTableSet& getConsantBuffer();

        lgfx::ConstantBufferRef* createConstantBuffer(u32 size);
        lgfx::ConstantBufferRef* createConstantBuffer(u32 size, const void* data);

        bool setConstant(Shader shader, s32 index, u32 size, const void* data);
        bool setConstantBuffer(Shader shader, s32 index, lgfx::ConstantBufferRef* constant);
        bool setConstantAligned16(Shader shader, s32 index, u32 size, const void* data);

        inline ID3D11PixelShader* getDefaultPixelShader();
        inline ID3D11ShaderResourceView* getDefaultShaderResource();

        /**
        @param rect ... left, top, right, bottom
        @param color ... 
        @param texcoord ... left, top, right, bottom
        */
        inline void addRect(const lmath::Vector4& rect, u32 abgr, const lmath::Vector4& texcoord);
        inline void addRect(const lmath::Vector4& rect, u32 abgr, const u16 uv[4]);
        inline void addTriangle(s32 numTriangles, const Vertex* vertices);

        void addRect(const lmath::Vector4& rect, u32 abgr, const lmath::Vector4& texcoord, ID3D11ShaderResourceView* srv, ID3D11PixelShader* ps);
        void addRect(const lmath::Vector4& rect, u32 abgr, const u16 uv[4], ID3D11ShaderResourceView* srv, ID3D11PixelShader* ps);
        void addTriangle(s32 numTriangles, const Vertex* vertices, ID3D11ShaderResourceView* srv, ID3D11PixelShader* ps);
    private:
        RenderContext2D(const RenderContext2D&);
        RenderContext2D& operator=(const RenderContext2D&);

        struct Command
        {
            u16 vertexStart_;
            u16 numVertices_;
            u16 indexStart_;
            u16 numIndices_;
            ID3D11ShaderResourceView* srv_; 
            ID3D11PixelShader* pixelShader_;
        };
        typedef lcore::ArrayPOD<Command> CommandArray;

        void pushCommand();

        lgfx::ContextRef* context_;
        lgfx::ConstantBufferTableSet* constantBufferTableSet_;
        DynamicVertexBuffer dynamicVertexBuffer_;
        DynamicIndexBuffer dynamicIndexBuffer_;

        lgfx::ConstantBufferRef constant2D_;
        lgfx::InputLayoutRef inputLayout_;
        lgfx::BlendStateRef blendState_;
        lgfx::SamplerStateRef samLinear_;
        lgfx::VertexShaderRef vertexShader_;

        lgfx::PixelShaderRef defaultPixelShader_;
        lgfx::ShaderResourceViewRef defaultSRV_;

        ID3D11ShaderResourceView* srv_;
        ID3D11PixelShader* pixelShader_;

        s32 left_;
        s32 top_;
        s32 screenWidth_;
        s32 screenHeight_;
        u16 maxVerticesPerFrame_;
        u16 maxIndicesPerFrame_;
        u16 prevNumVertices_;
        u16 numVertices_;
        u16 prevNumIndices_;
        u16 numIndices_;
        Vertex* vertices_;
        u16* indices_;

        CommandArray commands_;
    };

    inline s32 RenderContext2D::getLeft() const
    {
        return left_;
    }

    inline s32 RenderContext2D::getTop() const
    {
        return top_;
    }

    inline s32 RenderContext2D::getScreenWidth() const
    {
        return screenWidth_;
    }

    inline s32 RenderContext2D::getScreenHeight() const
    {
        return screenHeight_;
    }

    inline lgfx::ContextRef& RenderContext2D::getContext()
    {
        return *context_;
    }

    inline void RenderContext2D::setContext(lgfx::ContextRef* context)
    {
        context_ = context;
    }

    inline lgfx::ConstantBufferTableSet& RenderContext2D::getConsantBuffer()
    {
        return *constantBufferTableSet_;
    }

    inline ID3D11PixelShader* RenderContext2D::getDefaultPixelShader()
    {
        return defaultPixelShader_;
    }

    inline ID3D11ShaderResourceView* RenderContext2D::getDefaultShaderResource()
    {
        return defaultSRV_;
    }

    inline void RenderContext2D::addRect(const lmath::Vector4& rect, u32 abgr, const lmath::Vector4& texcoord)
    {
        addRect(rect, abgr, texcoord, defaultSRV_, defaultPixelShader_);
    }

    inline void RenderContext2D::addRect(const lmath::Vector4& rect, u32 abgr, const u16 uv[4])
    {
        addRect(rect, abgr, uv, defaultSRV_, defaultPixelShader_);
    }

    inline void RenderContext2D::addTriangle(s32 numTriangles, const Vertex* vertices)
    {
        addTriangle(numTriangles, vertices, defaultSRV_, defaultPixelShader_);
    }
}
#endif //INC_LFRAMEWORK_RENDERCONTEXT2D_H__
