/**
@file QBVH.cpp
@author t-sakai
@date 2013/05/09 create
*/
#include "QBVH.h"
#include <lcore/Sort.h>
#include <lmath/geometry/Ray.h>
#include <lmath/geometry/RayTest.h>

#include "Face.h"
#include "HitRecord.h"

namespace lrender
{

namespace
{
    static const s32 OrderTable[] =
    {
        0x44444,0x44444,0x44444,0x44444,0x44444,0x44444,0x44444,0x44444,
        0x44440,0x44440,0x44440,0x44440,0x44440,0x44440,0x44440,0x44440,
        0x44441,0x44441,0x44441,0x44441,0x44441,0x44441,0x44441,0x44441,
        0x44401,0x44401,0x44410,0x44410,0x44401,0x44401,0x44410,0x44410,
        0x44442,0x44442,0x44442,0x44442,0x44442,0x44442,0x44442,0x44442,
        0x44402,0x44402,0x44402,0x44402,0x44420,0x44420,0x44420,0x44420,
        0x44412,0x44412,0x44412,0x44412,0x44421,0x44421,0x44421,0x44421,
        0x44012,0x44012,0x44102,0x44102,0x44201,0x44201,0x44210,0x44210,
        0x44443,0x44443,0x44443,0x44443,0x44443,0x44443,0x44443,0x44443,
        0x44403,0x44403,0x44403,0x44403,0x44430,0x44430,0x44430,0x44430,
        0x44413,0x44413,0x44413,0x44413,0x44431,0x44431,0x44431,0x44431,
        0x44013,0x44013,0x44103,0x44103,0x44301,0x44301,0x44310,0x44310,
        0x44423,0x44432,0x44423,0x44432,0x44423,0x44432,0x44423,0x44432,
        0x44023,0x44032,0x44023,0x44032,0x44230,0x44320,0x44230,0x44320,
        0x44123,0x44132,0x44123,0x44132,0x44231,0x44321,0x44231,0x44321,
        0x40123,0x40132,0x41023,0x41032,0x42301,0x43201,0x42310,0x43210,
    };

    s32 calcAxis(const lmath::Vector3& bmin, const lmath::Vector3& bmax)
    {
        lmath::Vector3 s = bmax;
        s -= bmin;
        s32 axis = 0;
        if(s[1]>s[axis]){
            axis = 1;
        }
        if(s[2]>s[axis]){
            axis = 2;
        }
        return axis;
    }

    //void getBBox(lmath::Vector3& bmin, lmath::Vector3& bmax, const Face* f0, const Face* f1)
    //{
    //    f0->calcBBox(bmin, bmax);
    //    
    //    lmath::Vector3 bmin1, bmax1;
    //    f1->calcBBox(bmin1, bmax1);
    //    for(s32 i=0; i<3; ++i){
    //        bmin[i] = lcore::minimum(bmin[i], bmin1[i]);
    //        bmax[i] = lcore::maximum(bmax[i], bmax1[i]);
    //    }
    //}

    void getBBox(lmath::Vector3& bmin, lmath::Vector3& bmax, const Face** faces, u32 numFaces)
    {
        bmin.set(lcore::numeric_limits<f32>::maximum(), lcore::numeric_limits<f32>::maximum(), lcore::numeric_limits<f32>::maximum());
        bmax.set(lcore::numeric_limits<f32>::minimum(), lcore::numeric_limits<f32>::minimum(), lcore::numeric_limits<f32>::minimum());

        lmath::Vector3 tmin, tmax;
        for(u32 i=0; i<numFaces; ++i){
            faces[i]->calcBBox(tmin, tmax);

            for(s32 j=0; j<3; ++j){
                bmin[j] = lcore::minimum(tmin[j], bmin[j]);
                bmax[j] = lcore::maximum(tmax[j], bmax[j]);
            }
        }
    }

    struct FaceSortFunc
    {
        FaceSortFunc(s32 axis)
            :axis_(axis)
        {}

        bool operator()(const Face* f0, const Face* f1) const
        {
            f32 m0 = (*f0)[0][axis_] + (*f0)[1][axis_] + (*f0)[2][axis_];
            f32 m1 = (*f1)[0][axis_] + (*f1)[1][axis_] + (*f1)[2][axis_];
            return m0 < m1;
        }

        s32 axis_;
    };

    void sort(u32 numFaces, const Face** faces, s32 axis)
    {
        FaceSortFunc func(axis);
        lcore::heapsort(numFaces, faces, func);
    }

    f32 calcTrisMedian(const Face* face, s32 axis)
    {
        return (*face)[0][axis] + (*face)[1][axis] + (*face)[2][axis];
    }


    //-----------------------------------------------------------
    // ü•ª‚ÆAABB‚ÌŒð·”»’è
    s32 testRayAABB(
        lmath::lm128 tmin,
        lmath::lm128 tmax,
        lmath::lm128 origin[3],
        lmath::lm128 invDir[3],
        const s32 sign[3],
        const lmath::lm128 bbox[2][3])
    {
        for(s32 i=0; i<3; ++i){
            tmin = _mm_max_ps(
                tmin,
                _mm_mul_ps(_mm_sub_ps(bbox[sign[i]][i], origin[i]), invDir[i]));

            tmax = _mm_min_ps(
                tmax,
                _mm_mul_ps(_mm_sub_ps(bbox[1-sign[i]][i], origin[i]), invDir[i]));
        }

        return _mm_movemask_ps(_mm_cmpge_ps(tmax, tmin));
    }

    //AABB‚ÌŒð·”»’è
    s32 testAABB(const lmath::lm128 bbox0[2][3], const lmath::lm128 bbox1[2][3]){
        u32 mask = 0xFFFFFFFFU;
        f32 fmask = *((f32*)&mask);

        lmath::lm128 t = _mm_set1_ps(fmask);
        for(s32 i=0; i<3; ++i){
            t = _mm_and_ps(t, _mm_cmple_ps(bbox0[0][i], bbox1[1][i]));
            t = _mm_and_ps(t, _mm_cmple_ps(bbox1[0][i], bbox0[1][i]));
        }
        return _mm_movemask_ps(t);
    }

    //
    s32 testSphereAABB(const lmath::lm128 position[3], const lmath::lm128 radius, const lmath::lm128 bbox[2][3]){
        u32 mask = 0xFFFFFFFFU;
        f32 fmask = *((f32*)&mask);

        lmath::lm128 t = _mm_set1_ps(fmask);
        //for(s32 i=0; i<3; ++i){
        //    t = _mm_and_ps(t, _mm_cmple_ps(bbox0[0][i], bbox1[1][i]));
        //    t = _mm_and_ps(t, _mm_cmple_ps(bbox1[0][i], bbox0[1][i]));
        //}
        return _mm_movemask_ps(t);
    }
}

    const f32 QBVH::BoundingExpantion = F32_EPSILON*2.0f;

    bool QBVH::test(HitRecord& hitRecord, const lmath::Ray& ray) const
    {
        hitRecord.face_ = NULL;
        //numTestFaces_ = 0;

        f32 tmin = 0.0f;
        f32 tmax = ray.t_;
        if(!lmath::testRayAABB(tmin, tmax, ray, bmin_, bmax_)){
            return false;
        }

        tmin = 0.0f;
        tmax = ray.t_;

        if(nodes_.size()<=0){
            bool ret = false;
            f32 u, v;
            f32 t;
            for(u32 i=0; i<numFaces_; ++i){
                const Face* face = faces_[i];
                if(lmath::testRayTriangle(t, u, v, ray, (*face)[0], (*face)[1], (*face)[2])){
                    if(0.0f<=t && t<=tmax){
                        tmax = t;
                        hitRecord.t_ = t;
                        hitRecord.u_ = u;
                        hitRecord.v_ = v;
                        hitRecord.face_ = face;
                        ret = true;
                    }
                }
            }
            //numTestFaces_ += numFaces_;
            return ret;
        }

        s32 raySign[3];
        raySign[0] = (0.0f<=ray.direction_[0])? 0 : 1;
        raySign[1] = (0.0f<=ray.direction_[1])? 0 : 1;
        raySign[2] = (0.0f<=ray.direction_[2])? 0 : 1;

        return innerTest(hitRecord, ray, tmin, tmax, raySign);
    }

    bool QBVH::test(HitRecord& hitRecord, const lmath::Vector3& bmin, const lmath::Vector3& bmax)
    {
        hitRecord.face_ = NULL;
        //numTestFaces_ = 0;

        for(s32 i=0; i<3; ++i){
            if(bmax[i] < bmin_[i] || bmax_[i] < bmin[i]){
                return false;
            }
        }
        if(nodes_.size()<=0){
            return false;
        }

        lmath::lm128 bbox[2][3];
        
        bbox[0][0] = _mm_load1_ps(&bmin.x_);
        bbox[0][1] = _mm_load1_ps(&bmin.y_);
        bbox[0][2] = _mm_load1_ps(&bmin.z_);

        bbox[1][0] = _mm_load1_ps(&bmax.x_);
        bbox[1][1] = _mm_load1_ps(&bmax.y_);
        bbox[1][2] = _mm_load1_ps(&bmax.z_);

        innerTest(hitRecord, bbox);
        return true;
    }

    bool QBVH::test(HitRecord& hitRecord, const lmath::Sphere& sphere)
    {
        return false;
    }

    bool QBVH::innerTest(HitRecord& hitRecord, const lmath::Ray& ray, f32 tmin, f32 tmax, s32 raySign[3]) const
    {
        lmath::lm128 origin[3];
        lmath::lm128 invDir[3];
        lmath::lm128 tminSSE;
        lmath::lm128 tmaxSSE;
        origin[0] = _mm_set1_ps(ray.origin_.x_);
        origin[1] = _mm_set1_ps(ray.origin_.y_);
        origin[2] = _mm_set1_ps(ray.origin_.z_);

        invDir[0] = _mm_set1_ps(ray.invDirection_.x_);
        invDir[1] = _mm_set1_ps(ray.invDirection_.y_);
        invDir[2] = _mm_set1_ps(ray.invDirection_.z_);

        tminSSE = _mm_set1_ps(tmin);
        tmaxSSE = _mm_set1_ps(tmax);

        bool ret = false;
        s32 stack = 0;
        u32 nodeStack[TestStackSize];
        nodeStack[stack] = 0;
        while(stack>=0){
            u32 index = nodeStack[stack];
            --stack;
            if(Node::isLeaf(index)){
                if(Node::isEmpty(index)){
                    continue;
                }

                u32 faceIndex = Node::getFaceIndex(index);
                u32 numFaces = Node::getFaceNum(index);
                LASSERT(faceIndex<numFaces_);

                f32 u, v;
                for(u32 i=faceIndex; i<faceIndex+numFaces; ++i){
                    const Face* face = faces_[i];
                    f32 t;
                    if(lmath::testRayTriangle(t, u, v, ray, (*face)[0], (*face)[1], (*face)[2])){
                        if(0.0f<=t && t<=tmax){
                            ret = true;
                            hitRecord.t_ = t;
                            hitRecord.u_ = u;
                            hitRecord.v_ = v;
                            hitRecord.face_ = face;
                            tmax = t;
                            tmaxSSE = _mm_set1_ps(tmax);
                        }
                    }
                }
                //numTestFaces_ += numFaces;

            }else{
                const Node& node = nodes_[index];
                s32 hit = testRayAABB(tminSSE, tmaxSSE, origin, invDir, raySign, node.bbox_);

                if(hit){
                    s32 nodeIndex = (raySign[node.axis0_] << 2) | (raySign[node.axis1_] << 1) |(raySign[node.axis2_] << 0);
                    s32 order = OrderTable[hit*8 + nodeIndex];

                    while(!(order&0x04)){
                        ++stack;
                        nodeStack[stack] = node.children_[order&0x03];
                        order >>= 4;
                    }
                }
            }
        }

        return ret;
    }

    bool QBVH::innerTest(HitRecord& hitRecord, lmath::lm128 bbox[2][3])
    {
        bool ret = false;
        s32 stack = 0;
        u32 nodeStack[TestStackSize];
        nodeStack[stack] = 0;
        while(stack>=0){
            u32 index = nodeStack[stack];
            --stack;
            if(Node::isLeaf(index)){
                if(Node::isEmpty(index)){
                    continue;
                }

                u32 faceIndex = Node::getFaceIndex(index);
                u32 numFaces = Node::getFaceNum(index);

                f32 u, v;
                for(u32 i=faceIndex; i<faceIndex+numFaces; ++i){
                }
                //numTestFaces_ += numFaces;

            }else{
                const Node& node = nodes_[index];
                s32 hit = testAABB(bbox, node.bbox_);

                for(s32 i=0; i<3; ++i){
                    if((hit>>i)&0x01U){
                        ++stack;
                        nodeStack[stack] = node.children_[i];
                    }
                }
            }
        }

        return ret;
    }

    void QBVH::construct(u32 numFaces, const Face** faces, const lmath::Vector3& bmin, const lmath::Vector3& bmax)
    {
        bmin_ = bmin;
        bmax_ = bmax;
        numFaces_ = numFaces;
        faces_ = faces;

        f32 depth = lmath::log(numFaces/(MinLeafFaces) + 0.0001f)/lmath::log(4.0f);
        u32 numNodes = lmath::pow(4.0f, depth);
        nodes_.clear();
        nodes_.reserve(numNodes);
        recursiveConstruct(0, numFaces, bmin, bmax);

    }

    u32 QBVH::recursiveConstruct(u32 begin, u32 numFaces, const lmath::Vector3& bmin, const lmath::Vector3& bmax)
    {
        if(0 == numFaces){
            return Node::EmptyMask;
        }

        if(numFaces<=MinLeafFaces){
            return Node::getLeaf(begin, numFaces);

        }
        u32 end = begin + numFaces - 1;
        s32 axis0;
        s32 axis1;
        s32 axis2;
        u32 mid0;
        u32 mid1;
        u32 mid2;
        lmath::Vector3 cmin = bmin;
        lmath::Vector3 cmax = bmax;

        {
            axis0 = calcAxis(cmin, cmax);
            mid0 = begin + (numFaces>>1);
            sort(numFaces, faces_+begin, axis0);
        }

        {
            u32 num = numFaces>>1;
            getBBox(cmin, cmax, faces_+begin, num);
            axis1 = calcAxis(cmin, cmax);
            mid1 = begin + (num>>1);

            sort(num, faces_+begin, axis1);
        }

        {
            u32 num = numFaces - (numFaces>>1);
            getBBox(cmin, cmax, faces_+mid0, num);
            axis2 = calcAxis(cmin,cmax);
            mid2 = mid0 + (num>>1);
            sort(num, faces_+mid0, axis2);
        }

        u32 nindex = nodes_.size();
        u32 ranges[4][2];

        ranges[0][0] = begin;
        ranges[0][1] = mid1;
        ranges[1][0] = mid1;
        ranges[1][1] = mid0;
        ranges[2][0] = mid0;
        ranges[2][1] = mid2;
        ranges[3][0] = mid2;
        ranges[3][1] = begin + numFaces;

        lmath::Vector3 minmax[4][2];
        LIME_ALIGN16 f32 bbox[2][3][4];
        {
            for(s32 i=0; i<4; ++i){
                u32 num = ranges[i][1] - ranges[i][0];
                getBBox(minmax[i][0], minmax[i][1], faces_+ranges[i][0], num);
            }

            for(s32 i=0; i<3; ++i){
                for(s32 j=0; j<4; ++j){
                    bbox[0][i][j] = minmax[j][0][i] - BoundingExpantion;
                    bbox[1][i][j] = minmax[j][1][i] + BoundingExpantion;
                }
            }

            nodes_.push_back(Node());
            Node& node = nodes_[nindex];
            node.axis0_ = axis0;
            node.axis1_ = axis1;
            node.axis2_ = axis2;
            for(s32 i=0; i<2; ++i){
                for(s32 j=0; j<3; ++j){
                    node.bbox_[i][j] = _mm_loadu_ps(bbox[i][j]);
                }
            }
        }

        u32 ret;
        for(s32 i=0; i<4; ++i){
            u32 num = ranges[i][1] - ranges[i][0];
            ret = recursiveConstruct(ranges[i][0], num, minmax[i][0], minmax[i][1]);
            nodes_[nindex].children_[i] = ret;
        }
        return nindex;
    }
}
