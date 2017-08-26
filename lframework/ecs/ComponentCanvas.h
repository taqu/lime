#ifndef INC_LFRAMEWORK_COMPONENTCANVAS_H__
#define INC_LFRAMEWORK_COMPONENTCANVAS_H__
/**
@file ComponentCanvas.h
@author t-sakai
@date 2017/05/02 create
*/
#include "ComponentRenderer.h"

#include <lcore/Array.h>
#include <lmath/lmath.h>
#include <lgraphics/VertexBufferRef.h>
#include <lgraphics/IndexBufferRef.h>
#include <lgraphics/InputLayoutRef.h>
#include <lgraphics/ConstantBufferRef.h>
#include <lgraphics/ShaderRef.h>
#include <lgraphics/BlendStateRef.h>
#include <lgraphics/SamplerStateRef.h>

namespace lgfx
{
    class ConstantBufferTableSet;
}

namespace lfw
{
    class ComponentCanvasElement;

    class ComponentCanvas : public ComponentRenderer
    {
    public:
        static u8 category(){ return ECSCategory_Renderer;}
        static u32 type(){ return ECSType_Canvas;}

        static const u32 Flag_UpdateCommands = (0x01U<<3);

        enum Shader
        {
            Shader_PS =0,
            Shader_Num,
        };

        struct Screen
        {
            f32 invHalfWidth_;
            f32 invHalfHeight_;
            f32 top_;
            f32 left_;
        };

        struct Vertex
        {
            f32 position_[2];
            u32 abgr_;
            u16 texcoord_[2];
        };

        ComponentCanvas();
        virtual ~ComponentCanvas();

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void postUpdate();
        virtual void onDestroy();
        virtual bool addQueue(RenderQueue& queue);
        virtual void drawDepth(RenderContext& renderContext);
        virtual void drawOpaque(RenderContext& renderContext);
        virtual void drawTransparent(RenderContext& renderContext);

        void setScreen(s32 left, s32 top, s32 width, s32 height);
        void setScale(f32 horizontal, f32 vertical);

        inline s32 getLeft() const;
        inline s32 getTop() const;
        inline s32 getWidth() const;
        inline s32 getHeight() const;

        void add(ComponentCanvasElement* element);
        void remove(ComponentCanvasElement* element);

        void requestUpdate();

        /**
        @param rect ... left, top, right, bottom
        @param color ... 
        @param texcoord ... left, top, right, bottom
        */
        inline void addRect(const lmath::Vector4& rect, u32 abgr, const lmath::Vector4& texcoord);
        inline void addRect(const lmath::Vector4& rect, u32 abgr, const u16 uv[4]);
        inline void addTriangle(s32 numTriangles, const Vertex* vertices);

        inline void addRect(const lmath::Vector4& rect, u32 abgr, const lmath::Vector4& texcoord, ID3D11ShaderResourceView* srv);
        inline void addRect(const lmath::Vector4& rect, u32 abgr, const u16 uv[4], ID3D11ShaderResourceView* srv);
        inline void addTriangle(s32 numTriangles, const Vertex* vertices, ID3D11ShaderResourceView* srv);

        void addRect(const lmath::Vector4& rect, u32 abgr, const lmath::Vector4& texcoord, ID3D11ShaderResourceView* srv, ID3D11PixelShader* ps);
        void addRect(const lmath::Vector4& rect, u32 abgr, const u16 uv[4], ID3D11ShaderResourceView* srv, ID3D11PixelShader* ps);
        void addTriangle(s32 numTriangles, const Vertex* vertices, ID3D11ShaderResourceView* srv, ID3D11PixelShader* ps);

    protected:
        ComponentCanvas(const ComponentCanvas&) = delete;
        ComponentCanvas& operator=(const ComponentCanvas&) = delete;

        struct Command
        {
            u16 vertexStart_;
            u16 numVertices_;
            u16 indexStart_;
            u16 numIndices_;
            ID3D11ShaderResourceView* srv_; 
            ID3D11PixelShader* pixelShader_;
        };

        typedef ArrayPODAligned<Vertex, 128> VertexArray;
        typedef ArrayPODAligned<u16, 128> IndexArray;
        typedef lcore::ArrayPOD<Command> CommandArray;
        typedef lcore::ArrayPOD<ComponentCanvasElement*, lcore::array_static_inc_size<32>> CanvasElementArray;

        void pushCommand();
        void rebuild();

        lgfx::ConstantBufferRef* createConstantBuffer(u32 size, const void* data);
        bool setConstantBuffer(Shader shader, s32 index, lgfx::ConstantBufferRef* constant);
        bool setConstant(Shader shader, s32 index, u32 size, const void* data);
        bool setConstantAligned16(Shader shader, s32 index, u32 size, const void* data);

        lgfx::ConstantBufferTableSet* constantBufferTableSet_;

        lgfx::VertexBufferRef vbs_[2];
        lgfx::IndexBufferRef ibs_[2];

        lgfx::ConstantBufferRef screen_;
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
        s32 width_;
        s32 height_;
        u16 prevNumVertices_;
        u16 prevNumIndices_;
        s32 bufferIndex_;
        f32 horizontalScale_;
        f32 verticalScale_;

        VertexArray vertices_;
        IndexArray indices_;
        CanvasElementArray canvasElements_;
        CommandArray commands_;
    };

    inline s32 ComponentCanvas::getLeft() const
    {
        return left_;
    }

    inline s32 ComponentCanvas::getTop() const
    {
        return top_;
    }

    inline s32 ComponentCanvas::getWidth() const
    {
        return width_;
    }

    inline s32 ComponentCanvas::getHeight() const
    {
        return height_;
    }

    inline void ComponentCanvas::addRect(const lmath::Vector4& rect, u32 abgr, const lmath::Vector4& texcoord)
    {
        addRect(rect, abgr, texcoord, defaultSRV_, defaultPixelShader_);
    }

    inline void ComponentCanvas::addRect(const lmath::Vector4& rect, u32 abgr, const u16 uv[4])
    {
        addRect(rect, abgr, uv, defaultSRV_, defaultPixelShader_);
    }

    inline void ComponentCanvas::addTriangle(s32 numTriangles, const Vertex* vertices)
    {
        addTriangle(numTriangles, vertices, defaultSRV_, defaultPixelShader_);
    }

    inline void ComponentCanvas::addRect(const lmath::Vector4& rect, u32 abgr, const lmath::Vector4& texcoord, ID3D11ShaderResourceView* srv)
    {
        addRect(rect, abgr, texcoord, srv, defaultPixelShader_);
    }

    inline void ComponentCanvas::addRect(const lmath::Vector4& rect, u32 abgr, const u16 uv[4], ID3D11ShaderResourceView* srv)
    {
        addRect(rect, abgr, uv, srv, defaultPixelShader_);
    }

    inline void ComponentCanvas::addTriangle(s32 numTriangles, const Vertex* vertices, ID3D11ShaderResourceView* srv)
    {
        addTriangle(numTriangles, vertices, srv, defaultPixelShader_);
    }
}
#endif //INC_LFRAMEWORK_COMPONENTCANVAS_H__
