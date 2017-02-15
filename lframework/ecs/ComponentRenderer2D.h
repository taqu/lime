#ifndef INC_LFRAMEWORK_COMPONENTRENDERER2D_H__
#define INC_LFRAMEWORK_COMPONENTRENDERER2D_H__
/**
@file ComponentRenderer2D.h
@author t-sakai
@date 2016/12/20 create
*/
#include "Component.h"

namespace lfw
{
    class Entity;
    class Camera;
    class RenderQueue2D;
    class RenderContext2D;

    class ComponentRenderer2D : public Behavior
    {
    public:
        static u8 category(){ return ECSCategory_Renderer2D;}
        static u32 type(){ return ECSType_Renderer2D;}

        virtual ~ComponentRenderer2D();

        const Entity& getEntity() const;
        Entity getEntity();
        virtual u32 getType() const;

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void onDestroy();
        virtual void addQueue(RenderQueue2D& queue);
        virtual void draw(RenderContext2D& context);

        inline bool checkFlag(u32 flag) const;
        inline void setFlag(u32 flag);
        inline void resetFlag(u32 flag);

        inline s32 getLayer() const;
        inline void setLayer(s32 layer);
        inline s32 getSortLayer() const;
        inline void setSortLayer(s32 sortLayer);

    protected:
        ComponentRenderer2D(const ComponentRenderer2D&);
        ComponentRenderer2D& operator=(const ComponentRenderer2D&);

        ComponentRenderer2D();

        u32 flags_;
        s32 layer_;
        s32 sortLayer_;
    };

    inline bool ComponentRenderer2D::checkFlag(u32 flag) const
    {
        return 0 != (flags_ & flag);
    }

    inline void ComponentRenderer2D::setFlag(u32 flag)
    {
        flags_ |= flag;
    }

    inline void ComponentRenderer2D::resetFlag(u32 flag)
    {
        flags_ &= ~flag;
    }

    inline s32 ComponentRenderer2D::getLayer() const
    {
        return layer_;
    }

    inline void ComponentRenderer2D::setLayer(s32 layer)
    {
        layer_ = layer;
    }

    inline s32 ComponentRenderer2D::getSortLayer() const
    {
        return sortLayer_;
    }

    inline void ComponentRenderer2D::setSortLayer(s32 sortLayer)
    {
        sortLayer_ = sortLayer;
    }
}
#endif //INC_LFRAMEWORK_COMPONENTRENDERER2D_H__
