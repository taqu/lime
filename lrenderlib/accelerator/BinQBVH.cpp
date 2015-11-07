/**
@file BinQBVH.cpp
@author t-sakai
@date 2015/09/09 create
*/
#include "BinQBVH.h"
#include "../core/AABB.h"
#include "../core/Ray.h"
#include "../core/Intersection.h"
#include "../core/Geometry.h"

namespace lrender
{
    const f32 BinQBVH::Epsilon = 1.0e-6f;

    //----------------------------------------------------------------
    void BinQBVH::Node::setBBox(const AABB bbox[4])
    {
        LIME_ALIGN16 f32 bb[2][3][4];

        for(s32 i=0; i<3; ++i){
            for(s32 j=0; j<4; ++j){
                bb[0][i][j] = bbox[j].bmin_[i] - Epsilon;
                bb[1][i][j] = bbox[j].bmax_[i] + Epsilon;
            }
        }

        for(s32 i=0; i<2; ++i){
            for(s32 j=0; j<3; ++j){
                _mm_store_ps((f32*)&bbox_[i][j], _mm_load_ps(bb[i][j]));
            }
        }
    }

    void BinQBVH::Node::clear()
    {
        __m128 flt_min = _mm_set1_ps(-FLT_MAX);
        __m128 flt_max = _mm_set1_ps(FLT_MAX);
        for(s32 i=0; i<3; ++i){
            _mm_store_ps((f32*)&bbox_[0][i], flt_max);
            _mm_store_ps((f32*)&bbox_[1][i], flt_min);
        }
        for(s32 i=0; i<4; ++i){
            children_[i] = EmptyMask;
        }
        axis0_ = axis1_ = axis2_ = 0;
    }

    //----------------------------------------------------------------
    BinQBVH::BinQBVH()
        :SAH_KI_(1.2f)
        ,SAH_KT_(1.0f)
        ,depth_(0)
#if LRENDER_BINQBVH_SSEPACK
        ,numPackes_(0)
        ,numPackesCapacity_(0)
        ,packes_(NULL)
#endif
        ,stack_(NULL)
    {
        nodes_.resize(1);
        nodes_[0].clear();
    }

    BinQBVH::~BinQBVH()
    {
#if LRENDER_BINQBVH_SSEPACK
        LIME_ALIGNED_FREE(packes_, PackAlign);
#endif

        LIME_FREE(stack_);
    }

    void BinQBVH::swap(BinQBVH& rhs)
    {
        lcore::swap(SAH_KI_, rhs.SAH_KI_);
        lcore::swap(SAH_KT_, rhs.SAH_KT_);
        lcore::swap(depth_, rhs.depth_);

        shapes_.swap(rhs.shapes_);
        nodes_.swap(rhs.nodes_);
        primitives_.swap(rhs.primitives_);
        primitiveCentroids_.swap(rhs.primitiveCentroids_);
        primitiveBBoxes_.swap(rhs.primitiveBBoxes_);

#if LRENDER_BINQBVH_SSEPACK
        lcore::swap(numPackes_, rhs.numPackes_);
        lcore::swap(numPackesCapacity_, rhs.numPackesCapacity_);
        lcore::swap(packes_, rhs.packes_);
#endif

        lcore::swap(stack_, rhs.stack_);
    }

    void BinQBVH::clearShapes()
    {
        shapes_.clear();
    }

    void BinQBVH::addShape(const Shape::pointer& shape)
    {
        if(shapes_.size()<=shape->getID()){
            shapes_.resize(shape->getID()+1);
        }
        shapes_[shape->getID()] = shape.get();
    }

    void BinQBVH::build()
    {
        s32 numPrimitives = triangulate();

        f32 depth = logf(static_cast<f32>(numPrimitives) / MinLeafPrimitives) / logf(4.0f);
        s32 numNodes = static_cast<s32>(powf(2.0f, depth) + 0.5f);
        nodes_.reserve(numNodes);
        nodes_.resize(1);
        nodes_[0].clear();


        primitiveCentroids_.resize(numPrimitives*3);
        primitiveBBoxes_.resize(numPrimitives);

        //各primitiveのcentroid, bboxを事前計算
        f32* centroidX = &primitiveCentroids_[0];
        f32* centroidY = centroidX + numPrimitives;
        f32* centroidZ = centroidY + numPrimitives;

        AABB bbox;
        bbox.setInvalid();
        for(s32 i=0; i<numPrimitives; ++i){
            primitives_[i].index_ = i;

            Vector3 centroid = getCentroid(primitives_[i]);
            centroidX[i] = centroid.x_;
            centroidY[i] = centroid.y_;
            centroidZ[i] = centroid.z_;
            primitiveBBoxes_[i] = getBBox(primitives_[i]);

            bbox.extend( primitiveBBoxes_[i] );
        }

#if LRENDER_BINQBVH_SSEPACK
        numPackes_ = 0;
        s32 numPackesCapacity = numPrimitives>>1;
        if(numPackesCapacity_<numPackesCapacity){
            numPackesCapacity_ = numPackesCapacity;
            LIME_ALIGNED_FREE(packes_, PackAlign);
            packes_ = (Primitive4Pack*)LIME_ALIGNED_MALLOC(sizeof(Primitive4Pack)*numPackesCapacity_, PackAlign);
        }
#endif

        s32 prevDepth = depth_;
        depth_ = 1;
        if(numPrimitives<=MinLeafPrimitives){
            AABB childBBox[4];
            childBBox[0] = bbox;
            childBBox[1].setInvalid();
            childBBox[2].setInvalid();
            childBBox[3].setInvalid();

            nodes_[0].setBBox(childBBox);
            nodes_[0].setLeaf(0, 0, numPrimitives);

        }else{
            recursiveConstruct(0, numPrimitives, 0, bbox, 1);
        }

        primitiveCentroids_.swap(F32Vector());
        primitiveBBoxes_.swap(AABBVector());

        //intersectのためにスタック準備
        if(prevDepth<depth_){
            LIME_FREE(stack_);
            stack_ = (s32*)LIME_MALLOC(sizeof(s32)*depth_);
        }
    }

    s32 BinQBVH::triangulate()
    {
        primitives_.clear();

        s32 numPrimitives = 0;
        for(s32 i=0; i<shapes_.size(); ++i){
            numPrimitives += shapes_[i]->getPrimitiveCount();
        }
        primitives_.reserve(numPrimitives);
        for(s32 i=0; i<shapes_.size(); ++i){
            shapes_[i]->getTriangles(primitives_);
        }
        return primitives_.size();
    }


    void BinQBVH::recursiveConstruct(s32 start, s32 numPrimitives, s32 nodeIndex, const AABB& bbox, s32 depth)
    {
        AABB childBBox[4];
        s32 primStart[4];
        s32 num[4];
        {
            Node& node = nodes_[nodeIndex];
            primStart[0] = start;

            if(MaxBinningDepth<depth){
                splitMid(node.axis0_, num[0], num[2], childBBox[0], childBBox[2], primStart[0], numPrimitives, bbox);
                primStart[2] = start + num[0];

                splitMid(node.axis1_, num[0], num[1], childBBox[0], childBBox[1], primStart[0], num[0], childBBox[0]);
                primStart[1] = start + num[0];

                splitMid(node.axis2_, num[2], num[3], childBBox[2], childBBox[3], primStart[2], num[2], childBBox[2]);
                primStart[3] = primStart[2] + num[2];

            } else{
                //top分割
                f32 area = bbox.halfArea();
                if(area<=Epsilon){
                    splitMid(node.axis0_, num[0], num[2], childBBox[0], childBBox[2], primStart[0], numPrimitives, bbox);

                } else if(numPrimitives<NumBins){
                    split(node.axis0_, num[0], num[2], childBBox[0], childBBox[2], 1.0f/area, primStart[0], numPrimitives, bbox);

                } else{
                    splitBinned(node.axis0_, num[0], num[2], childBBox[0], childBBox[2], area, primStart[0], numPrimitives, bbox);
                }
                primStart[2] = start + num[0];

                //left分割
                area = childBBox[0].halfArea();
                if(area<=Epsilon){
                    splitMid(node.axis1_, num[0], num[1], childBBox[0], childBBox[1], primStart[0], num[0], childBBox[0]);

                } else if(num[0]<NumBins){
                    split(node.axis1_, num[0], num[1], childBBox[0], childBBox[1], 1.0f/area, primStart[0], num[0], childBBox[0]);

                } else{
                    splitBinned(node.axis1_, num[0], num[1], childBBox[0], childBBox[1], area, primStart[0], num[0], childBBox[0]);
                }
                primStart[1] = start + num[0];

                //right分割
                area = childBBox[2].halfArea();
                if(area<=Epsilon){
                    splitMid(node.axis2_, num[2], num[3], childBBox[2], childBBox[3], primStart[2], num[2], childBBox[2]);

                } else if(num[2]<NumBins){
                    split(node.axis2_, num[2], num[3], childBBox[2], childBBox[3], 1.0f/area, primStart[2], num[2], childBBox[2]);

                } else{
                    splitBinned(node.axis2_, num[2], num[3], childBBox[2], childBBox[3], area, primStart[2], num[2], childBBox[2]);
                }
                primStart[3] = primStart[2] + num[2];
            }

            node.setBBox(childBBox);

            for(s32 i=0; i<4; ++i){
                if(num[i]<=MinLeafPrimitives){
#if !LRENDER_BINQBVH_SSEPACK
                    nodes_[nodeIndex].setLeaf(i, primStart[i], num[i]);
#else
                    pushPrimitives(nodes_[nodeIndex], i, primStart[i], num[i]);
#endif
                }else{
                    s32 childIndex = nodes_.size();
                    nodes_.push_back(Node());
                    nodes_[childIndex].clear();
                    nodes_[nodeIndex].children_[i] = childIndex;
                }
            }
        }

        ++depth;
        for(s32 i=0; i<4; ++i){
            if(nodes_[nodeIndex].isJoint(i)){
                recursiveConstruct(primStart[i], num[i], nodes_[nodeIndex].children_[i], childBBox[i], depth);
            }
        }
        depth_ = lcore::maximum(depth, depth_);
    }

    void BinQBVH::split(s32& axis, s32& num_l, s32& num_r, AABB& bbox_l, AABB& bbox_r, f32 invArea, s32 start, s32 numPrimitives, const AABB& bbox)
    {
        s32 end = start + numPrimitives;
        s32 mid=start+(numPrimitives >> 1);

        f32 area_l, area_r;
        f32 bestCost = std::numeric_limits<f32>::max();

        //SAH, 全ての分割を試す
        axis = bbox.maxExtentAxis();
        f32* bestCentroids = &primitiveCentroids_[0] + axis*primitives_.size();
        insertionsort(numPrimitives, &primitives_[start], bestCentroids);

        AABB bl, br;
        for(s32 m=start+1; m<end; ++m){
            getBBox(bl, start, m);
            getBBox(br, m, end);

            area_l = bl.halfArea();
            area_r = br.halfArea();
            num_l = m-start;
            num_r = numPrimitives - num_l;

            f32 cost = SAH_KT_ + SAH_KI_*invArea*(area_l*num_l + area_r*num_r);
            if(cost<bestCost){
                mid = m;
                bestCost = cost;
                bbox_l = bl;
                bbox_r = br;
            }
        }

        num_l = mid-start;
        num_r = numPrimitives - num_l;
    }

    void BinQBVH::splitMid(s32& axis, s32& num_l, s32& num_r, AABB& bbox_l, AABB& bbox_r, s32 start, s32 numPrimitives, const AABB& bbox)
    {
        //最大の軸を半分に分割
        axis = bbox.maxExtentAxis();

        s32 end = start + numPrimitives;
        num_l = (numPrimitives >> 1);
        num_r = numPrimitives - num_l;
        s32 mid=start+num_l;

        f32* centroids = &primitiveCentroids_[0] + axis * primitives_.size();
        sort(numPrimitives, &primitives_[start], centroids);

        getBBox(bbox_l, start, mid);
        getBBox(bbox_r, mid, end);
    }

    void BinQBVH::splitBinned(s32& axis, s32& num_l, s32& num_r, AABB& bbox_l, AABB& bbox_r, f32 area, s32 start, s32 numPrimitives, const AABB& bbox)
    {
        LIME_ALIGN16 s32 minBins[NumBins];
        LIME_ALIGN16 s32 maxBins[NumBins];

        __m128 zero = _mm_setzero_ps();

        f32 invArea = 1.0f/area;
        axis = 0;
        s32 end = start + numPrimitives;

        f32* centroids = &primitiveCentroids_[0];
        f32* bestCentroids = centroids;

        f32 bestCost = std::numeric_limits<f32>::max();
        s32 midBin = NumBins/2;
        s32 step = static_cast<s32>(::log10f(static_cast<f32>(numPrimitives)));

        Vector3 extent =bbox.extent();
        Vector3 unit;
        unit.mul((1.0f/NumBins), extent);
        for(s32 curAxis=0; curAxis<3; ++curAxis){
            for(s32 i=0; i<NumBins; i+=4){
                _mm_store_ps(reinterpret_cast<f32*>(&minBins[i]), zero);
                _mm_store_ps(reinterpret_cast<f32*>(&maxBins[i]), zero);
            }
            sort(numPrimitives, &primitives_[start], centroids);

            f32 invUnit = (lcore::absolute(unit[curAxis])<Epsilon)? 0.0f : 1.0f/unit[curAxis];
            f32 bmin = bbox.bmin_[curAxis];

            for(s32 i = start; i < end; i+=step){
                s32 index = primitives_[i].index_;
                s32 minIndex = lcore::minimum(static_cast<s32>(invUnit * (primitiveBBoxes_[index].bmin_[curAxis] - bmin)), NumBins-1);
                s32 maxIndex = lcore::minimum(static_cast<s32>(invUnit * (primitiveBBoxes_[index].bmax_[curAxis] - bmin)), NumBins-1);
                LASSERT(0<=minIndex && minIndex<NumBins);
                LASSERT(0<=maxIndex && maxIndex<NumBins);
                ++minBins[minIndex];
                ++maxBins[maxIndex];
            }

            Vector3 e = extent; e[curAxis] = unit[curAxis];
            f32 unitArea = getHalfArea(e);

            s32 binLeft = 0;
            s32 binRight = NumBins - 1;

            while(minBins[binLeft]<=0){++binLeft;}
            while(maxBins[binRight]<=0){--binRight;}
            LASSERT(0<=binLeft&&binLeft<NumBins);
            LASSERT(0<=binRight&&binRight<NumBins);

            s32 n_l = 0;
            s32 n_r = maxBins[binRight];
            for(s32 i=0; i<binRight; ++i){
                n_r += maxBins[i];
            }
            for(s32 m=binLeft; m<=binRight; ++m){
                f32 area_l = m*unitArea;
                f32 area_r = (NumBins-m)*unitArea;
                f32 cost = SAH_KT_ + SAH_KI_*invArea*(area_l*n_l + area_r*n_r);
                if(cost<bestCost){
                    midBin = m;
                    bestCost = cost;
                    axis = curAxis;
                    bestCentroids = centroids;
                }

                n_l += minBins[m];
                n_r -= maxBins[m];
            }

            centroids += primitives_.size();
        }//for(s32 curAxis=0;

        f32 separate = unit[axis] * midBin + bbox.bmin_[axis];
        s32 mid = start+(numPrimitives >> 1);

        s32 left = start;
        s32 right = end-1;

        for(;;){
            
            while(left<end && bestCentroids[ primitives_[left].index_ ] < separate){
                ++left;
            }
            while(start<=right && separate <= bestCentroids[ primitives_[right].index_ ]){
                --right;
            }

            if(right<=left){
                mid = left;
                break;
            }
            lcore::swap(primitives_[left], primitives_[right]);
            ++left;
            --right;
        }

        LASSERT(start<=mid && mid<=end);
        if(mid == start || mid == end){
            splitMid(axis, num_l, num_r, bbox_l, bbox_r, start, numPrimitives, bbox);
        } else{

            getBBox(bbox_l, start, mid);
            getBBox(bbox_r, mid, end);

            num_l = mid - start;
            num_r = numPrimitives - num_l;
        }
    }

    bool BinQBVH::intersect(Intersection& intersection, const Ray& ray) const
    {
        __m128 origin[3];
        __m128 direction[3];
        __m128 invDir[3];
        __m128 tminSSE;
        __m128 tmaxSSE;
        origin[0] = _mm_set1_ps(ray.origin_.x_);
        origin[1] = _mm_set1_ps(ray.origin_.y_);
        origin[2] = _mm_set1_ps(ray.origin_.z_);

#if LRENDER_BINQBVH_SSEPACK
        direction[0] = _mm_set1_ps(ray.direction_.x_);
        direction[1] = _mm_set1_ps(ray.direction_.y_);
        direction[2] = _mm_set1_ps(ray.direction_.z_);
#endif

        invDir[0] = _mm_set1_ps(ray.invDirection_.x_);
        invDir[1] = _mm_set1_ps(ray.invDirection_.y_);
        invDir[2] = _mm_set1_ps(ray.invDirection_.z_);

        tminSSE = _mm_set1_ps(ray.epsilon_);
        tmaxSSE = _mm_set1_ps(ray.tmax_);

        s32 raySign[3];
        raySign[0] = (0.0f<=ray.direction_[0])? 0 : 1;
        raySign[1] = (0.0f<=ray.direction_[1])? 0 : 1;
        raySign[2] = (0.0f<=ray.direction_[2])? 0 : 1;

        const Shape* shape = NULL;
        ShapePrimitive shapePrimitive;
        Ray r = ray;
        f32 u = 0.0f, v = 0.0f;
        s32 stack = 0;
        u32 nodeStack[64];
        nodeStack[0] = 0;
        while(0<=stack){
            u32 index = nodeStack[stack];
            --stack;
            if(Node::isLeaf(index)){
                if(Node::isEmpty(index)){
                    continue;
                }
                u32 primIndex = Node::getPrimitiveIndex(index);

#if !LRENDER_BINQBVH_SSEPACK
                u32 primEnd = primIndex + Node::getNumPrimitives(index);
                for(u32 i=primIndex; i<primEnd; ++i){
                    f32 t;
                    s32 shp = primitives_[i].shape_;
                    s32 prim = primitives_[i].primitive_;

                    f32 tu, tv;
                    if(!shapes_[shp]->intersect(t, tu, tv, prim, r)){
                        continue;
                    }

                    if(r.epsilon_ < t && t < r.tmax_){
                        r.tmax_ = t;
                        u = tu;
                        v = tv;
                        shapePrimitive = primitives_[i];
                        shape = shapes_[shp];
                    }
                }//for(u32 i=primIndex;
                tmaxSSE = _mm_set1_ps(r.tmax_);
#else
                
                testPrimitives(u, v, tmaxSSE, r, origin, direction, shapePrimitive, shape, primIndex, Node::getNumPrimitives(index));
#endif

            }else{
                const Node& node = nodes_[index];
                s32 hit = testRayAABB(tminSSE, tmaxSSE, origin, invDir, raySign, node.bbox_);

                s32 split = raySign[node.axis0_] + (raySign[node.axis1_]<<1) + (raySign[node.axis2_]<<2);

                //それぞれの分割で反転するか. 2x2x2
                static const u16 TraverseOrder[] =
                {
                    0x0123U, 0x2301U, 0x1023U, 0x3201U, 0x0132U, 0x2301U, 0x1032U, 0x3210U,
                };

                u16 order = TraverseOrder[split];

                for(s32 i=0; i<4; ++i){
                    u16 o = order&0x03U;
                    if(hit&(0x01U<<o)){
                        nodeStack[++stack] = node.children_[o];
                    }
                    order >>= 4;
                }
            }
        }//while(0<=stack){

        if(NULL != shape){
            shape->getIntersection(intersection, r, u, v, shapePrimitive.primitive_);
            return true;
        }else{
            return false;
        }
    }

    bool BinQBVH::intersect(const Ray& ray) const
    {
        __m128 origin[3];
        __m128 direction[3];
        __m128 invDir[3];
        __m128 tminSSE;
        __m128 tmaxSSE;
        origin[0] = _mm_set1_ps(ray.origin_.x_);
        origin[1] = _mm_set1_ps(ray.origin_.y_);
        origin[2] = _mm_set1_ps(ray.origin_.z_);

#if LRENDER_BINQBVH_SSEPACK
        direction[0] = _mm_set1_ps(ray.direction_.x_);
        direction[1] = _mm_set1_ps(ray.direction_.y_);
        direction[2] = _mm_set1_ps(ray.direction_.z_);
#endif

        invDir[0] = _mm_set1_ps(ray.invDirection_.x_);
        invDir[1] = _mm_set1_ps(ray.invDirection_.y_);
        invDir[2] = _mm_set1_ps(ray.invDirection_.z_);

        tminSSE = _mm_set1_ps(ray.epsilon_);
        tmaxSSE = _mm_set1_ps(ray.tmax_);

        s32 raySign[3];
        raySign[0] = (0.0f<=ray.direction_[0])? 0 : 1;
        raySign[1] = (0.0f<=ray.direction_[1])? 0 : 1;
        raySign[2] = (0.0f<=ray.direction_[2])? 0 : 1;

        const Shape* shape = NULL;
        ShapePrimitive shapePrimitive;
        Ray r = ray;
        f32 u, v;
        s32 stack = 0;
        u32 nodeStack[64];
        nodeStack[0] = 0;
        while(0<=stack){
            u32 index = nodeStack[stack];
            --stack;
            if(Node::isLeaf(index)){
                if(Node::isEmpty(index)){
                    continue;
                }
                u32 primIndex = Node::getPrimitiveIndex(index);

#if !LRENDER_BINQBVH_SSEPACK
                u32 primEnd = primIndex + Node::getNumPrimitives(index);
                for(u32 i=primIndex; i<primEnd; ++i){
                    f32 t;
                    s32 shp = primitives_[i].shape_;
                    s32 prim = primitives_[i].primitive_;

                    if(!shapes_[shp]->intersect(t, u, v, prim, r)){
                        continue;
                    }

                    if(r.epsilon_ < t && t < r.tmax_){
                        r.tmax_ = t;
                        shapePrimitive = primitives_[i];
                        shape = shapes_[shp];
                    }
                }//for(u32 i=primIndex;
                tmaxSSE = _mm_set1_ps(r.tmax_);
#else
                
                testPrimitives(u, v, tmaxSSE, r, origin, direction, shapePrimitive, shape, primIndex, Node::getNumPrimitives(index));
#endif
            }else{
                const Node& node = nodes_[index];
                s32 hit = testRayAABB(tminSSE, tmaxSSE, origin, invDir, raySign, node.bbox_);

                s32 split = raySign[node.axis0_] + (raySign[node.axis1_]<<1) + (raySign[node.axis2_]<<2);

                //それぞれの分割で反転するか. 2x2x2
                static const u16 TraverseOrder[] =
                {
                    0x0123U, 0x2301U, 0x1023U, 0x3201U, 0x0132U, 0x2301U, 0x1032U, 0x3210U,
                };

                u16 order = TraverseOrder[split];

                for(s32 i=0; i<4; ++i){
                    u16 o = order&0x03U;
                    if(hit&(0x01U<<o)){
                        nodeStack[++stack] = node.children_[o];
                    }
                    order >>= 4;
                }
            }
        }//while(0<=stack){
        return (NULL != shape);
    }

#if LRENDER_BINQBVH_SSEPACK
    void BinQBVH::pushPrimitives(Node& node, s32 index, s32 start, s32 num)
    {
        s32 num4 = num>>2;
        s32 rest = num-(num4<<2);

        s32 numPackes = (num+0x03)>>2;

        s32 capacity = numPackes_ + numPackes;
        if(numPackesCapacity_<capacity){
            capacity = numPackesCapacity_ + 16;
            Primitive4Pack* packes = (Primitive4Pack*)LIME_ALIGNED_MALLOC(sizeof(Primitive4Pack)*capacity, PackAlign);
            lcore::memcpy(packes, packes_, sizeof(Primitive4Pack)*numPackesCapacity_);
            LIME_ALIGNED_FREE(packes_, PackAlign);
            packes_ = packes;
            numPackesCapacity_ = capacity;
        }

        node.setLeaf(index, start, num);
        primitives_[start].pack_ = numPackes_;

        LIME_ALIGN16 Vector4 vx[3];
        LIME_ALIGN16 Vector4 vy[3];
        LIME_ALIGN16 Vector4 vz[3];
        for(s32 i=0; i<num4; ++i){
            s32 prim = start + (i<<2);
            for(s32 j = 0; j < 4; ++j){
                const ShapePrimitive& p = primitives_[prim + j];
                shapes_[p.shape_]->getPrimitiveSOA(vx, vy, vz, j, p.primitive_);
            }

            Primitive4Pack& pack = packes_[numPackes_+i];
            //pack.x_[0] = vx[0]; pack.x_[1] = vx[1]; pack.x_[2] = vx[2];
            //pack.y_[0] = vy[0]; pack.y_[1] = vy[1]; pack.y_[2] = vy[2];
            //pack.z_[0] = vz[0]; pack.z_[1] = vz[1]; pack.z_[2] = vz[2];

            for(s32 j = 0; j < 3; ++j){
                pack.x_[j] = _mm_load_ps((const f32*)&vx[j]);
                pack.y_[j] = _mm_load_ps((const f32*)&vy[j]);
                pack.z_[j] = _mm_load_ps((const f32*)&vz[j]);
            }
        }

        if(0 < rest){
            //vx[0].zero(); vx[1].zero(); vx[2].zero();
            //vy[0].zero(); vy[1].zero(); vy[2].zero();
            //vz[0].zero(); vz[1].zero(); vz[2].zero();

            s32 prim = start + (num4 << 2);
            for(s32 j = 0; j < rest; ++j){
                const ShapePrimitive& p = primitives_[prim + j];
                shapes_[p.shape_]->getPrimitiveSOA(vx, vy, vz, j, p.primitive_);
            }
            Primitive4Pack& pack = packes_[numPackes_+num4];
            //pack.x_[0] = vx[0]; pack.x_[1] = vx[1]; pack.x_[2] = vx[2];
            //pack.y_[0] = vy[0]; pack.y_[1] = vy[1]; pack.y_[2] = vy[2];
            //pack.z_[0] = vz[0]; pack.z_[1] = vz[1]; pack.z_[2] = vz[2];

            for(s32 j = 0; j < 3; ++j){
                pack.x_[j] = _mm_load_ps((const f32*)&vx[j]);
                pack.y_[j] = _mm_load_ps((const f32*)&vy[j]);
                pack.z_[j] = _mm_load_ps((const f32*)&vz[j]);
            }
        }

        numPackes_ += numPackes;
    }

    //void BinQBVH::checkPrimitives(s32 start, s32 num) const
    //{
    //    s32 num4 = num>>2;
    //    s32 rest = num-(num4<<2);
    //    s32 packStart = primitives_[start].pack_;

    //    Vector4 vx[3];
    //    Vector4 vy[3];
    //    Vector4 vz[3];
    //    for(s32 i=0; i<num4; ++i){
    //        s32 prim = start + (i<<2);
    //        for(s32 j = 0; j < 4; ++j){
    //            const ShapePrimitive& p = primitives_[prim + j];
    //            shapes_[p.shape_]->getPrimitiveSOA(vx, vy, vz, j, p.primitive_);
    //        }

    //        Primitive4Pack& pack = packes_[packStart+i];
    //        for(s32 j=0; j<3; ++j){
    //            LASSERT(pack.x_[j].isEqual(vx[j]));
    //            LASSERT(pack.y_[j].isEqual(vy[j]));
    //            LASSERT(pack.z_[j].isEqual(vz[j]));
    //        }
    //    }

    //    if(0 < rest){
    //        vx[0].zero(); vx[1].zero(); vx[2].zero();
    //        vy[0].zero(); vy[1].zero(); vy[2].zero();
    //        vz[0].zero(); vz[1].zero(); vz[2].zero();

    //        s32 prim = start + (num4 << 2);
    //        for(s32 j = 0; j < rest; ++j){
    //            const ShapePrimitive& p = primitives_[prim + j];
    //            shapes_[p.shape_]->getPrimitiveSOA(vx, vy, vz, j, p.primitive_);
    //        }
    //        Primitive4Pack& pack = packes_[packStart+num4];
    //        for(s32 j=0; j<3; ++j){
    //            LASSERT(pack.x_[j].isEqual(vx[j]));
    //            LASSERT(pack.y_[j].isEqual(vy[j]));
    //            LASSERT(pack.z_[j].isEqual(vz[j]));
    //        }
    //    }
    //}

    void BinQBVH::testPrimitives(
        f32& u, f32& v,
        __m128& tmaxSSE,
        Ray& ray,
        const __m128 origin[3],
        const __m128 direction[3],
        ShapePrimitive& shapePrimitive,
        const Shape*& shape,
        s32 start, s32 num) const
    {
        s32 num4 = num>>2;
        s32 rest = num-(num4<<2);
        s32 packStart = primitives_[start].pack_;

        __m128 tt, tu, tv;
        for(s32 i=0; i<num4; ++i){
            Primitive4Pack& pack = packes_[packStart+i];

#if 0
            s32 flag = 0;
            f32 st, su, sv;
            for(s32 j=0; j<4; ++j){
                Vector3 v0(pack.x_[0].m128_f32[j], pack.y_[0].m128_f32[j], pack.z_[0].m128_f32[j]);
                Vector3 v1(pack.x_[1].m128_f32[j], pack.y_[1].m128_f32[j], pack.z_[1].m128_f32[j]);
                Vector3 v2(pack.x_[2].m128_f32[j], pack.y_[2].m128_f32[j], pack.z_[2].m128_f32[j]);
                if(testRayTriangle(st, su, sv, ray, v0, v1, v2)){
                    flag |= (1<<j);
                    tt.m128_f32[j] = st;
                    tu.m128_f32[j] = su;
                    tv.m128_f32[j] = sv;
                }
            }

            __m128 tt2, tu2, tv2;
            s32 flag2 = testRayTriangle(
                tt2, tu2, tv2,
                origin, direction,
                pack.x_, pack.y_, pack.z_);

            if(flag != flag2){
                LASSERT(false);
            }
            LASSERT(flag == flag2);
            for(s32 j=0; j<4; ++j){
                if(0 == (flag2 & (1<<j))){
                    continue;
                }
                LASSERT(tt.m128_f32[j] == tt2.m128_f32[j]);
                LASSERT(tu.m128_f32[j] == tu2.m128_f32[j]);
                LASSERT(tv.m128_f32[j] == tv2.m128_f32[j]);
            }
#else
            s32 flag = testRayTriangle(
                tt, tu, tv,
                origin, direction,
                pack.x_, pack.y_, pack.z_);
#endif
            for(s32 j=0; j<4; ++j){
                if(0 == (flag & (1<<j))){
                    continue;
                }

                f32 t = tt.m128_f32[j];
                if(ray.epsilon_<t && t<ray.tmax_){
                    ray.tmax_ = t;
                    u = tu.m128_f32[j];
                    v = tv.m128_f32[j];
                    shapePrimitive = primitives_[(i<<2) + start + j];
                    shape = shapes_[shapePrimitive.shape_];
                }
            }

        }

        if(0 < rest){
            Primitive4Pack& pack = packes_[packStart+num4];
#if 0
            s32 flag = 0;
            f32 st, su, sv;
            for(s32 j=0; j<4; ++j){
                Vector3 v0(pack.x_[0].m128_f32[j], pack.y_[0].m128_f32[j], pack.z_[0].m128_f32[j]);
                Vector3 v1(pack.x_[1].m128_f32[j], pack.y_[1].m128_f32[j], pack.z_[1].m128_f32[j]);
                Vector3 v2(pack.x_[2].m128_f32[j], pack.y_[2].m128_f32[j], pack.z_[2].m128_f32[j]);
                if(testRayTriangle(st, su, sv, ray, v0, v1, v2)){
                    flag |= (1<<j);
                    tt.m128_f32[j] = st;
                    tu.m128_f32[j] = su;
                    tv.m128_f32[j] = sv;
                }
            }

            __m128 tt2, tu2, tv2;
            s32 flag2 = testRayTriangle(
                tt2, tu2, tv2,
                origin, direction,
                pack.x_, pack.y_, pack.z_);

            LASSERT(flag == flag2);
            for(s32 j=0; j<4; ++j){
                if(0 == (flag2 & (1<<j))){
                    continue;
                }
                LASSERT(tt.m128_f32[j] == tt2.m128_f32[j]);
                LASSERT(tu.m128_f32[j] == tu2.m128_f32[j]);
                LASSERT(tv.m128_f32[j] == tv2.m128_f32[j]);
            }

#else
            s32 flag = testRayTriangle(
                tt, tu, tv,
                origin, direction,
                pack.x_, pack.y_, pack.z_);
#endif

            for(s32 j=0; j<rest; ++j){
                if(0 == (flag & (1<<j))){
                    continue;
                }

                f32 t = tt.m128_f32[j];
                if(ray.epsilon_<t && t<ray.tmax_){
                    ray.tmax_ = t;
                    u = tu.m128_f32[j];
                    v = tv.m128_f32[j];

                    s32 prim = start + (num4 << 2) + j;
                    shapePrimitive = primitives_[prim];
                    shape = shapes_[shapePrimitive.shape_];
                }
            }

        } //if(0 < rest)

        tmaxSSE = _mm_set1_ps(ray.tmax_);
    }
#endif
}
