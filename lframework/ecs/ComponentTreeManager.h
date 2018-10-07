#ifndef INC_LFRAMEWORK_COMPONENTTREEMANAGER_H_
#define INC_LFRAMEWORK_COMPONENTTREEMANAGER_H_
/**
@file ComponentTreeManager.h
@author t-sakai
@date 2016/09/15 create
*/
#include "../System.h"
#include "ComponentManager.h"
#include "ECSTree.h"
#include <lcore/ObjectAllocator.h>
#include <lcore/Array.h>
#include <lcore/File.h>

namespace lfw
{
    template<class ComponentType>
    class ComponentTreeManager : public ComponentManager
    {
    public:
        typedef ECSNodeIterator<ComponentType> iterator;
        typedef ECSNodeConstIterator<ComponentType> const_iterator;

        typedef lcore::Array<Entity> EntityArray;

        typedef void (*TraverseCallBack)(s32 depth, const ComponentType* component, void* userData);

        static const u32 Flag_Dirty = 0x01U<<0;

        virtual ID create(Entity entity);
        virtual void destroy(ID id);

        virtual Behavior* getBehavior(ID){ return NULL;}
        ComponentType* get(ID id);

        ComponentType* getTreeComponent(ID id);

        virtual void updateComponent(ID id);

        inline const Entity& getEntity(ID id) const;
        void destroyAllEntries();

        const NameString& getName(ID id) const;
        NameString& getName(ID id);

        inline ECSNode& getNode(ID id);
        inline const ECSNode& getNode(ID id) const;

        inline ECSNode& getRoot();
        inline const ECSNode& getRoot() const;
        void setParent(ID id, ComponentType* parent);
        inline void setFirstSibling(ID id);
        inline void setLastSibling(ID id);
        inline void setDirty();

        void add(ID id, Operation::Type type);
        /**
        @return ツリーの更新があった場合true
        */
        bool update();

        inline iterator beginChildren(ID id);
        inline const_iterator beginChildren(ID id) const;

        inline s32 getNumEntities() const;
        inline const Entity* begin() const;
        inline const Entity* end() const;

        ComponentType* find(const Char* path);
        inline void traverse(TraverseCallBack func, void* userData);
        inline void traverse(TraverseCallBack funcBegin, TraverseCallBack funcEnd,void* userData);
    protected:
        ComponentTreeManager(const ComponentTreeManager&);
        ComponentTreeManager& operator=(const ComponentTreeManager&);

        ComponentTreeManager(s32 capacity=DefaultComponentManagerCapacity, s32 expandSize=DefaultComponentManagerExpandSize);
        virtual ~ComponentTreeManager();

        bool updateRootFlags(const Entity& entity);
        void updateFlags(const Entity& entity);
        void destroyRecurse(u16 index);
        void traverse(s32 depth, const ECSNode* node, TraverseCallBack func, void* userData);
        void traverse(s32 depth, const ECSNode* node, TraverseCallBack funcBegin, TraverseCallBack funcEnd, void* userData);

        lcore::BitSet32 flags_;
        IDTable ids_;
        lcore::ObjectAllocator<ComponentType> allocator_;
        ComponentType** components_;
        NameString* names_;
        lcore::Array<Operation> operations_;
        lcore::Array<Operation> workOperations_;
        ECSEntityTree entityTree_;
    };

    template<class ComponentType>
    ComponentTreeManager<ComponentType>::ComponentTreeManager(s32 capacity, s32 expandSize)
        :ids_(capacity, expandSize)
        ,components_(NULL)
        ,names_(NULL)
    {
        LASSERT(0<=capacity);
        allocator_.initialize();
        components_ = LNEW ComponentType*[capacity];
        lcore::memset(components_, 0, sizeof(ComponentType*)*capacity);

        names_ = LNEW NameString[capacity];
        entityTree_.resize(0, capacity);
    }

    template<class ComponentType>
    ComponentTreeManager<ComponentType>::~ComponentTreeManager()
    {
        LDELETE_ARRAY(names_);
        LDELETE_ARRAY(components_);
        allocator_.terminate();
    }

    template<class ComponentType>
    ID ComponentTreeManager<ComponentType>::create(Entity entity)
    {
        s32 capacity = ids_.capacity();
        ID id = ids_.create(category());
        if(capacity<ids_.capacity()){
            s32 newCapacity = ids_.capacity();
            expand(components_, capacity, newCapacity);
            expand(names_, capacity, newCapacity);
            entityTree_.resize(capacity, newCapacity);
        }
        u16 index = id.index();
        void* ptr = allocator_.allocate();
        components_[index] = LPLACEMENT_NEW(ptr) ComponentType;
        components_[index]->setID(id);
        getName(id).clear();
        entityTree_.create(index, entity);
        return id;
    }

    template<class ComponentType>
    void ComponentTreeManager<ComponentType>::destroy(ID id)
    {
        LASSERT(id.valid());
        LASSERT(NULL != components_[id.index()]);
        u16 index = id.index();
        getName(id).clear();
        components_[index]->~ComponentType();
        allocator_.deallocate(components_[index]);
        components_[index] = NULL;
        names_[index].clear();

        entityTree_.destroy(index);
        ids_.destroy(ID::construct(0, index));
    }

    template<class ComponentType>
    ComponentType* ComponentTreeManager<ComponentType>::get(ID id)
    {
        return (id.valid() && 0<=id.index() && id.index()<ids_.capacity())? components_[id.index()] : NULL;
    }

    template<class ComponentType>
    ComponentType* ComponentTreeManager<ComponentType>::getTreeComponent(ID id)
    {
        LASSERT(0<=id.index() && id.index()<ids_.capacity() || ECSNode::Root==id.index());
        LASSERT(id.valid());
        return (ECSNode::Root != id.index())? components_[id.index()] : NULL;
    }

    template<class ComponentType>
    void ComponentTreeManager<ComponentType>::updateComponent(ID)
    {
    }

    template<class ComponentType>
    inline const Entity& ComponentTreeManager<ComponentType>::getEntity(ID id) const
    {
        LASSERT(0<=id.index() && id.index()<ids_.capacity());
        LASSERT(id.valid());
        return entityTree_.getEntity(id.index());
    }

    template<class ComponentType>
    void ComponentTreeManager<ComponentType>::destroyAllEntries()
    {
        ECSManager& ecsManager = System::getECSManager();
        ECSNode& root = getRoot();
        if(0 < root.numChildren()){
            ECSNode* child = &entityTree_.get(root.child());
            for(s32 i=0; i<root.numChildren(); ++i){
                ecsManager.requestDestroy(entityTree_.getEntity(child->index()));
                child = &entityTree_.get(child->next());
            }
        }
        update();
    }

    template<class ComponentType>
    const NameString& ComponentTreeManager<ComponentType>::getName(ID id) const
    {
        LASSERT(0<=id.index() && id.index()<ids_.capacity());
        LASSERT(id.valid());
        return names_[id.index()];
    }

    template<class ComponentType>
    NameString& ComponentTreeManager<ComponentType>::getName(ID id)
    {
        LASSERT(0<=id.index() && id.index()<ids_.capacity());
        LASSERT(id.valid());
        return names_[id.index()];
    }

    template<class ComponentType>
    inline const ECSNode& ComponentTreeManager<ComponentType>::getNode(ID id) const
    {
        LASSERT(id.index() == ECSNode::Root || (0<=id.index() && id.index()<ids_.capacity()));
        LASSERT(id.valid());

        const ECSNode& node = id.index() == ECSNode::Root? entityTree_.getRoot() : entityTree_.get(id.index());
        return node;
    }

    template<class ComponentType>
    inline ECSNode& ComponentTreeManager<ComponentType>::getNode(ID id)
    {
        LASSERT(id.index() == ECSNode::Root || (0<=id.index() && id.index()<ids_.capacity()));
        LASSERT(id.valid());

        ECSNode& node = (id.index() == ECSNode::Root)? entityTree_.getRoot() : entityTree_.get(id.index());
        return node;
    }

    template<class ComponentType>
    inline const ECSNode& ComponentTreeManager<ComponentType>::getRoot() const
    {
        return entityTree_.getRoot();
    }

    template<class ComponentType>
    inline ECSNode& ComponentTreeManager<ComponentType>::getRoot()
    {
        return entityTree_.getRoot();
    }

    template<class ComponentType>
    void ComponentTreeManager<ComponentType>::setParent(ID id, ComponentType* parent)
    {
        ECSNode& node = getNode(id);
        if(NULL == parent){
            entityTree_.setParent(node, getRoot());
        }else{
            entityTree_.setParent(node, getNode(parent->getID()));
        }
        flags_.set(Flag_Dirty);
    }

    template<class ComponentType>
    inline void ComponentTreeManager<ComponentType>::setFirstSibling(ID id)
    {
        LASSERT(id.index() != ECSNode::Root && (0<=id.index() && id.index()<ids_.capacity()));
        LASSERT(id.valid());
        entityTree_.setFirstSibling(getNode(id));
    }

    template<class ComponentType>
    inline void ComponentTreeManager<ComponentType>::setLastSibling(ID id)
    {
        LASSERT(id.index() != ECSNode::Root && (0<=id.index() && id.index()<ids_.capacity()));
        LASSERT(id.valid());
        entityTree_.setLastSibling(getNode(id));
    }

    template<class ComponentType>
    inline void ComponentTreeManager<ComponentType>::setDirty()
    {
        flags_.set(Flag_Dirty);
    }

    template<class ComponentType>
    void ComponentTreeManager<ComponentType>::add(ID id, Operation::Type type)
    {
        for(s32 i=0; i<operations_.size(); ++i){
            if(operations_[i].id() == id){
                operations_[i].type_ = type;
                flags_.set(Flag_Dirty);
                return;
            }
        }
        Operation op;
        op.type_ = type;
        op.id_ = id;
        operations_.push_back(op);
        flags_.set(Flag_Dirty);
    }

    template<class ComponentType>
    bool ComponentTreeManager<ComponentType>::updateRootFlags(const Entity& entity)
    {
        ECSManager& ecsManager = System::getECSManager();
        bool update = ecsManager.checkFlag(entity, EntityFlag_UpdateSelf);
        bool render = ecsManager.checkFlag(entity, EntityFlag_RenderSelf);

        IDConstAccess access = ecsManager.getComponents(entity);
        LASSERT(ECSCategory_Logical == access[0].category() || ECSCategory_Geometric == access[0].category());
        ComponentType* component = getTreeComponent(access[0]);
        const ECSNode& node = getNode(component->getID());
        if(node.parent() != ECSNode::Root){
            return false;
        }

        if(update){
            ecsManager.setFlag(entity, EntityFlag_Update);
        } else{
            ecsManager.resetFlag(entity, EntityFlag_Update);
        }

        if(render){
            ecsManager.setFlag(entity, EntityFlag_Render);
        } else{
            ecsManager.resetFlag(entity, EntityFlag_Render);
        }
        return true;
    }

    template<class ComponentType>
    void ComponentTreeManager<ComponentType>::updateFlags(const Entity& entity)
    {
        ECSManager& ecsManager = System::getECSManager();
        bool update = ecsManager.checkFlag(entity, EntityFlag_UpdateSelf);
        bool render = ecsManager.checkFlag(entity, EntityFlag_RenderSelf);

        IDConstAccess access = ecsManager.getComponents(entity);
        LASSERT(ECSCategory_Logical == access[0].category() || ECSCategory_Geometric == access[0].category());
        ComponentType* component = getTreeComponent(access[0]);

        const ECSNode& node = getNode(component->getID());
        LASSERT(node.parent() != ECSNode::Root);

        const Entity& parent = entityTree_.getEntity(node.parent());
        bool parentUpdate = ecsManager.checkFlag(parent, EntityFlag_Update);
        bool parentRender = ecsManager.checkFlag(parent, EntityFlag_Render);

        if(update && parentUpdate){
            ecsManager.setFlag(entity, EntityFlag_Update);
        } else{
            ecsManager.resetFlag(entity, EntityFlag_Update);
        }

        if(render && parentRender){
            ecsManager.setFlag(entity, EntityFlag_Render);
        } else{
            ecsManager.resetFlag(entity, EntityFlag_Render);
        }
    }

    template<class ComponentType>
    bool ComponentTreeManager<ComponentType>::update()
    {
        if(0 < operations_.size ()) {
            workOperations_.resize(operations_.size());
            lcore::memcpy(&workOperations_[0], &operations_[0], sizeof(Operation)*operations_.size());
            operations_.clear();
            for(s32 i = 0; i < workOperations_.size (); ++i){
                switch(workOperations_[i].type())
                {
                case Operation::Type_Create:
                    break;
                case Operation::Type_Destroy:
                default:
                    destroyRecurse(workOperations_[i].id().index());
                    break;
                }
            }
            workOperations_.clear();
        }

        if(flags_.check(Flag_Dirty)){
            flags_.reset(Flag_Dirty);
            entityTree_.traverse();

            const Entity* itr;
            for(itr = entityTree_.beginEntity();
                itr != entityTree_.endEntity();
                ++itr)
            {
                if(!updateRootFlags(*itr)){
                    break;
                }
            }
            for(;
                itr != entityTree_.endEntity();
                ++itr)
            {
                updateFlags(*itr);
            }
            return true;
        }
        return false;
    }

    template<class ComponentType>
    inline typename ComponentTreeManager<ComponentType>::iterator ComponentTreeManager<ComponentType>::beginChildren(ID id)
    {
        ECSNode& node = getNode(id);
        ECSNode* child = (node.numChildren()<=0)? NULL : &entityTree_.get(node.child());
        return iterator(node.numChildren(), components_, &entityTree_, child);
    }

    template<class ComponentType>
    inline typename ComponentTreeManager<ComponentType>::const_iterator ComponentTreeManager<ComponentType>::beginChildren(ID id) const
    {
        const ECSNode& node = getNode(id);
        const ECSNode* child = (node.numChildren()<=0)? NULL : &entityTree_.get(node.child());
        return const_iterator(node.numChildren(), reinterpret_cast<ComponentType** const>( components_), &entityTree_, child);
    }

    template<class ComponentType>
    inline s32 ComponentTreeManager<ComponentType>::getNumEntities() const
    {
        return entityTree_.getNumEntities();
    }

    template<class ComponentType>
    inline const Entity* ComponentTreeManager<ComponentType>::begin() const
    {
        return entityTree_.beginEntity();
    }

    template<class ComponentType>
    inline const Entity* ComponentTreeManager<ComponentType>::end() const
    {
        return entityTree_.endEntity();
    }

    template<class ComponentType>
    ComponentType* ComponentTreeManager<ComponentType>::find(const Char* path)
    {
        LASSERT(NULL != path);
        if(lcore::PathDelimiter == *path){
            ++path;
        }
        if(lcore::CharNull == *path){
            return NULL;
        }

        Char name[NameBufferSize];
        s32 length;

        //ノード名をパスから取り出す
        path = lcore::Path::parseFirstNameFromPath(length, name, NameBufferSize, path);
        const ECSNode* current = &entityTree_.getRoot();
        for(;;){
            //一致するノード名の子を検索
            s32 numChildren = current->numChildren();
            u16 child = current->child();

            current = NULL;
            for(s32 i=0; i<numChildren; ++i){
                const ECSNode* childNode = &entityTree_.get(child);
                if(names_[childNode->index()] == name){
                    current = childNode;
                    break;
                }
                child = childNode->next();
            }
            if(NULL == current){
                return NULL;
            }
            if(lcore::CharNull == *path){
                break;
            }
            //次のノード名をパスから取り出す
            path = lcore::Path::parseFirstNameFromPath(length, name, NameBufferSize, path);
        }
        return (current == &entityTree_.getRoot())? NULL : components_[current->index()];
    }

    template<class ComponentType>
    void ComponentTreeManager<ComponentType>::destroyRecurse(u16 id)
    {
        ECSNode& node = entityTree_.get(id);
        while(0<node.numChildren()){
            destroyRecurse(node.child());
        }
        ECSManager& ecsManager = System::getECSManager();
        Entity entity = entityTree_.getEntity(id);
        if(ecsManager.isValidEntity(entity)) {
            ecsManager.destroyEntity(entity);
        }
    }

    template<class ComponentType>
    inline void ComponentTreeManager<ComponentType>::traverse(TraverseCallBack func, void* userData)
    {
        LASSERT(NULL != func);
        traverse(0, &entityTree_.getRoot(), func, userData);
    }

    template<class ComponentType>
    inline void ComponentTreeManager<ComponentType>::traverse(TraverseCallBack funcBegin, TraverseCallBack funcEnd, void* userData)
    {
        LASSERT(NULL != funcBegin);
        LASSERT(NULL != funcEnd);
        traverse(0, &entityTree_.getRoot(), funcBegin, funcEnd, userData);
    }

    template<class ComponentType>
    void ComponentTreeManager<ComponentType>::traverse(s32 depth, const ECSNode* node, TraverseCallBack func, void* userData)
    {
        s32 numChildren = node->numChildren();
        u16 child = node->child();
        for(s32 i=0; i<numChildren; ++i){
            const ECSNode* childNode = &entityTree_.get(child);
            func(depth, components_[childNode->index()], userData);
            traverse(depth+1, childNode, func, userData);
            child = childNode->next();
        }
    }

    template<class ComponentType>
    void ComponentTreeManager<ComponentType>::traverse(s32 depth, const ECSNode* node, TraverseCallBack funcBegin, TraverseCallBack funcEnd, void* userData)
    {
        s32 numChildren = node->numChildren();
        u16 child = node->child();
        for(s32 i=0; i<numChildren; ++i){
            const ECSNode* childNode = &entityTree_.get(child);
            if(funcBegin(depth, components_[childNode->index()], userData)){
                traverse(depth+1, childNode, funcBegin, funcEnd, userData);
                funcEnd(depth, components_[childNode->index()], userData);
            }
            child = childNode->next();
        }
    }
}
#endif //INC_LFRAMEWORK_COMPONENTTREEMANAGER_H_
