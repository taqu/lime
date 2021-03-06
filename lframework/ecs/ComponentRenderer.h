#ifndef INC_LFRAMEWORK_COMPONENTRENDERER_H_
#define INC_LFRAMEWORK_COMPONENTRENDERER_H_
/**
@file ComponentRenderer.h
@author t-sakai
@date 2016/10/18 create
*/
#include "Component.h"
#include <lmath/lmath.h>

namespace lfw
{
    class Camera;
    class RenderQueue;
    class RenderContext;

    class ComponentRenderer : public Behavior
    {
    public:
        static u8 category(){ return ECSCategory_Renderer;}
        static u32 type(){ return ECSType_Renderer;}

        static const u32 Flag_ShadowCast = (0x01U<<0);
        static const u32 Flag_Opaque = (0x01U<<1);
        static const u32 Flag_Transparent = (0x01U<<2);

        virtual ~ComponentRenderer();

        const Entity& getEntity() const;
        Entity& getEntity();
        virtual u32 getType() const;

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void postUpdate();
        virtual void onDestroy();
        virtual bool addQueue(RenderQueue& queue);
        virtual void drawDepth(RenderContext& renderContext);
        virtual void drawGBuffer(RenderContext& renderContext);
        virtual void drawOpaque(RenderContext& renderContext);
        virtual void drawTransparent(RenderContext& renderContext);
        virtual void getAABB(lmath::lm128& bmin, lmath::lm128& bmax);

        inline bool checkFlag(u32 flag) const;
        inline void setFlag(u32 flag);
        inline void resetFlag(u32 flag);

        inline s32 getLayer() const;
        inline void setLayer(s32 layer);
        inline s32 getSortLayer() const;
        inline void setSortLayer(s32 sortLayer);

    protected:
        ComponentRenderer(const ComponentRenderer&) = delete;
        ComponentRenderer& operator=(const ComponentRenderer&) = delete;

        ComponentRenderer();

        u32 flags_;
        s32 layer_;
        s32 sortLayer_;
    };

    inline bool ComponentRenderer::checkFlag(u32 flag) const
    {
        return 0 != (flags_ & flag);
    }

    inline void ComponentRenderer::setFlag(u32 flag)
    {
        flags_ |= flag;
    }

    inline void ComponentRenderer::resetFlag(u32 flag)
    {
        flags_ &= ~flag;
    }

    inline s32 ComponentRenderer::getLayer() const
    {
        return layer_;
    }

    inline void ComponentRenderer::setLayer(s32 layer)
    {
        layer_ = layer;
    }

    inline s32 ComponentRenderer::getSortLayer() const
    {
        return sortLayer_;
    }

    inline void ComponentRenderer::setSortLayer(s32 sortLayer)
    {
        sortLayer_ = sortLayer;
    }
}
#endif //INC_LFRAMEWORK_COMPONENTRENDERER_H_
