#ifndef INC_RENDERINGSYSTEM_H__
#define INC_RENDERINGSYSTEM_H__
/**
@file RenderingSystem.h
@author t-sakai
@date 2010/11/18 create

*/
#include "lrender.h"
#include "PassMain.h"
#include "../scene/Scene.h"
#include <lgraphics/api/Enumerations.h>

namespace lrender
{
    class Drawable;
    struct RenderBufferBatchState;
    class RenderBuffer;

    enum Pass
    {
        Pass_Main,
        Pass_Num,
    };

    class RenderingSystem
    {
    public:
        RenderingSystem();
        ~RenderingSystem();

        void initialize();
        void terminate();

        void add(Pass pass, Drawable* drawable);
        void remove(Pass pass, Drawable* drawable);

        void beginDraw();
        void draw();
        void endDraw();

        inline lscene::Scene& getScene();
        //inline RenderBufferBatchState& getRenderBufferBatchState();

        void setClearTarget(u32 clearTarget);

        //bool createRenderBuffer(RenderBuffer& buffer, u32 width, u32 height, lgraphics::BufferFormat format);
        //bool createRenderBufferFromTarget(RenderBuffer& buffer);
    private:
        RenderingSystem(const RenderingSystem&);
        RenderingSystem& operator=(const RenderingSystem&);

        //RenderBufferBatchState* renderBufferBatchState_;
        u32 clearTarget_;
        PassMain passMain_;
    };

    inline lscene::Scene& RenderingSystem::getScene()
    {
        return passMain_.getScene();
    }

    //inline RenderBufferBatchState& RenderingSystem::getRenderBufferBatchState()
    //{
    //    return *renderBufferBatchState_;
    //}

    inline void RenderingSystem::setClearTarget(u32 clearTarget)
    {
        clearTarget_ = clearTarget;
    }
}
#endif //INC_RENDERINGSYSTEM_H__
