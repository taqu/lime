#ifndef INC_VIEWER_LIGHT_H__
#define INC_VIEWER_LIGHT_H__
/**
@file Light.h
@author t-sakai
@date 2011/02/26 create
*/
namespace pmm
{
    class LightAnimPack;
}

namespace viewer
{
    //------------------------------------------------
    //---
    //--- Light
    //---
    //------------------------------------------------
    class Light
    {
    public:
        Light();
        ~Light();

        void update();

        void setLightAnim(pmm::LightAnimPack* pack, u32 lastFrame);

        void reset();
    private:

        u32 frame_;
        u32 lastFrame_;

        pmm::LightAnimPack* lightAnimPack_;
    };
}

#endif //INC_VIEWER_LIGHT_H__
