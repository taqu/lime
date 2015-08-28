#ifndef INC_LCOLLIDE_LCOLLIDE_H__
#define INC_LCOLLIDE_LCOLLIDE_H__
/**
@file lcollide.h
@author t-sakai
@date 2013/05/14 create
*/
#include "../lscene.h"
#include <lcore/lcore.h>
#include <lmath/lmath.h>

namespace lmath
{
    class Ray;
}

namespace lcollide
{
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;
    using lcore::u8;
    using lcore::u16;
    using lcore::u32;
    using lcore::f32;
    using lcore::f64;

    using lcore::Char;

    enum CollisionType
    {
        CollisionType_Sphere = 0,
        CollisionType_Ray,
        CollisionType_AABB,
        CollisionType_Num,
    };

    enum CollisionInfoType
    {
        CollisionInfoType_NormalDepth,
        CollisionInfoType_ClosestPoint,
    };

    static const s32 MaxCollisionGroup = 16;


    //-------------------------------------------------------
    //---
    //--- CollisionInfo
    //---
    //-------------------------------------------------------
    struct CollisionInfo
    {
        s32 type_;
        lmath::Vector4 info_;
    };


    //-------------------------------------------------------
    //---
    //--- ColliderBase
    //---
    //-------------------------------------------------------
    class ColliderBase : public lscene::ColliderAllocator
    {
    public:
        ColliderBase* getPrev(){ return prev_; }
        ColliderBase* getNext(){ return next_; }

        bool empty() const
        {
            return this == next_;
        }

        void reset()
        {
            prev_ = this;
            next_ = this;
        }

        void unlink()
        {
            next_->prev_ = prev_;
            prev_->next_ = next_;
            prev_ = this;
            next_ = this;
        }

        void link(ColliderBase* nextNode)
        {
            prev_ = nextNode->prev_;
            next_ = nextNode;
            nextNode->prev_ = this;
            prev_->next_ = this;
        }

        const void* getData() const{ return data_;}
        void* getData(){ return data_;}
        void setData(void* data){ data_ = data;}

        virtual bool test(const lmath::Ray& /*ray*/, f32& /*t*/) const
        {
            return false;
        }

        u16 getGroup() const{ return group_;}
        void setGroup(u16 group){ group_ = group;}
        u16 getType() const{ return type_;}
        void setType(u16 type){ type_ = type;}
    protected:
        ColliderBase()
            :prev_(this)
            ,next_(this)
            ,data_(NULL)
            ,group_(0)
            ,type_(0)
        {}

        ColliderBase(void* data, u16 group, u16 type)
            :prev_(this)
            ,next_(this)
            ,data_(data)
            ,group_(group)
            ,type_(type)
        {}

        virtual ~ColliderBase()
        {}

        ColliderBase* prev_;
        ColliderBase* next_;
        void* data_;

        u16 group_;
        u16 type_;
    };

    //-------------------------------------------------------
    //---
    //--- ColliderBase2
    //---
    //-------------------------------------------------------
    class ColliderBase2 : public ColliderBase
    {
    public:
        virtual void getBoundingBox(lmath::Vector2& bmin, lmath::Vector2& bmax) =0;

    protected:
        ColliderBase2()
        {}

        ColliderBase2(void* data, u16 group, u16 type)
            :ColliderBase(data, group, type)
        {}
    };

    //-------------------------------------------------------
    //---
    //--- ColliderBase3
    //---
    //-------------------------------------------------------
    class ColliderBase3 : public ColliderBase
    {
    public:
        virtual void getBoundingBox(lmath::Vector4& bmin, lmath::Vector4& bmax) =0;

    protected:
        ColliderBase3()
        {}

        ColliderBase3(void* data, u16 group, u16 type)
            :ColliderBase(data, group, type)
        {}
    };

    //-------------------------------------------------------
    //---
    //--- CollisionPair
    //---
    //-------------------------------------------------------
    struct CollisionPair
    {
        CollisionPair(ColliderBase* node0, ColliderBase* node1)
            :node0_(node0)
            ,node1_(node1)
        {}

        //lmath::Vector4 normalDepth_; //node0Ç©ÇÁnode1Ç÷ÇÃè’ìÀï˚å¸ÅAä—í ãóó£

        ColliderBase* node0_;
        ColliderBase* node1_;
    };

    //-------------------------------------------------------
    //---
    //--- RayHitInfo
    //---
    //-------------------------------------------------------
    struct RayHitInfo
    {
        ColliderBase* collider_;
        f32 t_;
    };
}
#endif //INC_LCOLLIDE_LCOLLIDE_H__
