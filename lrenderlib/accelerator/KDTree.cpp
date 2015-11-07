/**
@file KDTree.cpp
@author t-sakai
@date 2013/06/07 create
*/
#include "KDTree.h"

#include <lcore/Sort.h>

#include "Photon.h"

namespace lrender
{
namespace
{
    struct PhotonSortFunc
    {
        PhotonSortFunc(s32 axis)
            :axis_(axis)
        {}

        bool operator()(const Photon& p0, const Photon& p1) const
        {
            return p0.position_[axis_] < p1.position_[axis_];
        }

        s32 axis_;
    };

    void sort(u32 numPhotons, Photon* photons, s32 axis)
    {
        PhotonSortFunc func(axis);
        lcore::heapsort(numPhotons, photons, func);
    }

    s32 calcMaxExtentAxis(u32 numPhotons, const Photon* photons)
    {
        f32 bmin[3] = {lcore::numeric_limits<f32>::maximum(), lcore::numeric_limits<f32>::maximum(), lcore::numeric_limits<f32>::maximum()};
        f32 bmax[3] = {lcore::numeric_limits<f32>::minimum(), lcore::numeric_limits<f32>::minimum(), lcore::numeric_limits<f32>::minimum()};
        for(u32 i=0; i<numPhotons; ++i){
            for(s32 j=0; j<3; ++j){
                if(photons[i].position_[j] < bmin[j]){
                    bmin[j] = photons[i].position_[j];
                }
                if(bmax[j] < photons[i].position_[j]){
                    bmax[j] = photons[i].position_[j];
                }
            }
        }

        f32 extent[3];
        s32 axis = 0;
        for(s32 i=0; i<3; ++i){
            extent[i] = bmax[i] - bmin[i];

            if(extent[axis] < extent[i]){
                axis = i;
            }
        }
        return axis;
    }
}

    KDTree::KDTree()
    {
    }

    KDTree::~KDTree()
    {
    }

    void KDTree::add(const Photon& photon)
    {
        photons_.push_back(photon);
    }

    void KDTree::construct()
    {
        nodes_.clear();

        Photon* photons = &photons_[0];
        u32 numPhotons = photons_.size();


        recursiveConstruct(0, numPhotons, photons, 0);
    }

    s32 KDTree::getParentIndex(s32 index)
    {
        return (index-1)>>1;
    }

    s32 KDTree::getChildStartIndex(s32 index)
    {
        return (index<<1) + 1;
    }

    KDTree::Node& KDTree::createNode(s32 index)
    {
        s32 childStartIndex = getChildStartIndex(index);
        if(nodes_.capacity()<=(childStartIndex+2)){
            nodes_.resize(childStartIndex+64);
        }
        return nodes_[index];
    }

    void KDTree::recursiveConstruct(u32 start, u32 end, Photon* photons, s32 nodeIndex)
    {
        u32 num = end-start;
        s32 split = num>>1;
        s32 childStartIndex = getChildStartIndex(nodeIndex);

        {
            if(num<=MinLeafElements){
                KDTree::Node& node = createNode(nodeIndex);
                node.setLeaf(start, num);
                return;
            }

            u8 axis = calcMaxExtentAxis(num, photons);
            sort(num, photons, axis);

            KDTree::Node& node = createNode(nodeIndex);
            node.setInner(photons[split].position_[axis], axis);
        }
        recursiveConstruct(start, split, photons, childStartIndex);
        recursiveConstruct(split, end, photons+split, childStartIndex+1);
    }
}
