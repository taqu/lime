/**
@file NodeBase.cpp
@author t-sakai
@date 2014/10/08 create
*/
#include "NodeBase.h"

#include <lmath/Matrix44.h>
#include "SceneManager.h"

namespace lscene
{
    NodeBase::NodeBase(const Char* name, u16 group, u16 type)
        :Collidable(group, type)
        ,parent_(NULL)
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

    NodeBase* NodeBase::getParent()
    {
        return parent_;
    }

    void NodeBase::setParent(NodeBase* parent)
    {
        parent_ = parent;
        if(NULL != parent){
            parent->addChild(this);
        }
    }

    void NodeBase::removeFromParent()
    {
        NodeBase* oldParent = parent_;
        parent_ = NULL;
        if(NULL != oldParent){
            oldParent->removeChild(this);
        }
    }

    void NodeBase::addChild(NodeBase* child)
    {
        LASSERT(NULL != child);
        LASSERT(this != child);

        for(s32 i=0; i<children_.size(); ++i){
            if(child == children_[i]){
                return;
            }
        }
        SceneManager::getInstance().setUpdated();

        child->addRef();
        child->removeFromParent();
        children_.push_back(child);
        child->parent_ = this;
        child->onAttach();
    }

    void NodeBase::removeChild(NodeBase* child)
    {
        LASSERT(NULL != child);
        LASSERT(this != child);

        s32 count;
        for(count=0; count<children_.size(); ++count){
            if(child == children_[count]){
                break;
            }
        }
        if(children_.size()<=count){
            return;
        }

        child->onDetach();
        SceneManager::getInstance().removeFromUpdate(child);
        child->parent_ = NULL;
        child->release();
        children_.removeAt(count);
    }

    void NodeBase::clearChildren()
    {
        for(s32 i=0; i<children_.size(); ++i){
            NodeBase* child = children_[i];
            child->onDetach();
            SceneManager::getInstance().removeFromUpdate(child);
            child->parent_ = NULL;
            child->release();
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

    void NodeBase::onCollide(Collidable* /*opposite*/, lcollide::CollisionInfo& /*info*/)
    {
    }

    void NodeBase::visitUpdate(UpdateVisitor& visitor)
    {
        s32 index = visitor.push(this);

        for(NodeVector::iterator itr = children_.begin();
            itr != children_.end();
            ++itr)
        {
            (*itr)->visitUpdate(visitor);
        }

        visitor.setNumUpdates(index);
    }

    void NodeBase::update()
    {
    }

    void NodeBase::traverseUpdateTransform()
    {
        for(NodeVector::iterator itr = children_.begin();
            itr != children_.end();
            ++itr)
        {
            (*itr)->traverseUpdateTransform();
        }
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
        return lmath::Matrix44::identity_;
    }

    const lmath::Matrix44& NodeBase::getParentMatrix() const
    {
        return (NULL == parent_)? lmath::Matrix44::identity_ : parent_->getMatrix();
    }
}
