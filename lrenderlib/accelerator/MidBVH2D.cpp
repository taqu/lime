/**
@file MidBVH2D.cpp
@author t-sakai
@date 2015/10/07 create
*/
#include "MidBVH2D.h"

namespace lrender
{
    const f32 MidBVH2D::Epsilon = 1.0e-6f;

    MidBVH2D::MidBVH2D()
        :depth_(0)
        ,stack_(NULL)
    {
    }

    MidBVH2D::~MidBVH2D()
    {
        LIME_FREE(stack_);
    }

    void MidBVH2D::clearShapes()
    {
        shapes_.clear();
    }

    void MidBVH2D::addShape(const Shape::pointer& shape)
    {
        if(shapes_.size()<=shape->getID()){
            shapes_.resize(shape->getID()+1);
        }
        shapes_[shape->getID()] = shape.get();
    }

    s32 MidBVH2D::triangulate()
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

    void MidBVH2D::build()
    {
        s32 numPrimitives = triangulate();

        f32 depth = logf(static_cast<f32>(numPrimitives) / MinLeafPrimitives) / logf(2.0f);
        s32 numNodes = static_cast<s32>(powf(2.0f, depth) + 0.5f);

        nodes_.reserve(numNodes);
        nodes_.resize(1);
        nodes_[0].clear();

        primitiveCentroids_.resize(numPrimitives*2);
        primitiveBBoxes_.resize(numPrimitives);

        //各primitiveのcentroid, bboxを事前計算
        f32* centroidX = &primitiveCentroids_[0];
        f32* centroidY = centroidX + numPrimitives;
        f32* centroidZ = centroidY + numPrimitives;
        for(s32 i=0; i<numPrimitives; ++i){
            primitives_[i].index_ = i;

            Vector2 centroid = getCentroid(primitives_[i]);
            centroidX[i] = centroid.x_;
            centroidY[i] = centroid.y_;
            primitiveBBoxes_[i] = getBBox(primitives_[i]);
            nodes_[0].bbox_.extend( primitiveBBoxes_[i] );
        }

        s32 prevDepth = depth_;
        depth_ = 0;
        recursiveConstruct(0, numPrimitives, 0, 1);

        F32Vector tmpF32Vector;
        primitiveCentroids_.swap(tmpF32Vector);
        AABBVector tmpAABBVector;
        primitiveBBoxes_.swap(tmpAABBVector);

        //intersectのためにスタック準備
        if(prevDepth<depth_){
            LIME_FREE(stack_);
            stack_ = (s32*)LIME_MALLOC(sizeof(s32)*depth_);
        }
    }


    void MidBVH2D::recursiveConstruct(s32 start, s32 numPrimitives, s32 nodeIndex, s32 depth)
    {
        if (numPrimitives <= MinLeafPrimitives){
            nodes_[nodeIndex].setLeaf(start, numPrimitives);
            depth_ = lcore::maximum(depth, depth_);
            return;
        }

        s32 end = start + numPrimitives;

        s32 num_l, num_r, mid=start+(numPrimitives >> 1);
        AABB2D bbox_l, bbox_r;
        s32 axis = 0;

        num_l = (numPrimitives >> 1);
        num_r = numPrimitives - num_l;

        axis = nodes_[nodeIndex].bbox_.maxExtentAxis();
        f32* centroids = &primitiveCentroids_[0] + axis*primitives_.size();
        sort(numPrimitives, &primitives_[start], centroids);

        getBBox(bbox_l, start, mid);
        getBBox(bbox_r, mid, end);


        s32 childIndex = nodes_.size();
        nodes_.push_back(Node());
        nodes_.push_back(Node());

        nodes_[nodeIndex].child_ = childIndex;

        nodes_[childIndex].bbox_ = bbox_l;
        nodes_[childIndex+1].bbox_ = bbox_r;

        ++depth;
        recursiveConstruct(start, num_l, nodes_[nodeIndex].child_, depth);
        recursiveConstruct(start+num_l, num_r, nodes_[nodeIndex].child_+1, depth);
    }

    bool MidBVH2D::intersect(HitRecord& hitRecord, const Vector2& point) const
    {
        hitRecord.shape_ = NULL;
        s32 top = 0;

        s32 currentNode = 0;
        for(;;){
            const Node& node = nodes_[currentNode];

            //f32 tmin, tmax;
            //bool hit = node.bbox_.testRay(tmin, tmax, ray);
            //if(hit){
            //    printf("%f, %f, %f\n", ray.direction_.x_, ray.direction_.y_, ray.direction_.z_);
            //}
            if(node.isLeaf()){
                s32 primIndex = node.getPrimitiveIndex();
                s32 primEnd = primIndex + node.getNumPrimitives();

                Vector2 uvs[3];
                for(s32 i=primIndex; i<primEnd; ++i){
                    s32 shp = primitives_[i].shape_;
                    s32 prim = primitives_[i].primitive_;
                    shapes_[shp]->getTexcoord(uvs, prim);

                    f32 b0, b1, b2;
                    if(!testPointInTriangle(b0, b1, b2, point, uvs[0], uvs[1], uvs[2])){
                        continue;
                    }
                    hitRecord.b0_ = b0;
                    hitRecord.b1_ = b1;
                    hitRecord.b2_ = b2;
                    hitRecord.primitive_ = prim;
                    hitRecord.shape_ = shapes_[shp];
                    return true;
                }
                if(top <= 0){
                    break;
                }
                currentNode = stack_[--top];

            }else{
                const Node& nodeLeft = nodes_[node.child_];
                const Node& nodeRight = nodes_[node.child_+1];
                bool hitLeft = nodeLeft.bbox_.testPoint(point);
                bool hitRight = nodeRight.bbox_.testPoint(point);

                if(hitLeft && !hitRight){
                    currentNode = node.child_;

                }else if(!hitLeft && hitRight){
                    currentNode = node.child_ + 1;

                }else if(hitLeft && hitRight){
                    currentNode = node.child_;
                    s32 farNode = node.child_+1;
                    stack_[top++] = farNode;

                }else{
                    if(top<=0){
                        break;
                    }
                    currentNode = stack_[--top];
                }
            }//if(node.isLeaf()){
        }//for(;;){
        return false;
    }

    void MidBVH2D::swap(MidBVH2D& rhs)
    {
        lcore::swap(depth_, rhs.depth_);
        shapes_.swap(rhs.shapes_);
        nodes_.swap(rhs.nodes_);
        primitives_.swap(rhs.primitives_);
        primitiveCentroids_.swap(rhs.primitiveCentroids_);
        primitiveBBoxes_.swap(rhs.primitiveBBoxes_);
        lcore::swap(stack_, rhs.stack_);
    }
}
