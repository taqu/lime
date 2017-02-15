#ifndef INC_SAMPLES_TEXT00_H__
#define INC_SAMPLES_TEXT00_H__
/**
@file Text00.h
@author t-sakai
@date 2016/12/23 create
*/
#include <lframework/ecs/ComponentFontRenderer.h>

namespace lfw
{
    class ComponentLogical;
    class ComponentGeometric;
}

namespace debug
{
    class Text00 : public lfw::ComponentFontRenderer
    {
    public:
        Text00();
        virtual ~Text00();

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void postUpdate();
        virtual void onDestroy();

        static bool traverse(lfw::s32 depth, const lfw::ComponentLogical& logical, void* userData);
        static bool traverse(lfw::s32 depth, const lfw::ComponentGeometric& geometric, void* userData);
    };
}
#endif //INC_SAMPLES_TEXT00_H__
