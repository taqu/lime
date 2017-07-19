#ifndef INC_LFRAMEWORK_COMPONENTCANVASELEMENT_H__
#define INC_LFRAMEWORK_COMPONENTCANVASELEMENT_H__
/**
@file ComponentCanvasElement.h
@author t-sakai
@date 2017/05/02 create
*/
#include "ComponentBehavior.h"

namespace lfw
{
    class ComponentCanvas;

    class ComponentCanvasElement : public ComponentBehavior
    {
    public:
        static u8 category(){ return ECSCategory_Behavior;}
        static u32 type(){ return ECSType_CanvasElement;}

        ComponentCanvasElement();
        virtual ~ComponentCanvasElement();

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void onDestroy();

        void setDirty();

        virtual void updateMesh(ComponentCanvas& canvas) =0;
    protected:
        ComponentCanvasElement(const ComponentCanvasElement&) = delete;
        ComponentCanvasElement& operator=(const ComponentCanvasElement&) = delete;

        ComponentCanvas* getCanvas();

        ComponentCanvas* canvas_;
    };
}
#endif //INC_LFRAMEWORK_COMPONENTCANVASELEMENT_H__
