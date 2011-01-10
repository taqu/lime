#ifndef INC_LFRAMEWORK_STATE2D_H__
#define INC_LFRAMEWORK_STATE2D_H__
/**
@file State2D.h
@author t-sakai
@date 2010/03/02 create
*/
#include <lgraphics/api/RenderStateRef.h>
#include <lgraphics/api/VertexDeclarationRef.h>
#include <lgraphics/api/ShaderRef.h>

#include <lmath/Vector3.h>
#include <lmath/Vector2.h>
#include <lmath/Color.h>

namespace lframework
{
    /**
    */
    class State2D
    {
    public:
        struct Vertex
        {
            lmath::Vector3 position_;
            lmath::Vector2 uv_;
            lmath::Color color_;
        };

        State2D();
        ~State2D();

        inline void apply();
    private:
        lgraphics::RenderStateRef renderState_;
        lgraphics::VertexDeclarationRef vertexDecl_;
        lgraphics::VertexShaderRef vertexShader_;
        lgraphics::PixelShaderRef pixelShader_;
    };

    inline void State2D::apply()
    {
        renderState_.apply();
    }
}

#endif //INC_LFRAMEWORK_STATE2D_H__
