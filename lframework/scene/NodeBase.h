#ifndef INC_LSCENE_NODEBASE_H__
#define INC_LSCENE_NODEBASE_H__
/**
@file NodeBase.h
@author t-sakai
@date 2014/10/08 create
*/
#include "lscene.h"
#include <lcore/Vector.h>
#include "Collidable.h"
#include "SceneRenderable.h"

namespace lmath
{
    class Matrix44;
};

namespace lscene
{
    class RenderQueue;
    class UpdateVisitor;

    class NodeBase : public Collidable
    {
    public:
        typedef lcore::vector_arena<NodeBase*, lcore::vector_arena_static_inc_size<4>, lscene::SceneAllocator> NodeVector;

        explicit NodeBase(const Char* name = NULL, u16 group=Group_None, u16 type=NodeType_Base);
        virtual ~NodeBase();

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

        template<class T>
        inline T* findChild(const Char* name);

        template<class T>
        inline T* findChild(u32 length, const Char* name);

        virtual void onAttach();
        virtual void onDetach();
        virtual void onCollide(Collidable* opposite, lcollide::CollisionInfo& info);

        void visitUpdate(UpdateVisitor& visitor);

        virtual void update();
        virtual void traverseUpdateTransform();
        virtual void visitRenderQueue(RenderContext& renderContext);

        virtual void render(RenderContext& renderContext);

        virtual const lmath::Matrix44& getMatrix() const;
        const lmath::Matrix44& getParentMatrix() const;

    protected:
        friend class SceneManager;

        NodeBase(const NodeBase&);
        NodeBase& operator=(const NodeBase&);

        inline void visitRenderQueueChildren(RenderContext& renderContext);

        inline NodeVector::iterator childBegin();
        inline NodeVector::iterator childEnd();

    private:
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

    template<class T>
    inline T* NodeBase::findChild(const Char* name)
    {
        return reinterpret_cast<T*>(findChild(name));
    }

    template<class T>
    inline T* NodeBase::findChild(u32 length, const Char* name)
    {
        return reinterpret_cast<T*>(findChild(length, name));
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

    inline NodeBase::NodeVector::iterator NodeBase::childBegin()
    {
        return children_.begin();
    }

    inline NodeBase::NodeVector::iterator NodeBase::childEnd()
    {
        return children_.end();
    }
}
#endif //INC_LSCENE_NODEBASE_H__
