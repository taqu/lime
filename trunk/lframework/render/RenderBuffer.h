#ifndef INC_LRENDER_RENDERBUFFER_H__
#define INC_LRENDER_RENDERBUFFER_H__
/**
@file RenderBuffer.h
@author t-sakai
@date 2011/03/07 create

*/
#include "lrender.h"
#include <lmath/Vector2.h>
#include <lgraphics/api/SurfaceRef.h>
#include <lgraphics/api/VertexDeclarationRef.h>

struct IDirect3DSurface9;

namespace lscene
{
    class ShaderVSBase;
    class ShaderPSBase;
}

namespace lrender
{
    struct RenderBufferBatchState
    {
        void create();
        inline void attach();

        lgraphics::VertexDeclarationRef decl_;
    };

    class RenderBuffer
    {
    public:
        class ShaderVS;
        class ShaderPS;

        RenderBuffer();
        RenderBuffer(IDirect3DSurface9* surface, RenderBufferBatchState* batchState);

        ~RenderBuffer();

        void draw();

        bool setPixelShader(u32 sourceSize, const Char* source);

        void swap(RenderBuffer& rhs);
    private:
        static const u32 VSSourceSize;
        static const u32 PSSourceSize;

        static const Char VSSource[];
        static const Char PSSource[];

        void createGeometry();

        struct Vertex
        {
            lmath::Vector2 position_;
            lmath::Vector2 uv_;
        };

        RenderBufferBatchState* batchState_;
        lgraphics::SurfaceRef surface_;
        Vertex vertices_[4];
    };
}

#endif //INC_LRENDER_RENDERBUFFER_H__
