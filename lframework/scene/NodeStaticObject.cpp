/**
@file NodeStaticObject.cpp
@author t-sakai
@date 2015/05/24 create
*/
#include "NodeStaticObject.h"

namespace lscene
{
    NodeStaticObject::NodeStaticObject(const Char* name)
        :NodeObject(name)
    {
    }

    NodeStaticObject::~NodeStaticObject()
    {
    }

    s32 NodeStaticObject::getType() const
    {
        return NodeType_StaticObject;
    }

    void NodeStaticObject::update()
    {

    }
}
