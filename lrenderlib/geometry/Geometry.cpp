/**
@file Geometry.cpp
@author t-sakai
@date 2010/01/12 create

*/
#include <lmath/geometry/RayTest.h>
#include <lmath/geometry/Ray.h>

#include "Geometry.h"
#include "GeometryBuffer.h"
#include "Face.h"
#include "ShadingGeometry.h"
#include "HitRecord.h"

namespace lrender
{
    using namespace lgraphics;

    //----------------------------------------------------
    //---
    //--- Geometry
    //---
    //----------------------------------------------------
    Geometry::Geometry()
        :count_(0),
        materialIndex_(-1)
    {
    }

    Geometry::Geometry(
        GeomBufferPtr& geomBuffer,
        u32 count,
        s32 materialIndex)
        :geomBuffer_(geomBuffer),
        count_(count),
        materialIndex_(materialIndex)
    {
        LASSERT(geomBuffer_);
    }

    Geometry::~Geometry()
    {
    }

    PrimitiveType Geometry::getType() const
    {
        return geomBuffer_->getType();
    }

    VertexBufferUP& Geometry::getVertexBuffer()
    {
        return geomBuffer_->getVertexBuffer();
    }

    const VertexBufferUP& Geometry::getVertexBuffer() const
    {
        return geomBuffer_->getVertexBuffer();
    }

    IndexBufferUP& Geometry::getIndexBuffer()
    {
        return geomBuffer_->getIndexBuffer();
    }

    const IndexBufferUP& Geometry::getIndexBuffer() const
    {
        return geomBuffer_->getIndexBuffer();
    }

    VertexDeclaration& Geometry::getDecl()
    {
        return geomBuffer_->getDecl();
    }

    void Geometry::swap(Geometry& rhs)
    {
        geomBuffer_.swap(rhs.geomBuffer_);
        std::swap(count_, rhs.count_);
        std::swap(materialIndex_, rhs.materialIndex_);
    }

    void Geometry::calcBBox(Vector3& bmin, Vector3& bmax) const
    {
        const VertexBufferUP &vbuffer = getVertexBuffer();
        const s8 *bytes = NULL;
        bool ret = vbuffer.lock((const void**)bytes);
        LASSERT(ret);

        const f32 maxval = std::numeric_limits<f32>::max();
        const f32 minval = -std::numeric_limits<f32>::max();

        bmax.set(minval, minval, minval);
        bmin.set(maxval, maxval, maxval);

        for(u32 i=0; i<vbuffer.getVertexNum(); ++i){
            const Vector3 &v = reinterpret_cast<const Vector3&>(*bytes);
            
            for(u32 j=0; j<3; ++j){
                if(bmax[j] < v[j]){
                    bmax[j] = v[j];
                }
                if(bmin[j] > v[j]){
                    bmin[j] = v[j];
                }
            }

            bytes += vbuffer.getVertexSize();
        }

        vbuffer.unlock();
    }

    void Geometry::calcMedian(Vector3& median) const
    {
        const VertexBufferUP &vbuffer = getVertexBuffer();
        const s8 *bytes = NULL;
        bool ret = vbuffer.lock((const void**)bytes);
        LASSERT(ret);
        LASSERT(vbuffer.getVertexNum()>0);

        median.set(0.0f, 0.0f, 0.0f);
        f32 invSize = 1.0f / vbuffer.getVertexNum();

        for(u32 i=0; i<vbuffer.getVertexNum(); ++i){
            const Vector3 &v = reinterpret_cast<const Vector3&>(*bytes);

            median += v;

            bytes += vbuffer.getVertexSize();
        }

        median *= invSize;
        vbuffer.unlock();
    }


    // Shapeのメソッド実装
    //---------------------------------------------------------------------------
    u32 Geometry::calcNumElements() const
    {
        return getIndexBuffer().getNumFaces();
    }

    bool Geometry::getElements(Element* elements, u32 bufferCount)
    {
        LASSERT(calcNumElements() <= bufferCount);
        for(u32 i=0; i<calcNumElements(); ++i){
            elements[i].set(this, i);
        }
        return true;
    }

    void Geometry::calcBBox(u32 index, Vector3& bmin, Vector3& bmax) const
    {
        LASSERT(0<=index && index<getIndexBuffer().getNumFaces());

        const f32 maxval = std::numeric_limits<f32>::max();
        const f32 minval = -std::numeric_limits<f32>::max();

        bmax.set(minval, minval, minval);
        bmin.set(maxval, maxval, maxval);

        const IndexBufferUP &ibuffer = getIndexBuffer();
        const VertexBufferUP &vbuffer = getVertexBuffer();

        const lgraphics::Face *faces = NULL;
        bool ret = ibuffer.lock((const void**)&faces);
        LASSERT(ret);

        const s8 *bytes = NULL;
        ret = vbuffer.lock((const void**)&bytes);
        LASSERT(ret);

        for(u32 i=0; i<3; ++i){
            const s8 *pv = bytes + faces[index].indices_[i] * vbuffer.getVertexSize();

            const Vector3 &v = reinterpret_cast<const Vector3&>(*pv);

            for(u32 j=0; j<3; ++j){
                if(bmax[j] < v[j]){
                    bmax[j] = v[j];
                }
                if(bmin[j] > v[j]){
                    bmin[j] = v[j];
                }
            }
        }

        ibuffer.unlock();
        vbuffer.unlock();
    }

    void Geometry::calcMedian(u32 index, Vector3& median) const
    {
        LASSERT(0<=index && index<getIndexBuffer().getNumFaces());

        median.set(0.0f, 0.0f, 0.0f);
        f32 invSize = 1.0f / 3.0f;

        const IndexBufferUP &ibuffer = getIndexBuffer();
        const VertexBufferUP &vbuffer = getVertexBuffer();

        const lgraphics::Face *faces = NULL;
        bool ret = ibuffer.lock((const void**)&faces);
        LASSERT(ret);

        const s8 *bytes = NULL;
        ret = vbuffer.lock((const void**)&bytes);
        LASSERT(ret);

        for(u32 i=0; i<3; ++i){
            const s8 *pv = bytes + faces[index].indices_[i] * vbuffer.getVertexSize();

            const Vector3 &v = reinterpret_cast<const Vector3&>(*pv);

            median += v;
        }
        median *= invSize;

        ibuffer.unlock();
        vbuffer.unlock();
    }

    bool Geometry::hit(HitRecord& hitRecord, u32 index, const lmath::Ray& ray) const
    {
#if 1
        LASSERT(0<=index && index<getIndexBuffer().getNumFaces());

        const IndexBufferUP &ibuffer = getIndexBuffer();
        const VertexBufferUP &vbuffer = getVertexBuffer();

        const lgraphics::Face *faces = NULL;
        bool ret = ibuffer.lock((const void**)&faces);
        LASSERT(ret);

        const s8 *bytes = NULL;
        ret = vbuffer.lock((const void**)&bytes);
        LASSERT(ret);

        f32 t = std::numeric_limits<f32>::max();
        f32 u = 0.0f; 
        f32 v = 0.0f;

        bool bHit = false;

        const s8 *pv0 = bytes + faces[index].indices_[0] * vbuffer.getVertexSize();
        const s8 *pv1 = bytes + faces[index].indices_[1] * vbuffer.getVertexSize();
        const s8 *pv2 = bytes + faces[index].indices_[2] * vbuffer.getVertexSize();

        const Vector3 &v0 = reinterpret_cast<const Vector3&>(*pv0);
        const Vector3 &v1 = reinterpret_cast<const Vector3&>(*pv1);
        const Vector3 &v2 = reinterpret_cast<const Vector3&>(*pv2);

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

#else
        const IndexBufferUP &ibuffer = getIndexBuffer();
        const VertexBufferUP &vbuffer = getVertexBuffer();

        const lgraphics::Face *faces = NULL;
        bool ret = ibuffer.lock((const void**)&faces);
        LASSERT(ret);

        const s8 *bytes = NULL;
        ret = vbuffer.lock((const void**)&bytes);
        LASSERT(ret);

        f32 t = std::numeric_limits<f32>::max();
        f32 u = 0.0f; 
        f32 v = 0.0f;

        bool bHit = false;
        u32 hitIndex = -1;
        for(u32 i=0; i<ibuffer.getNumFaces(); ++i){
            const s8 *pv0 = bytes + faces[i].indices_[0] * vbuffer.getVertexSize();
            const s8 *pv1 = bytes + faces[i].indices_[1] * vbuffer.getVertexSize();
            const s8 *pv2 = bytes + faces[i].indices_[2] * vbuffer.getVertexSize();

            const Vector3 &v0 = reinterpret_cast<const Vector3&>(*pv0);
            const Vector3 &v1 = reinterpret_cast<const Vector3&>(*pv1);
            const Vector3 &v2 = reinterpret_cast<const Vector3&>(*pv2);

            f32 tmpT = t;
            f32 tmpU = 0.0f;
            f32 tmpV = 0.0f;
            if(lmath::testRayTriangle(tmpT, tmpU, tmpV, ray, v0, v1, v2)){
                if(tmpT<t){
                    bHit = true;
                    t = tmpT;
                    u = tmpU;
                    v = tmpV;
                    hitIndex = i;
                }
            }
        }

        if(bHit){
            hitRecord.t_ = t;
            hitRecord.u_ = u;
            hitRecord.v_ = v;
            hitRecord.index_ = hitIndex;
            hitRecord.shape_ = this;

            Vector3 &hitPos = hitRecord.position_;
            hitPos = ray.direction_;
            hitPos *= hitRecord.t_;
            hitPos += ray.origin_;
        }

        ibuffer.unlock();
        vbuffer.unlock();
        return bHit;
#endif
    }

    void Geometry::calcShadingGeometry(ShadingGeometry& shadingGeom, const HitRecord& hitRecord) const
    {
        shadingGeom.position_ = hitRecord.position_;


        // バッファロック
        const IndexBufferUP &ibuffer = getIndexBuffer();
        const VertexBufferUP &vbuffer = getVertexBuffer();

        const lgraphics::Face *faces = NULL;
        bool ret = ibuffer.lock((const void**)&faces);
        LASSERT(ret);

        const s8 *bytes = NULL;
        ret = vbuffer.lock((const void**)&bytes);
        LASSERT(ret);

        s32 index = hitRecord.index_;
        const s8 *pn0 = bytes + faces[index].indices_[0] * vbuffer.getVertexSize() + sizeof(Vector3);
        const s8 *pn1 = bytes + faces[index].indices_[1] * vbuffer.getVertexSize() + sizeof(Vector3);
        const s8 *pn2 = bytes + faces[index].indices_[2] * vbuffer.getVertexSize() + sizeof(Vector3);

        const Vector3 &n0 = reinterpret_cast<const Vector3&>(*pn0);
        const Vector3 &n1 = reinterpret_cast<const Vector3&>(*pn1);
        const Vector3 &n2 = reinterpret_cast<const Vector3&>(*pn2);

        //法線計算
        f32 w = static_cast<f32>(1.0 - hitRecord.u_ - hitRecord.v_);
        shadingGeom.normal_ = n0 * w;
        shadingGeom.normal_ += n1 * hitRecord.u_;
        shadingGeom.normal_ += n2 * hitRecord.v_;
        shadingGeom.normal_.normalize();
        

        shadingGeom.color_.set(1.0f, 1.0f, 1.0f, 1.0f);

#if 1
        const s8 *p0 = bytes + faces[index].indices_[0] * vbuffer.getVertexSize();
        const s8 *p1 = bytes + faces[index].indices_[1] * vbuffer.getVertexSize();
        const s8 *p2 = bytes + faces[index].indices_[2] * vbuffer.getVertexSize();

        const Vector3 &v0 = reinterpret_cast<const Vector3&>(*p0);
        const Vector3 &v1 = reinterpret_cast<const Vector3&>(*p1);
        const Vector3 &v2 = reinterpret_cast<const Vector3&>(*p2);
        Vector3 pos = v0 * w;
        pos += v1 * hitRecord.u_;
        pos += v2 * hitRecord.v_;
#endif
    }
}
