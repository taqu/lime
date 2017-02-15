#ifndef INC_LFRAMEWORK_LCOLLIDE_H__
#define INC_LFRAMEWORK_LCOLLIDE_H__
/**
@file lcollide.h
@author t-sakai
@date 2016/12/19 create
*/
#include "../lframework.h"
#include <lmath/Vector4.h>

namespace lmath
{
    class Ray;
}

namespace lfw
{
    static const s32 MaxCollisionGroup = 16;
    static const s32 MaxColliders = 0xFFFE;

    //-------------------------------------------------------
    //---
    //--- CollideCallback
    //---
    //-------------------------------------------------------
    struct CollisionInfo;
    class ColliderBase;
    typedef void(*CollideCallback)(ColliderBase* self, ColliderBase* opposite, CollisionInfo& info);

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
    class ColliderBase
    {
    public:
        static const u16 Invalid = 0xFFFFU;

        ColliderBase()
            :callback_(NULL)
            ,data_(NULL)
            ,id_(Invalid)
            ,next_(Invalid)
            ,group_(0)
            ,type_(0)
        {}

        u16 getID() const{ return id_;}
        void setID(u16 id){ id_ = id;}
        u16 getNext() const{ return next_;}
        void setNext(u16 next){ next_ = next;}

        void reset()
        {
            id_ = Invalid;
            next_ = Invalid;
        }

        const void* getData() const{ return data_;}
        void* getData(){ return data_;}
        void setData(void* data){ data_ = data;}
        template<class T>
        T* castData(){ return reinterpret_cast<T*>(data_);}

        virtual bool test(const lmath::Ray& /*ray*/, f32& /*t*/) const
        {
            return false;
        }

        u16 getGroup() const{ return group_;}
        void setGroup(u16 group){ group_ = group;}
        u16 getType() const{ return type_;}
        void setType(u16 type){ type_ = type;}

        void setCallback(CollideCallback callback)
        {
            callback_ = callback;
        }

        void invoke(ColliderBase* opposite, CollisionInfo& info)
        {
            if(NULL != callback_){
                callback_(this, opposite, info);
            }
        }

    protected:
        ColliderBase(CollideCallback callback, void* data, u16 id, u16 next, u16 group, u16 type)
            :callback_(callback)
            ,data_(data)
            ,id_(id)
            ,next_(next)
            ,group_(group)
            ,type_(type)
        {}

        virtual ~ColliderBase()
        {}

        CollideCallback callback_;
        void* data_;

        u16 id_;
        u16 next_;
        u16 group_;
        u16 type_;
    };

    //-------------------------------------------------------
    //---
    //--- ColliderBase2D
    //---
    //-------------------------------------------------------
    class ColliderBase2D : public ColliderBase
    {
    public:
        virtual void getBoundingBox(lmath::Vector2& bmin, lmath::Vector2& bmax) =0;

    protected:
        ColliderBase2D()
        {}

        ColliderBase2D(CollideCallback callback, void* data, u16 id, u16 next, u16 group, u16 type)
            :ColliderBase(callback, data, id, next, group, type)
        {}
    };

    //-------------------------------------------------------
    //---
    //--- ColliderBase3D
    //---
    //-------------------------------------------------------
    class ColliderBase3D : public ColliderBase
    {
    public:
        virtual void getBoundingBox(lmath::Vector4& bmin, lmath::Vector4& bmax) =0;

    protected:
        ColliderBase3D()
        {}

        ColliderBase3D(CollideCallback callback, void* data, u16 id, u16 next, u16 group, u16 type)
            :ColliderBase(callback, data, id, next, group, type)
        {}
    };

    //-------------------------------------------------------
    //---
    //--- CollisionPair
    //---
    //-------------------------------------------------------
    struct CollisionPair
    {
        static CollisionPair construct(ColliderBase* node0, ColliderBase* node1)
        {
            return {node0, node1};
        }

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
#endif //INC_LFRAMEWORK_LCOLLIDE_H__
