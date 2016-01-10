#ifndef INC_LSCENE_COLLIDABLE_H__
#define INC_LSCENE_COLLIDABLE_H__
/**
@file Collidable.h
@author t-sakai
@date 2016/01/04 create
*/
#include "lscene.h"
#include "SceneRenderable.h"

namespace lcollide
{
    struct CollisionInfo;
}

namespace lscene
{
    class Collidable : public SceneRenderable
    {
    public:
        virtual ~Collidable()
        {}

        inline u16 getGroup() const;
        inline void setGroup(u16 group);
        inline u16 getType() const;
        inline void setType(u16 type);

        virtual void onCollide(Collidable* opposite, lcollide::CollisionInfo& info) =0;
    protected:
        //Collidable()
        //    :group_(0)
        //    ,type_(0)
        //{}

        Collidable(u16 group, u16 type)
            :group_(group)
            ,type_(type)
        {}

        u16 group_;
        u16 type_;
    };

    inline u16 Collidable::getGroup() const
    {
        return group_;
    }

    inline void Collidable::setGroup(u16 group)
    {
        group_ = group;
    }

    inline u16 Collidable::getType() const
    {
        return type_;
    }

    inline void Collidable::setType(u16 type)
    {
        type_ = type;
    }
}
#endif //INC_LSCENE_COLLIDABLE_H__
