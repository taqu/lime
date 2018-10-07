/**
@file RectPack.cpp
@author t-sakai
@date 2017/01/14 create
*/
#include "RectPack.h"

namespace lcore
{
    s32 RectPack::Context::pop()
    {
        s32 left = free_;
        free_ += 2;
        return left;
    }

    s32 RectPack::Node::fit(u16 width, u16 height) const
    {
        if(width == width_ && height == height_){
            return 0;
        }

        if(width<=width_ && height<=height_){
            return 1;
        }
        return -1;
    }

    void RectPack::initialize(Context& context, u16 width, u16 height, s32 numNodes, Node* nodes)
    {
        LASSERT(NULL != nodes);

        LASSERT((lcore::maximum(width, height)*2)<=numNodes);

        context.free_ = 0;
        context.nodes_ = nodes;
        context.width_ = width;
        context.height_ = height;
    }
}
