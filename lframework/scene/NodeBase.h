#ifndef INC_LSCENE_NODEBASE_H__
#define INC_LSCENE_NODEBASE_H__
/**
@file NodeBase.h
@author t-sakai
@date 2014/10/08 create
*/
#include "lscene.h"
#include <lcore/Vector.h>
#include "SceneRenderable.h"

namespace lmath
{
    class Matrix44;
};

namespace lcollide
{
    struct CollisionInfo;
}

namespace lscene
{
    class RenderQueue;

    class NodeBase : public SceneRenderable
    {
    public:
        typedef lcore::vector_arena<NodeBase*, lscene::SceneAllocator, lcore::vector_arena_static_inc_size<4> > NodeVector;

        explicit NodeBase(const Char* name = NULL);
        virtual ~NodeBase();
        virtual s32 getType() const;

        inline bool checkFlag(u32 flag) const;
        inline void setFlag(u32 flag);
        inline void unsetFlag(u32 flag);

        inline const NameString& getName() const;
        inline void setName(const Char* name);

        template<class T>
        inline T* cast();

        NodeBase* getParent();
        void setParent(NodeBase* parent);
        void removeFromParent();

        void addChild(NodeBase* child);
        void removeChild(NodeBase* child);
        void clearChildren();

        s32 getNumChildren() const;
        NodeBase* getChild(s32 index);
        const NodeBase* getChild(s32 index) const;

        NodeBase* findChild(const Char* name);
        NodeBase* findChild(u32 length, const Char* name);

        virtual void onAttach();
        virtual void onDetach();
        virtual void onCollide(NodeBase* opposite, lcollide::CollisionInfo& info);

        inline void updateBase();

        virtual void update();
        virtual void traverseUpdateTransform();
        virtual void visitRenderQueue(RenderContext& renderContext);

        virtual void render(RenderContext& renderContext);

        virtual const lmath::Matrix44& getMatrix() const;

    protected:
        friend class SceneManager;
        NodeBase(const NodeBase&);
        NodeBase& operator=(const NodeBase&);

        inline void updateChildren();
        inline void visitRenderQueueChildren(RenderContext& renderContext);

        static const lmath::Matrix44 identity_;

        u32 flags_;
        NameString name_;
        NodeBase* parent_;
        NodeVector children_;
    };

    inline bool NodeBase::checkFlag(u32 flag) const
    {
        return 0 != (flags_ & flag);
    }

    inline void NodeBase::setFlag(u32 flag)
    {
        flags_ |= flag;
    }

    inline void NodeBase::unsetFlag(u32 flag)
    {
        flags_ &= ~flag;
    }

    inline const NameString& NodeBase::getName() const
    {
        return name_;
    }

    inline void NodeBase::setName(const Char* name)
    {
        name_.assign(name);
    }

    template<class T>
    inline T* NodeBase::cast()
    {
        return reinterpret_cast<T*>(this);
    }

    inline void NodeBase::updateBase()
    {
        if(checkFlag(NodeFlag_Update)){
            update();
            updateChildren();
        }
    }

    inline void NodeBase::updateChildren()
    {
        for(NodeVector::iterator itr = children_.begin();
            itr != children_.end();
            ++itr)
        {
            (*itr)->updateBase();
        }
    }

    inline void NodeBase::visitRenderQueueChildren(RenderContext& renderContext)
    {
        for(NodeVector::iterator itr = children_.begin();
            itr != children_.end();
            ++itr)
        {
            (*itr)->visitRenderQueue(renderContext);
        }
    }
}
#endif //INC_LSCENE_NODEBASE_H__
