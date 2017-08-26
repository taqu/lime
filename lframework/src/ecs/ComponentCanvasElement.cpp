/**
@file ComponentCanvasElement.cpp
@author t-sakai
@date 2017/05/21 create
*/
#include "ecs/ComponentCanvasElement.h"
#include "ecs/ComponentGeometric.h"
#include "ecs/ComponentCanvas.h"

namespace lfw
{
    ComponentCanvasElement::ComponentCanvasElement()
        :canvas_(NULL)
    {
    }

    ComponentCanvasElement::~ComponentCanvasElement()
    {
    }

    void ComponentCanvasElement::onCreate()
    {
    }

    void ComponentCanvasElement::onStart()
    {
        canvas_ = getCanvas();
        if(NULL != canvas_){
            canvas_->add(this);
        }
    }

    void ComponentCanvasElement::update()
    {
    }

    void ComponentCanvasElement::onDestroy()
    {
        if(NULL != canvas_){
            canvas_->remove(this);
        }
    }

    void ComponentCanvasElement::setDirty()
    {
        if(NULL != canvas_){
            canvas_->requestUpdate();
        }
    }

    ComponentCanvas* ComponentCanvasElement::getCanvas()
    {
        //e‚É‚ ‚éCanvas‚ğ’Tõ
        ComponentCanvas* canvas = getEntity().getComponent<ComponentCanvas>();
        if(NULL != canvas){
            return canvas;
        }

        ComponentGeometric* geometric = getEntity().getGeometric();
        LASSERT(NULL != geometric);
        do{
            canvas = geometric->getEntity().getComponent<ComponentCanvas>();
            if(NULL != canvas){
                return canvas;
            }
            geometric = geometric->getParent();
        }while(NULL != geometric);

        return NULL;
    }
}
