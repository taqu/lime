/**
@file QBVH.cpp
@author t-sakai
@date 2013/05/09 create
*/
#include "QBVH.h"

#include <lcore/liostream.h>

#include <lcore/Sort.h>

#include <lmath/geometry/Ray.h>
#include <lmath/geometry/RayTest.h>

namespace lmath
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


    //-----------------------------------------------------------
    // 線分とAABBの交差判定
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

    //AABBの交差判定
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

        lmath::lm128 tbbox[2][3];
        for(s32 i=0; i<3; ++i){
            tbbox[0][i] = _mm_sub_ps(bbox[0][i], radius);
            tbbox[1][i] = _mm_add_ps(bbox[1][i], radius);
        }
        lmath::lm128 t = _mm_set1_ps(fmask);
        for(s32 i=0; i<3; ++i){
            t = _mm_and_ps(t, _mm_cmple_ps(position[i], tbbox[1][i]));
            t = _mm_and_ps(t, _mm_cmple_ps(tbbox[0][i], position[i]));
        }
        return _mm_movemask_ps(t);
    }
}

    const f32 QBVHConstructor::BoundingExpansion = F32_EPSILON*2.0f;

    struct QBVHConstructor::FaceSortFunc
    {
        FaceSortFunc(s32 axis, QBVHConstructor* qbvh)
            :axis_(axis)
            ,qbvh_(qbvh)
        {}

        bool operator()(const QBVHConstructor::Face& f0, const QBVHConstructor::Face& f1) const
        {
            const lmath::Vector3& v00 = qbvh_->vertices_[f0.v0_].position_;
            const lmath::Vector3& v01 = qbvh_->vertices_[f0.v1_].position_;
            const lmath::Vector3& v02 = qbvh_->vertices_[f0.v2_].position_;

            const lmath::Vector3& v10 = qbvh_->vertices_[f1.v0_].position_;
            const lmath::Vector3& v11 = qbvh_->vertices_[f1.v1_].position_;
            const lmath::Vector3& v12 = qbvh_->vertices_[f1.v2_].position_;

            f32 m0 = v00[axis_] + v01[axis_] + v02[axis_];
            f32 m1 = v10[axis_] + v11[axis_] + v12[axis_];
            return m0 < m1;
        }

        s32 axis_;
        QBVHConstructor* qbvh_;
    };

    QBVHConstructor::QBVHConstructor()
        :boundingExpansion_(BoundingExpansion)
        ,numVertices_(0)
        ,vertices_(NULL)
        ,numFaces_(0)
        ,faces_(NULL)
    {
    }

    QBVHConstructor::~QBVHConstructor()
    {
        LIME_DELETE_ARRAY(vertices_);
        LIME_DELETE_ARRAY(faces_);
    }

    void QBVHConstructor::construct(u32 numVertices, Vertex* vertices, u32 numFaces, Face* faces, const lmath::Vector3& bmin, const lmath::Vector3& bmax)
    {
        LASSERT(NULL != vertices);
        LASSERT(NULL != faces);

        LIME_DELETE_ARRAY(vertices_);
        LIME_DELETE_ARRAY(faces_);

        bmin_ = bmin;
        bmax_ = bmax;
        numVertices_ = numVertices;
        vertices_ = vertices;
        numFaces_ = numFaces;
        faces_ = faces;

        f32 depth = lmath::log(numFaces/(MinLeafFaces) + 0.0001f)/lmath::log(4.0f);
        u32 numNodes = static_cast<u32>(lmath::pow(4.0f, depth));
        nodes_.clear();
        nodes_.reserve(numNodes);
        recursiveConstruct(0, numFaces, bmin, bmax);
    }

    void QBVHConstructor::save(const Char* filepath)
    {
        LASSERT(NULL != filepath);

        lcore::ofstream out(filepath, lcore::ios::binary);

        if(!out.is_open()){
            return;
        }

        //各個数と境界ボックス
        u32 numNodes = nodes_.size();

        lcore::io::write(out, numNodes);
        lcore::io::write(out, numVertices_);
        lcore::io::write(out, numFaces_);

        lcore::io::write(out, bmin_.x_);
        lcore::io::write(out, bmin_.y_);
        lcore::io::write(out, bmin_.z_);

        lcore::io::write(out, bmax_.x_);
        lcore::io::write(out, bmax_.y_);
        lcore::io::write(out, bmax_.z_);

        //データ保存
        LIME_ALIGN16 f32 tmp[4];
        for(u32 i=0; i<nodes_.size(); ++i){
            //SSEレジスタ保存
            //_mm_store_ps(tmp, nodes_[i].bbox_[0][0]); lcore::io::write(out, tmp, sizeof(f32)*4);
            //_mm_store_ps(tmp, nodes_[i].bbox_[0][1]); lcore::io::write(out, tmp, sizeof(f32)*4);
            //_mm_store_ps(tmp, nodes_[i].bbox_[0][2]); lcore::io::write(out, tmp, sizeof(f32)*4);

            //_mm_store_ps(tmp, nodes_[i].bbox_[1][0]); lcore::io::write(out, tmp, sizeof(f32)*4);
            //_mm_store_ps(tmp, nodes_[i].bbox_[1][1]); lcore::io::write(out, tmp, sizeof(f32)*4);
            //_mm_store_ps(tmp, nodes_[i].bbox_[1][2]); lcore::io::write(out, tmp, sizeof(f32)*4);

            lcore::memcpy(tmp, nodes_[i].bbox_[0][0], sizeof(f32)*4); lcore::io::write(out, tmp, sizeof(f32)*4);
            lcore::memcpy(tmp, nodes_[i].bbox_[0][1], sizeof(f32)*4); lcore::io::write(out, tmp, sizeof(f32)*4);
            lcore::memcpy(tmp, nodes_[i].bbox_[0][2], sizeof(f32)*4); lcore::io::write(out, tmp, sizeof(f32)*4);

            lcore::memcpy(tmp, nodes_[i].bbox_[1][0], sizeof(f32)*4); lcore::io::write(out, tmp, sizeof(f32)*4);
            lcore::memcpy(tmp, nodes_[i].bbox_[1][1], sizeof(f32)*4); lcore::io::write(out, tmp, sizeof(f32)*4);
            lcore::memcpy(tmp, nodes_[i].bbox_[1][2], sizeof(f32)*4); lcore::io::write(out, tmp, sizeof(f32)*4);

            lcore::io::write(out, nodes_[i].children_, sizeof(u32)*4);
            lcore::io::write(out, nodes_[i].axis0_);
            lcore::io::write(out, nodes_[i].axis1_);
            lcore::io::write(out, nodes_[i].axis2_);
            lcore::io::write(out, nodes_[i].reserved_);
        }

        lcore::io::write(out, vertices_, sizeof(Vertex)*numVertices_);
        lcore::io::write(out, faces_, sizeof(Face)*numFaces_);

        out.close();
    }

    u32 QBVHConstructor::recursiveConstruct(u32 begin, u32 numFaces, const lmath::Vector3& bmin, const lmath::Vector3& bmax)
    {
        if(0 == numFaces){
            return Node::EmptyMask;
        }

        if(numFaces<=MinLeafFaces){
            return Node::getLeaf(begin, numFaces);

        }
        //u32 end = begin + numFaces - 1;
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
                    bbox[0][i][j] = minmax[j][0][i] - boundingExpansion_;
                    bbox[1][i][j] = minmax[j][1][i] + boundingExpansion_;
                }
            }

            nodes_.push_back(Node());
            Node& node = nodes_[nindex];
            node.axis0_ = axis0;
            node.axis1_ = axis1;
            node.axis2_ = axis2;
            for(s32 i=0; i<2; ++i){
                for(s32 j=0; j<3; ++j){
                    //node.bbox_[i][j] = _mm_loadu_ps(bbox[i][j]);
                    lcore::memcpy(node.bbox_[i][j], bbox[i][j], sizeof(f32)*4);
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


    void QBVHConstructor::sort(u32 numFaces, Face* faces, s32 axis)
    {
        FaceSortFunc func(axis, this);
        lcore::heapsort(numFaces, faces, func);
    }

    void QBVHConstructor::calcBBox(lmath::Vector3& bmin, lmath::Vector3& bmax, const Face& face)
    {
        const lmath::Vector3& v0 = vertices_[face.v0_].position_;
        const lmath::Vector3& v1 = vertices_[face.v1_].position_;
        const lmath::Vector3& v2 = vertices_[face.v2_].position_;

        bmin.x_ = lcore::minimum(v0.x_, v1.x_);
        bmin.y_ = lcore::minimum(v0.y_, v1.y_);
        bmin.z_ = lcore::minimum(v0.z_, v1.z_);

        bmax.x_ = lcore::maximum(v0.x_, v1.x_);
        bmax.y_ = lcore::maximum(v0.y_, v1.y_);
        bmax.z_ = lcore::maximum(v0.z_, v1.z_);

        bmin.x_ = lcore::minimum(bmin.x_, v2.x_);
        bmin.y_ = lcore::minimum(bmin.y_, v2.y_);
        bmin.z_ = lcore::minimum(bmin.z_, v2.z_);

        bmax.x_ = lcore::maximum(bmax.x_, v2.x_);
        bmax.y_ = lcore::maximum(bmax.y_, v2.y_);
        bmax.z_ = lcore::maximum(bmax.z_, v2.z_);
    }

    void QBVHConstructor::getBBox(lmath::Vector3& bmin, lmath::Vector3& bmax, const Face* faces, u32 numFaces)
    {
        bmin.set(lcore::numeric_limits<f32>::maximum(), lcore::numeric_limits<f32>::maximum(), lcore::numeric_limits<f32>::maximum());
        bmax.set(lcore::numeric_limits<f32>::minimum(), lcore::numeric_limits<f32>::minimum(), lcore::numeric_limits<f32>::minimum());

        lmath::Vector3 tmin, tmax;
        for(u32 i=0; i<numFaces; ++i){
            calcBBox(tmin, tmax, faces[i]);

            for(s32 j=0; j<3; ++j){
                bmin[j] = lcore::minimum(tmin[j], bmin[j]);
                bmax[j] = lcore::maximum(tmax[j], bmax[j]);
            }
        }
    }


    QBVH::QBVH()
        :numNodes_(0)
        ,nodes_(NULL)
        ,numVertices_(0)
        ,vertices_(NULL)
        ,numFaces_(0)
        ,faces_(NULL)
    {
        position_.zero();
    }

    QBVH::~QBVH()
    {
        LIME_FREE(faces_);
        LIME_FREE(vertices_);
        LIME_FREE(nodes_);
    }

    void QBVH::translateToLocal(lmath::Ray& ray)
    {
        ray.origin_ -= position_;
    }

    bool QBVH::load(lcore::istream& in, s32 offset)
    {
        in.seekg(offset, lcore::ios::beg);

        LIME_FREE(faces_);
        LIME_FREE(vertices_);
        LIME_FREE(nodes_);
        nodes_ = NULL;

        //各個数と境界ボックス
        lcore::io::read(in, numNodes_);
        lcore::io::read(in, numVertices_);
        lcore::io::read(in, numFaces_);

        lcore::io::read(in, bmin_.x_);
        lcore::io::read(in, bmin_.y_);
        lcore::io::read(in, bmin_.z_);

        lcore::io::read(in, bmax_.x_);
        lcore::io::read(in, bmax_.y_);
        lcore::io::read(in, bmax_.z_);

        //データロード

        nodes_ = (Node*)LIME_ALIGNED_MALLOC(sizeof(Node)*numNodes_, 16);
        LIME_ALIGN16 f32 bbox[2][3][4];
        for(u32 i=0; i<numNodes_; ++i){

            //SSEレジスタロード
            lcore::io::read(in, bbox[0][0], sizeof(f32)*4);
            lcore::io::read(in, bbox[0][1], sizeof(f32)*4);
            lcore::io::read(in, bbox[0][2], sizeof(f32)*4);

            lcore::io::read(in, bbox[1][0], sizeof(f32)*4);
            lcore::io::read(in, bbox[1][1], sizeof(f32)*4);
            lcore::io::read(in, bbox[1][2], sizeof(f32)*4);

            lcore::io::read(in, nodes_[i].children_, sizeof(u32)*4);
            lcore::io::read(in, nodes_[i].axis0_);
            lcore::io::read(in, nodes_[i].axis1_);
            lcore::io::read(in, nodes_[i].axis2_);
            lcore::io::read(in, nodes_[i].reserved_);

            for(u32 j=0;j<2; ++j){
                for(u32 k=0; k<3; ++k){
                    nodes_[i].bbox_[j][k] = _mm_load_ps(bbox[j][k]);
                }
            }
        }

        vertices_ = (Vertex*)LIME_MALLOC(sizeof(Vertex)*numVertices_);
        lcore::io::read(in, vertices_, sizeof(Vertex)*numVertices_);

        faces_ = (Face*)LIME_MALLOC(sizeof(Face)*numFaces_);
        lcore::io::read(in, faces_, sizeof(Face)*numFaces_);
        return true;
    }

    void QBVH::copyFrom(QBVHConstructor& constructor)
    {
        LIME_FREE(faces_);
        LIME_FREE(vertices_);
        LIME_FREE(nodes_);
        nodes_ = NULL;
        numNodes_ = constructor.nodes_.size();

        numVertices_ = constructor.numVertices_;
        numFaces_ = constructor.numFaces_;

        bmin_ = constructor.bmin_;
        bmax_ = constructor.bmax_;

        nodes_ = (Node*)LIME_ALIGNED_MALLOC(sizeof(Node)*numNodes_, 16);
        LIME_ALIGN16 f32 bbox[2][3][4];
        for(u32 i=0; i<numNodes_; ++i){

            //SSEレジスタロード
            lcore::memcpy(bbox[0][0], constructor.nodes_[i].bbox_[0][0], sizeof(f32)*4);
            lcore::memcpy(bbox[0][1], constructor.nodes_[i].bbox_[0][1], sizeof(f32)*4);
            lcore::memcpy(bbox[0][2], constructor.nodes_[i].bbox_[0][2], sizeof(f32)*4);

            lcore::memcpy(bbox[1][0], constructor.nodes_[i].bbox_[1][0], sizeof(f32)*4);
            lcore::memcpy(bbox[1][1], constructor.nodes_[i].bbox_[1][1], sizeof(f32)*4);
            lcore::memcpy(bbox[1][2], constructor.nodes_[i].bbox_[1][2], sizeof(f32)*4);

            lcore::memcpy(nodes_[i].children_, constructor.nodes_[i].children_, sizeof(u32)*4);

            nodes_[i].axis0_ = constructor.nodes_[i].axis0_;
            nodes_[i].axis1_ = constructor.nodes_[i].axis1_;
            nodes_[i].axis2_ = constructor.nodes_[i].axis2_;
            nodes_[i].reserved_ = constructor.nodes_[i].reserved_;

            for(u32 j=0;j<2; ++j){
                for(u32 k=0; k<3; ++k){
                    nodes_[i].bbox_[j][k] = _mm_load_ps(bbox[j][k]);
                }
            }
        }

        vertices_ = (Vertex*)LIME_MALLOC(sizeof(Vertex)*numVertices_);
        lcore::memcpy(vertices_, constructor.vertices_, sizeof(Vertex)*numVertices_);

        faces_ = (Face*)LIME_MALLOC(sizeof(Face)*numFaces_);
        lcore::memcpy(faces_, constructor.faces_, sizeof(Face)*numFaces_);
    }

    bool QBVH::test(HitRecord& hitRecord, const lmath::Ray& localRay)
    {
        hitRecord.face_ = NULL;
        numTestFaces_ = 0;

        f32 tmin;// = 0.0f;
        f32 tmax;// = localRay.t_;
        if(!lmath::testRayAABB(tmin, tmax, localRay, bmin_, bmax_)){
            return false;
        }

        tmin = 0.0f;
        tmax = localRay.t_;

        if(numNodes_<=0){
            bool ret = false;
            f32 u, v;
            f32 t;
            for(u32 i=0; i<numFaces_; ++i){
                const Face& face = faces_[i];
                const lmath::Vector3& p0 = vertices_[face.v0_].position_;
                const lmath::Vector3& p1 = vertices_[face.v1_].position_;
                const lmath::Vector3& p2 = vertices_[face.v2_].position_;

                if(lmath::testRayTriangle(t, u, v, localRay, p0, p1, p2)){
                    if(0.0f<=t && t<=tmax){
                        tmax = t;
                        hitRecord.t_ = t;
                        hitRecord.face_ = &face;
                        ret = true;
                    }
                }
            }
            numTestFaces_ += numFaces_;
            return ret;
        }

        raySign_[0] = (0.0f<=localRay.direction_[0])? 0 : 1;
        raySign_[1] = (0.0f<=localRay.direction_[1])? 0 : 1;
        raySign_[2] = (0.0f<=localRay.direction_[2])? 0 : 1;

        return innerTest(hitRecord, localRay, tmin, tmax);
    }

    bool QBVH::test(HitRecord& hitRecord, const lmath::Vector3& bmin, const lmath::Vector3& bmax)
    {
        hitRecord.face_ = NULL;
        numTestFaces_ = 0;

        for(s32 i=0; i<3; ++i){
            if(bmax[i] < bmin_[i] || bmax_[i] < bmin[i]){
                return false;
            }
        }
        if(numNodes_<=0){
            return false;
        }
        return innerTest(hitRecord, bmin, bmax);
    }

    //bool QBVH::test(HitRecord& hitRecord, const lmath::Sphere& sphere)
    //{
    //}

    bool QBVH::innerTest(HitRecord& hitRecord, const lmath::Ray& ray, f32 tmin, f32 tmax)
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

                f32 u, v;
                for(u32 i=faceIndex; i<faceIndex+numFaces; ++i){
                    const Face& face = faces_[i];
                    f32 t;
                    const lmath::Vector3& p0 = vertices_[face.v0_].position_;
                    const lmath::Vector3& p1 = vertices_[face.v1_].position_;
                    const lmath::Vector3& p2 = vertices_[face.v2_].position_;

                    if(lmath::testRayTriangle(t, u, v, ray, p0, p1, p2)){
                        if(0<=t && t<=tmax){
                            ret = true;
                            hitRecord.t_ = t;
                            hitRecord.face_ = &face;
                            tmax = t;
                            tmaxSSE = _mm_set1_ps(tmax);
                        }
                    }
                }
                numTestFaces_ += numFaces;

            }else{
                const Node& node = nodes_[index];
                s32 hit = testRayAABB(tminSSE, tmaxSSE, origin, invDir, raySign_, node.bbox_);

                if(hit){
                    s32 nodeIndex = (raySign_[node.axis0_] << 2) | (raySign_[node.axis1_] << 1) |(raySign_[node.axis2_] << 0);
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

    bool QBVH::innerTest(HitRecord& /*hitRecord*/, const lmath::Vector3& bmin, const lmath::Vector3& bmax)
    {
        lmath::lm128 bbox[2][3];
        
        bbox[0][0] = _mm_load1_ps(&bmin.x_);
        bbox[0][1] = _mm_load1_ps(&bmin.y_);
        bbox[0][2] = _mm_load1_ps(&bmin.z_);

        bbox[1][0] = _mm_load1_ps(&bmax.x_);
        bbox[1][1] = _mm_load1_ps(&bmax.y_);
        bbox[1][2] = _mm_load1_ps(&bmax.z_);


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

                //TODO:
                for(u32 i=faceIndex; i<faceIndex+numFaces; ++i){
                }
                numTestFaces_ += numFaces;

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

}
