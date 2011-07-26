#ifndef INC_DYNAMICSDEBUGDRAW_H__
#define INC_DYNAMICSDEBUGDRAW_H__
/**
@file DynamicsDebugDraw.h
@author t-sakai
@date 2011/07/26 create
*/
#include "dynamics.h"
#include <lgraphics/lgraphics.h>
#include <lgraphics/api/VertexDeclarationRef.h>
#include <lframework/scene/ShaderBase.h>

#include <bullet/LinearMath/btIDebugDraw.h>

namespace dynamics
{
    class DynamicsDebugDraw : public btIDebugDraw
    {
    public:
        struct LineVertex
        {
            lmath::Vector3 position_;
            u32 color_;
        };

        //---------------------------------------------------
        //--- LineShaderVS
        //---------------------------------------------------
        class LineShaderVS : public lscene::ShaderVSBase
        {
        public:
            enum ParamVS
            {
                ParamVS_WVP =0,
                ParamVS_Num,
            };

            LineShaderVS()
            {}

            LineShaderVS(const lgraphics::VertexShaderRef& ref)
                :ShaderVSBase(ref)
            {}

            virtual ~LineShaderVS()
            {}

            virtual void initialize();

            virtual void setParameters(const lrender::Batch&, const lscene::Scene&)
            {
            }

            void setParameters();

            lgraphics::HANDLE params_[ParamVS_Num];
        };


        //---------------------------------------------------
        //--- LineShaderPS
        //---------------------------------------------------
        class LineShaderPS : public lscene::ShaderPSBase
        {
        public:
            LineShaderPS()
            {}

            LineShaderPS(const lgraphics::PixelShaderRef& ref)
                :ShaderPSBase(ref)
            {}

            ~LineShaderPS()
            {}

            virtual void initialize()
            {
            }

            virtual void setParameters(const lrender::Batch& , const lscene::Scene& )
            {
            }

            void setParameters(const lscene::Material& )
            {
            }
        };

        DynamicsDebugDraw();
        ~DynamicsDebugDraw();

        void initialize();
        void terminate();

        void flush();

        virtual void drawLine (const btVector3 &from, const btVector3 &to, const btVector3 &color);
        virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);

        virtual void draw3dText(const btVector3& location,const char* textString);

        virtual void setDebugMode(int debugMode);

        virtual int getDebugMode() const;

        virtual void reportErrorWarning(const char* warningString);
    private:
        static const u32 MaxLines = 2048;
        static const u32 MaxVertices= MaxLines*2;

        s32 debugMode_;
        u32 numLines_;
        lgraphics::VertexDeclarationRef decl_;
        LineVertex vertices_[MaxVertices];

        LineShaderVS shaderVS_;
        LineShaderPS shaderPS_;
    };
}
#endif //INC_DYNAMICSDEBUGDRAW_H__
