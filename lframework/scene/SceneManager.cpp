/**
@file SceneManager.cpp
@author t-sakai
@date 2014/10/08 create
*/
#include "SceneManager.h"
#include "NodeBase.h"

namespace lscene
{
    SceneManager SceneManager::instance_;

    SceneManager::SceneManager()
        :root_(NULL)
        ,updated_(false)
    {
    }

    SceneManager::~SceneManager()
    {
    }

    void SceneManager::initialize()
    {
        terminate();

        root_ = LSCENE_NEW NodeBase("()");
        root_->addRef();
    }

    void SceneManager::terminate()
    {
        LSCENE_RELEASE(root_);
        updateNodes_.swap(UpdateFuncVector());
    }

    void SceneManager::addNode(NodeBase* node)
    {
        root_->addChild(node);
    }

    void SceneManager::removeNode(NodeBase* node)
    {
        root_->removeChild(node);
    }

    void SceneManager::clearNodes()
    {
        root_->clearChildren();
    }

    NodeBase* SceneManager::find(const Char* path)
    {
        LASSERT(NULL != path);
        if(lcore::Delimiter == path[0]){
            ++path;
        }

        u32 length;
        path = lcore::parseNextNameFromPath(path, length, name_, MaxNodeNameBufferSize);

        NodeBase* current = root_;
        while(0<length){
            current = current->findChild(length, name_);
            if(NULL == current){
                return NULL;
            }
            path = lcore::parseNextNameFromPath(path, length, name_, MaxNodeNameBufferSize);
        }
        return (current == root_)? NULL : current;
    }

    void SceneManager::update()
    {
        if(updated_){
            updateNodes_.clear();
            UpdateVisitor updateVisitor(updateNodes_);
            for(NodeBase::NodeVector::iterator itr = root_->children_.begin();
                itr != root_->children_.end();
                ++itr)
            {
                (*itr)->visitUpdate(updateVisitor);
            }
            updated_ = false;
        }

        for(currentUpdateIndex_=0; currentUpdateIndex_<updateNodes_.size();){
            UpdateFunctor& func = updateNodes_[currentUpdateIndex_];
            if(NULL == func.node_){
                continue;
            }

            if(func.node_->checkFlag(NodeFlag_Update)){
                func.node_->update();
                ++currentUpdateIndex_;

            }else{
                currentUpdateIndex_ += func.numUpdates_;
            }
        }
    }

    void SceneManager::visitRenderQueue(RenderContext& renderContext)
    {
        root_->visitRenderQueueChildren(renderContext);
    }

    void SceneManager::setUpdated()
    {
        updated_ = true;
    }

    void SceneManager::removeFromUpdate(NodeBase* node)
    {
        for(s32 i=0; i<updateNodes_.size(); ++i){
            if(updateNodes_[i].node_ == node){
                updateNodes_[i].node_ = NULL;
                updated_ = true;
                return;
            }
        }
    }
}
