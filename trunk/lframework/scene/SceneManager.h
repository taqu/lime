#ifndef INC_LSCENE_SCENEMANAGER_H__
#define INC_LSCENE_SCENEMANAGER_H__
/**
@file SceneManager.h
@author t-sakai
@date 2014/10/08 create
*/
#include "lscene.h"

namespace lscene
{
    class RenderContext;
    class NodeBase;

    class SceneManager
    {
    public:
        SceneManager();
        ~SceneManager();

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

    private:
        SceneManager(const SceneManager&);
        SceneManager& operator=(const SceneManager&);

        NodeBase* root_;
        Char name_[MaxNodeNameBufferSize];
    };
}
#endif //INC_LSCENE_SCENEMANAGER_H__
