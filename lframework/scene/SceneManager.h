#ifndef INC_LSCENE_SCENEMANAGER_H__
#define INC_LSCENE_SCENEMANAGER_H__
/**
@file SceneManager.h
@author t-sakai
@date 2014/10/08 create
*/
#include "lscene.h"
#include <lcore/Vector.h>

namespace lscene
{
    class RenderContext;
    class NodeBase;

    struct UpdateFunctor
    {
        s32 numUpdates_;
        NodeBase* node_;
    };

    class SceneManager
    {
    public:
        typedef lcore::vector_arena<UpdateFunctor, lcore::vector_arena_static_inc_size<128> > UpdateFuncVector;

        inline static SceneManager& getInstance();

        void initialize();
        void terminate();

        void addNode(NodeBase* node);
        void removeNode(NodeBase* node);
        void clearNodes();

        NodeBase* find(const Char* path);
        template<class T>
        T* findCast(const Char* path){ return reinterpret_cast<T*>(find(path)); }

        void update();
        void visitRenderQueue(RenderContext& renderContext);

        void setUpdated();
        void removeFromUpdate(NodeBase* node);
    private:
        SceneManager(const SceneManager&);
        SceneManager& operator=(const SceneManager&);

        static SceneManager instance_;

        SceneManager();
        ~SceneManager();

        NodeBase* root_;
        Char name_[MaxNodeNameBufferSize];
        
        bool updated_;
        s32 currentUpdateIndex_;
        UpdateFuncVector updateNodes_;
    };

    inline SceneManager& SceneManager::getInstance()
    {
        return instance_;
    }

    class UpdateVisitor
    {
    public:
        inline UpdateVisitor(SceneManager::UpdateFuncVector& updateVector);
        inline s32 push(NodeBase* node);
        inline void setNumUpdates(s32 index);
    private:
        UpdateVisitor(const UpdateVisitor&);
        UpdateVisitor& operator=(const UpdateVisitor&);

        SceneManager::UpdateFuncVector& updateVector_;
    };

    inline UpdateVisitor::UpdateVisitor(SceneManager::UpdateFuncVector& updateVector)
        :updateVector_(updateVector)
    {}

    inline s32 UpdateVisitor::push(NodeBase* node)
    {
        LASSERT(NULL != node);

        s32 index = updateVector_.size();
        updateVector_.push_back({0, node});
        return index;
    }

    inline void UpdateVisitor::setNumUpdates(s32 index)
    {
        LASSERT(0<=index && index<updateVector_.size());
        updateVector_[index].numUpdates_ = updateVector_.size()-index;
    }
}
#endif //INC_LSCENE_SCENEMANAGER_H__
