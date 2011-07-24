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

        void initialize();
        void update(u32 frame);

        void setLightAnim(pmm::LightAnimPack* pack);

        void reset();

    private:
        pmm::LightAnimPack* lightAnimPack_;
    };
}

#endif //INC_VIEWER_LIGHT_H__
