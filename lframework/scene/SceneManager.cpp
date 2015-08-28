/**
@file SceneManager.cpp
@author t-sakai
@date 2014/10/08 create
*/
#include "SceneManager.h"
#include "NodeBase.h"

namespace lscene
{
    SceneManager::SceneManager()
        :root_(NULL)
    {
    }

    SceneManager::~SceneManager()
    {
        terminate();
    }

    void SceneManager::initialize()
    {
        root_ = LSCENE_NEW NodeBase("()");
        root_->addRef();
    }

    void SceneManager::terminate()
    {
        if(root_){
            root_->release();
            root_ = NULL;
        }
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
        while(0<=length){
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
        root_->updateChildren();
    }

    void SceneManager::visitRenderQueue(RenderContext& renderContext)
    {
        root_->visitRenderQueueChildren(renderContext);
    }
}
