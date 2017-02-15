/**
@file ComponentSprite2D.cpp
@author t-sakai
@date 2016/12/20 create
*/
#include "ecs/ComponentSprite2D.h"
#include "ecs/ECSManager.h"
#include "ecs/ComponentRenderer2DManager.h"
#include "ecs/Entity.h"
#include "ecs/ComponentGeometric.h"
#include "render/RenderQueue2D.h"
#include "render/RenderContext2D.h"

namespace lfw
{
    namespace
    {
        inline ComponentRenderer2DManager* getManager()
        {
            return ECSManager::getInstance().getComponentManager<ComponentRenderer2DManager>();
        }
    }

    ComponentSprite2D::ComponentSprite2D()
        :color_(0xFFFFFFFFU)
    {
        rect_.zero();
        setTexcoord(0.0f, 0.0f, 0.0f, 0.0f);
    }

    ComponentSprite2D::~ComponentSprite2D()
    {
    }

    void ComponentSprite2D::onCreate()
    {
    }

    void ComponentSprite2D::onStart()
    {
    }

    void ComponentSprite2D::update()
    {
    }

    void ComponentSprite2D::onDestroy()
    {
    }

    void ComponentSprite2D::addQueue(RenderQueue2D& queue)
    {
        const ComponentGeometric* geometric = getEntity().getGeometric();
        queue.add(geometric->getPosition().z_, getMaterialId(), this);
    }

    void ComponentSprite2D::draw(RenderContext2D& context)
    {
        if(texture_.valid()){
            context.addRect(rect_, color_, texcoord_, srv_, context.getDefaultPixelShader());
        }
    }

    void ComponentSprite2D::setScreenRect(f32 left, f32 top, f32 right, f32 bottom)
    {
        rect_.set(left, top, right, bottom);
    }

    void ComponentSprite2D::setTexcoord(f32 u0, f32 v0, f32 u1, f32 v1)
    {
        texcoord_[0] = lcore::toBinary16Float(u0);
        texcoord_[1] = lcore::toBinary16Float(v0);
        texcoord_[2] = lcore::toBinary16Float(u1);
        texcoord_[3] = lcore::toBinary16Float(v1);
    }

    void ComponentSprite2D::setTexture(lgfx::Texture2DRef& texture, lgfx::ShaderResourceViewRef& srv)
    {
        texture_ = texture;
        srv_ = srv;
    }
}
