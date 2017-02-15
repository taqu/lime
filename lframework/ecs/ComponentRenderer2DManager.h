#ifndef INC_LFRAMEWORK_COMPONENTRENDERERMANAGER2D_H__
#define INC_LFRAMEWORK_COMPONENTRENDERERMANAGER2D_H__
/**
@file ComponentRenderer2DManager.h
@author t-sakai
@date 2016/12/20 create
*/
#include "ComponentManager.h"
#include <lcore/BitSet.h>
#include <lmath/lmath.h>
#include <lgraphics/lgraphics.h>

namespace lmath
{
    class Vector4;
}

namespace lfw
{
    class ComponentRenderer2D;

    class ComponentRenderer2DManager : public ComponentManager
    {
    public:
        static const u8 Category = ECSCategory_Renderer2D;

        typedef lcore::ArrayPOD<ComponentRenderer2D*, lcore::array_static_inc_size<128> > ComponentRendererArray;

        struct Data : public ComponentDataCommon<Data>
        {

        };

        ComponentRenderer2DManager(s32 capacity=DefaultComponentManagerCapacity, s32 expandSize=DefaultComponentManagerExpandSize);

        virtual ~ComponentRenderer2DManager();

        virtual u8 category() const;

        ID create(Entity entity, u32 type, ComponentRenderer2D* component);
        virtual void destroy(ID id);

        virtual Behavior* getBehavior(ID id);
        virtual void initialize();
        virtual void terminate();

        void preUpdateComponents();
        virtual void updateComponent(ID id);
        void postUpdateComponents();

        inline const Data& getData(ID id) const;

        inline ComponentRendererArray& getComponents();
    protected:
        ComponentRenderer2DManager(const ComponentRenderer2DManager&);
        ComponentRenderer2DManager& operator=(const ComponentRenderer2DManager&);

        IDTable ids_;
        ComponentRenderer2D** components_;
        Data* data_;

        ComponentRendererArray componentRenderers_;
    };

    inline const ComponentRenderer2DManager::Data& ComponentRenderer2DManager::getData(ID id) const
    {
        return data_[id.index()];
    }

    inline ComponentRenderer2DManager::ComponentRendererArray& ComponentRenderer2DManager::getComponents()
    {
        return componentRenderers_;
    }
}
#endif //INC_LFRAMEWORK_COMPONENTRENDERERMANAGER2D_H__
