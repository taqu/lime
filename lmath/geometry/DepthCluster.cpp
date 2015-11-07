/**
@file DepthCluster.cpp
@author t-sakai
@date 2015/06/01 create
*/
#include "DepthCluster.h"
#include <lcore/Sort.h>

namespace lmath
{
    void DepthCluster::Iterator::operator++()
    {
        if(size_<=start_){
            return;
        }

        start_ = end_;
        while(++end_<size_){
            if(vertices_[end_].material_ != material_){
                material_ = vertices_[end_].material_;
                return;
            }
        }
        return;
    }

    DepthCluster::DepthCluster()
        :vertices_(NULL)
        ,capacity_(0)
        ,size_(0)
    {
    }

    DepthCluster::DepthCluster(s32 capacity)
        :capacity_(capacity)
        ,size_(0)
    {
        vertices_ = (Vertex*)LIME_ALIGNED_MALLOC(capacity_*sizeof(Vertex), Align);
    }

    DepthCluster::~DepthCluster()
    {
        LIME_ALIGNED_FREE(vertices_, Align);
    }

    void DepthCluster::add(f32 z, u16 id, u16 material)
    {
        if(MaxNum<=size_){
            return;
        }
        if(capacity_<=size_){
            resize();
        }
        vertices_[size_].z_ = z;
        vertices_[size_].id_ = id;
        vertices_[size_].material_ = material;
        ++size_;
    }

    void DepthCluster::construct()
    {
        lcore::introsort(size_, vertices_, compare);
    }

    void DepthCluster::clear()
    {
        size_ = 0;
    }

    DepthCluster::Iterator DepthCluster::begin()
    {
        Iterator iterator;
        iterator.start_ = 0;
        iterator.end_ = 0;
        iterator.size_ = size_;
        iterator.material_ = (size_<=0)? Invalid : vertices_[0].material_;
        iterator.vertices_ = vertices_;
        ++iterator;
        return iterator;
    }

    bool DepthCluster::compare(const Vertex& v0, const Vertex& v1)
    {
        return v1.z_ < v0.z_;
    }

    void DepthCluster::resize()
    {
        capacity_ += 128;

        Vertex* vertices = (Vertex*)LIME_ALIGNED_MALLOC(capacity_*sizeof(Vertex), Align);

        s32 count = size_ >> 3;
        f32* src = reinterpret_cast<f32*>(vertices_);
        f32* dst = reinterpret_cast<f32*>(vertices);
        for(s32 i=0; i<count; ++i){
            lm128 m0 = _mm_load_ps(src);
            lm128 m1 = _mm_load_ps(src+4);
            lm128 m2 = _mm_load_ps(src+8);
            lm128 m3 = _mm_load_ps(src+12);

            _mm_store_ps(dst, m0);
            _mm_store_ps(dst+4, m1);
            _mm_store_ps(dst+8, m2);
            _mm_store_ps(dst+12, m3);

            src += 16;
            dst += 16;
        }
        count = (count<<3);
        for(s32 i=count; i<size_; ++i){
            vertices[i] = vertices_[i];
        }

        LIME_ALIGNED_FREE(vertices_, Align);
        vertices_ = vertices;
    }
}
