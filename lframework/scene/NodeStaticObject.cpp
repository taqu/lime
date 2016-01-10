/**
@file NodeStaticObject.cpp
@author t-sakai
@date 2015/05/24 create
*/
#include "NodeStaticObject.h"

namespace lscene
{
    NodeStaticObject::NodeStaticObject(const Char* name, u16 group, u16 type)
        :NodeObject(name, group, type)
    {
    }

    NodeStaticObject::~NodeStaticObject()
    {
    }

    void NodeStaticObject::update()
    {

    }
}
