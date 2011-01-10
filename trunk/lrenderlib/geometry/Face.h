#ifndef INC_LRENDER_FACE_H__
#define INC_LRENDER_FACE_H__
/**
@file Face.h
@author t-sakai
@date 2009/12/21 create
*/
#include "../lrendercore.h"
#include <lmath/lmathutil.h>

namespace lmath
{
    class Ray;
}

namespace lrender
{
    struct HitRecord;

    template<class Owner>
    class Face
    {
    public:
        typedef Owner owner_type;

        Face(){}
        ~Face(){}

        static const int VERTEX_NUM = 3;

        //----------------------------------
        bool calcBBox(Vector3& bmin, Vector3& bmax) const;
        bool calcMedian(Vector3& median) const;

        bool hit(HitRecord& hitRecord, const lmath::Ray& ray) const;

        u32 vertices_[VERTEX_NUM];
        u32 material_;

        owner_type* owner_;
    };

    template<class Owner>
    bool Face::calcBBox(Vector3& bmin, Vector3& bmax) const
    {
        LASSERT(owner_ != NULL);
        const Vertex &v0 = owner_->getVertex(vertices_[0]);
        const Vertex &v1 = owner_->getVertex(vertices_[1]);
        const Vertex &v2 = owner_->getVertex(vertices_[2]);
        lmath::calcBBox(bmin, bmax, v0, v1, v2);
    }

    template<class Owner>
    bool Face::calcMedian(Vector3& median) const
    {
        LASSERT(owner_ != NULL);
        const Vertex &v0 = owner_->getVertex(vertices_[0]);
        const Vertex &v1 = owner_->getVertex(vertices_[1]);
        const Vertex &v2 = owner_->getVertex(vertices_[2]);
        lmath::calcMedian(median, v0, v1, v2);
    }

    template<class Owner>
    bool Face::hit(HitRecord& hitRecord, const lmath::Ray& ray) const
    {
        LASSERT(owner_ != NULL);

        const Vertex &v0 = owner_->getVertex(vertices_[0]);
        const Vertex &v1 = owner_->getVertex(vertices_[1]);
        const Vertex &v2 = owner_->getVertex(vertices_[2]);

        f32 t = std::numeric_limits<f32>::max();
        f32 u = 0.0f; 
        f32 v = 0.0f;

        bool bHit = false;

        f32 tmpT = t;
        f32 tmpU = 0.0f;
        f32 tmpV = 0.0f;
        if(lmath::testRayTriangle(tmpT, tmpU, tmpV, ray, v0, v1, v2)){
            if(tmpT<t){
                bHit = true;
                t = tmpT;
                u = tmpU;
                v = tmpV;
            }
        }

        if(bHit){
            hitRecord.t_ = t;
            hitRecord.u_ = u;
            hitRecord.v_ = v;
            hitRecord.index_ = index;
            hitRecord.shape_ = this;

            Vector3 &hitPos = hitRecord.position_;
            hitPos = ray.direction_;
            hitPos *= static_cast<f32>(hitRecord.t_);
            hitPos += ray.origin_;
        }

        ibuffer.unlock();
        vbuffer.unlock();
        return bHit;
    }

    /// KDTree用
    class Element
    {
    public:
        Element()
            :shape_(NULL)
            ,index_(0)
        {
        }

        Element(Shape* shape, u32 index);

        ~Element()
        {
        }

        void set(Shape* shape, u32 index);

        //----------------------------------
        const Vector3& getBBoxMin() const{ return bmin_;}
        const Vector3& getBBoxMax() const{ return bmax_;}

        //----------------------------------
        const Vector3& getMedian() const{ return median_;}

        Shape* getShape(){ return shape_;}
        const Shape* getShape() const{ return shape_;}
        u32 getIndex() const{ return index_;}

        bool hit(HitRecord& hitRecord, const lmath::Ray& ray) const;
    private:

        Shape *shape_; /// 面の親
        u32 index_; /// 面インデックス

        Vector3 bmin_;
        Vector3 bmax_;
        Vector3 median_;
    };
}

#endif //INC_LRENDER_FACE_H__
