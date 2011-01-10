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

namespace lrender
{
    class Batch;

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

        void add(Pass pass, Batch* batch);
        void remove(Pass pass, Batch* batch);

        void beginDraw();
        void draw();
        void endDraw();

        inline lscene::Scene& getScene();

        void setClearTarget(u32 clearTarget);
    private:
        RenderingSystem(const RenderingSystem&);
        RenderingSystem& operator=(const RenderingSystem&);

        u32 clearTarget_;
        PassMain passMain_;
    };

    inline lscene::Scene& RenderingSystem::getScene()
    {
        return passMain_.getScene();
    }

    inline void RenderingSystem::setClearTarget(u32 clearTarget)
    {
        clearTarget_ = clearTarget;
    }
}
#endif //INC_RENDERINGSYSTEM_H__
