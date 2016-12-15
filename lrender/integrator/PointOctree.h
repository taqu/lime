#ifndef INC_LRENDER_POINTOCTREE_H__
#define INC_LRENDER_POINTOCTREE_H__
/**
@file PointOctree.h
@author t-sakai
@date 2016/02/08 create
*/
#include "../lrender.h"
#include <lcore/StackAllocator.h>
#include "../core/Spectrum.h"
#include "../core/AABB.h"

namespace lrender
{
    /**
    @brief ポイントクラウドの八分木
    */
    template<class T, class U>
    class PointOctreeNode
    {
    public:
        static const f32 Epsilon;
        static const s32 MaxNumNodePoints = 8;
        typedef PointOctreeNode<T, U> this_type;
        typedef T value_type;
        typedef lcore::StackAllocator<lcore::DefaultAllocator> NodeAllocator;

        static AABB getChildBound(s32 index, const AABB& bound, const Vector3& center)
        {
            Vector3 center = bound.center();
            AABB b;
            b.bmin_.x_ = (index & 0x04U) ? center.x_ : bound.bmin_.x_;
            b.bmax_.x_ = (index & 0x04U) ? bound.bmax_.x_  : center.x_;
            b.bmin_.y_ = (index & 0x02U) ? center.y_ : bound.bmin_.y_;
            b.bmax_.y_ = (index & 0x02U) ? bound.bmax_.y_  : center.y_;
            b.bmin_.z_ = (index & 0x01U) ? center.z_ : bound.bmin_.z_;
            b.bmax_.z_ = (index & 0x01U) ? bound.bmax_.z_  : center.z_;
            return b;
        }

        PointOctreeNode();
        ~PointOctreeNode();

        void insert(const AABB& bound, value_type* point, NodeAllocator& nodeAllocator);
        void initialize();

        Color3 Mo(const AABB& bound, const Vector3& point, const U& diffusion, f32 maxSolidAngle) const;

        bool isLeaf_;
        Vector3 point_;
        Color3 E_;
        f32 area_;
        union
        {
            PointOctreeNode* children_[MaxNumNodePoints];
            value_type* points_[MaxNumNodePoints];
        };

    private:
        bool addPoint(value_type* point)
        {
            for(s32 i=0; i<MaxNumNodePoints; ++i){
                if(NULL == points_[i]){
                    points_[i] = point;
                    return true;
                }
            }
            return false;
        }
    };

    template<class T, class U>
    PointOctreeNode<T, U>::PointOctreeNode()
        :isLeaf_(true)
        ,area_(0.0f)
    {
        point_.zero();
        E_ = Color3::black();
        for(s32 i=0; i<MaxNumNodePoints; ++i){
            points_[i] = NULL;
        }
    }

    template<class T, class U>
    PointOctreeNode<T, U>::~PointOctreeNode()
    {
    }

    template<class T, class U>
    void PointOctreeNode<T, U>::insert(const AABB& bound, value_type* point, NodeAllocator& nodeAllocator)
    {
        Vector3 center = bound.center();
        if(isLeaf_){
            if(addPoint(point)){
                return;
            }
            isLeaf_ = false;
            value_type* tmpPoints[MaxNumNodePoints];
            for(s32 i=0; i<MaxNumNodePoints; ++i){
                tmpPoints[i] = points_[i];
                children_[i] = NULL;
            }

            for(s32 i=0; i<MaxNumNodePoints; ++i){
                value_type* p = tmpPoints[i];
                int index = octreeChildIndex(p->point_, center);
                if(NULL == children_[index]){
                    children_[index] = lcore::construct<this_type>(nodeAllocator.allocate());
                }
                AABB childBound = octreeChildBound(index, center, bound);
                children_[index]->insert(childBound, p, nodeAllocator);
            }
        }
        int index = octreeChildIndex(point->point_, center);
        if(NULL == children_[index]){
            children_[index] = lcore::construct<this_type>(nodeAllocator.allocate());
        }
        AABB childBound = octreeChildBound(index, center, bound);
        children_[index]->insert(childBound, point, nodeAllocator);
    }

    template<class T, class U>
    void PointOctreeNode<T, U>::initialize()
    {
        //area_ = 0.0f;
        //point_.zero();
        //E_ = Color3::black();
        float totalWeight = 0.0f;
        s32 count = 0;

        if(isLeaf_){
            for(;count<MaxNumNodePoints; ++count){
                if(NULL == points_[count]){
                    break;
                }
                f32 weight = points_[count]->E_.getLuminance();
                E_ += points_[count]->E_;
                point_ = muladd(weight, points_[count]->point_, point_);
                area_ += points_[count]->area_;
                totalWeight += weight;
            }
            if(1.0e-5f<totalWeight){
                point_ /= totalWeight;
            }
            E_ /= count;

        }else{
            for(s32 i=0; i<MaxNumNodePoints; ++i){
                if(NULL == children_[i]){
                    continue;
                }
                ++count;
                children_[i]->initialize();

                f32 weight = children_[i]->E_.getLuminance();
                E_ += children_[i]->E_;
                point_ = muladd(weight, children_[i]->point_, point_);
                area_ += children_[i]->area_;
                totalWeight += weight;
            }
            if(1.0e-5f<totalWeight){
                point_ /= totalWeight;
            }
            E_ /= count;
        }
    }

    template<class T, class U>
    Color3 PointOctreeNode<T, U>::Mo(const AABB& bound, const Vector3& point, const U& diffusion, f32 maxSolidAngle) const
    {

#if 1
        if(isLeaf_){
            Color3 mo = Color3::black();
            for(s32 i=0; i<MaxNumNodePoints; ++i){
                if(NULL == points_[i]){
                    break;
                }
                f32 d = distance(point, points_[i]->point_);

                mo += diffusion(d) * points_[i]->E_ * points_[i]->area_;
            }
            return mo;

        } else {
            f32 sqrDistance = bound.sqrDistance(point);
            f32 omega = (1.0e-5f<sqrDistance)? area_/sqrDistance : 0.0f;
            if(sqrDistance<1.0e-5f || maxSolidAngle<omega){
                Color3 mo = Color3::black();
                Vector3 center = bound.center();
                for(s32 i = 0; i < MaxNumNodePoints; ++i){
                    if(NULL == children_[i]){
                        continue;
                    }
                    AABB childBound = octreeChildBound(i, center, bound);
                    mo += children_[i]->Mo(childBound, point, diffusion, maxSolidAngle);
                }
                return mo;
            }else{
                f32 d = distance(point_, point);
                return diffusion(d) * E_ * area_;
            }
        }
#else
        f32 sqrDistance = point_.distanceSqr(point);
        f32 solidAngle = area_ / sqrDistance;
        if(solidAngle<maxSolidAngle && !bound.contains(point)){
            return diffusion(sqrDistance) * E_ * area_;
        }

        Color3 mo = Color3::black();
        if(isLeaf_){
            for(s32 i=0; i<MaxNumNodePoints; ++i){
                if(NULL == points_[i]){
                    break;
                }
                mo += diffusion(point.distanceSqr(points_[i]->point_)) * points_[i]->E_ * points_[i]->area_;
            }
        }else{
            Vector3 center = bound.center();
            for(s32 i=0; i<MaxNumNodePoints; ++i){
                if(NULL == children_[i]){
                    continue;
                }
                AABB childBound = octreeChildBound(i, center, bound);
                mo += children_[i]->Mo(childBound, point, diffusion, maxSolidAngle);
            }
        }
        return mo;
#endif
    }
}
#endif //INC_LRENDER_POINTOCTREE_H__
