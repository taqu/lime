/**
@file NodeBase.cpp
@author t-sakai
@date 2014/10/08 create
*/
#include "NodeBase.h"
#include "SystemBase.h"
#include <lmath/Matrix44.h>

namespace lscene
{
    const lmath::Matrix44 NodeBase::identity_ = lmath::Matrix44(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

    NodeBase::NodeBase(const Char* name)
        :parent_(NULL)
        ,flags_(NodeFlag_Update|NodeFlag_Render)
    {
        if(NULL != name){
            name_.assign(name);
        }
    }

    NodeBase::~NodeBase()
    {
        clearChildren();
    }

    s32 NodeBase::getType() const
    {
        return NodeType_Base;
    }

    NodeBase* NodeBase::getParent()
    {
        return parent_;
    }

    void NodeBase::setParent(NodeBase* parent)
    {
        parent_ = parent;
    }

    void NodeBase::removeFromParent()
    {
        if(parent_){
            parent_->removeChild(this);
        }
    }

    void NodeBase::addChild(NodeBase* child)
    {
        LASSERT(NULL != child);
        LASSERT(this != child);

        if(0<=children_.find(child)){
            return;
        }

        child->addRef();
        if(child->getParent()){
            child->getParent()->removeChild(child);
        }
        child->setParent(this);
        children_.push_back(child);
        child->onAttach();
    }

    void NodeBase::removeChild(NodeBase* child)
    {
        LASSERT(NULL != child);
        LASSERT(this != child);

        s32 ret = children_.find(child);
        if(ret<0){
            return;
        }

        child->onDetach();
        child->setParent(NULL);
        children_.removeAt(ret);
        child->release();
    }

    void NodeBase::clearChildren()
    {
        for(NodeVector::iterator itr = children_.begin();
            itr != children_.end();
            ++itr)
        {
            (*itr)->onDetach();
            (*itr)->setParent(NULL);
            (*itr)->release();
        }
        children_.clear();
    }

    s32 NodeBase::getNumChildren() const
    {
        return children_.size();
    }

    NodeBase* NodeBase::getChild(s32 index)
    {
        return children_[index];
    }

    const NodeBase* NodeBase::getChild(s32 index) const
    {
        return children_[index];
    }

    NodeBase* NodeBase::findChild(const Char* name)
    {
        return findChild(lcore::strlen(name), name);
    }

    NodeBase* NodeBase::findChild(u32 length, const Char* name)
    {
        for(NodeVector::iterator itr = children_.begin();
            itr != children_.end();
            ++itr)
        {
            const NameString& nodeName = (*itr)->getName();
            if(length == nodeName.size() && 0 == lcore::strncmp(name, nodeName.c_str(), length)){
                return (*itr);
            }
        }
        return NULL;
    }

    void NodeBase::onAttach()
    {
    }

    void NodeBase::onDetach()
    {
    }

    void NodeBase::onCollide(NodeBase* opposite, lcollide::CollisionInfo& info)
    {
    }

    void NodeBase::update()
    {
    }

    void NodeBase::visitRenderQueue(RenderContext& renderContext)
    {
        if(checkFlag(NodeFlag_Render)){
            visitRenderQueueChildren(renderContext);
        }
    }

    void NodeBase::render(RenderContext& /*renderContext*/)
    {
    }

    const lmath::Matrix44& NodeBase::getMatrix() const
    {
        return identity_;
    }
}
