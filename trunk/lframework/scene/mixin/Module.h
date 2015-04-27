#ifndef INC_LSCENE_MIXIN_MODULE_H__
#define INC_LSCENE_MIXIN_MODULE_H__
/**
@file Module.h
@author t-sakai
@date 2014/10/30 create
*/
#include "../lscene.h"
#include "ReferenceCounted.h"

namespace lscene
{
    class NodeBase;
    class RenderQueue;
    class RenderContext;

namespace mixin
{
    class Module : public ReferenceCounted<ModuleAllocator>
    {
    public:
        virtual ~Module()
        {}

        virtual void onAttach(NodeBase& node) =0;
        virtual void onDetach(NodeBase& node) =0;
        virtual void update(NodeBase& node) =0;
        virtual void visitRenderQueue(NodeBase& node, RenderQueue& queue) =0;
        virtual void render(RenderContext& context) =0;
    protected:
        Module()
        {}
    };

    template<class Base>
    class NodeModule : public Base
    {
    public:
        typedef lscene::vector_arena<Module*, 2> ModuleVector;

        explicit NodeModule(const Char* name = NULL);
        virtual ~NodeModule();

        void addModule(Module* module);
        void removeModule(Module* module);
        void clearModules();

        virtual void update();
        virtual void visitRenderQueue(RenderQueue& queue);

        virtual void render(RenderContext& context);
    protected:
        NodeModule(const NodeModule&);
        NodeModule& operator=(const NodeModule&);

        inline void updateModules();
        inline void visitRenderQueueModules(RenderQueue& queue);
        inline void renderModules(RenderContext& context);

        ModuleVector modules_;
    };

    template<class Base>
    NodeModule<Base>::NodeModule(const Char* name)
        :NodeBase(name)
    {
    }

    template<class Base>
    NodeModule<Base>::~NodeModule()
    {
        clearModules();
    }

    template<class Base>
    void NodeModule<Base>::addModule(Module* module)
    {
        LASSERT(NULL != module);
        if(modules_.find(module)){
            return;
        }
        module->addRef();
        modules_.push_back(module);
        module->onAttach(*this);
    }

    template<class Base>
    void NodeModule<Base>::removeModule(Module* module)
    {
        LASSERT(NULL != module);
        for(ModuleVector::size_type i=0; i<modules_.size(); ++i){
            if(module == modules_[i]){
                module->onDetach(*this);
                modules_.removeAt(i);
                module->release();
                break;
            }
        }
    }

    template<class Base>
    void NodeModule<Base>::clearModules()
    {
        for(ModuleVector::iterator itr = modules_.begin();
            itr != modules_.end();
            ++itr)
        {
            (*itr)->onDetach(*this);
            (*itr)->release();
        }
        modules_.clear();
    }

    template<class Base>
    void NodeModule<Base>::update()
    {
        if(checkFlag(NodeFlag_Update)){
            updateModules();
            updateChildren(bsphere_);
        }
    }

    template<class Base>
    void NodeModule<Base>::visitRenderQueue(RenderQueue& queue)
    {
        if(checkFlag(NodeFlag_Render)){
            visitRenderQueueModules(queue);
            visitRenderQueueChildren(queue);
        }
    }

    template<class Base>
    void NodeModule<Base>::render(RenderContext& context)
    {
        renderModules(context);
    }

    template<class Base>
    inline void NodeModule<Base>::updateModules()
    {
        for(ModuleVector::iterator itr = modules_.begin();
            itr != modules_.end();
            ++itr)
        {
            (*itr)->update(*this);
        }
    }

    template<class Base>
    inline void NodeModule<Base>::visitRenderQueueModules(RenderQueue& queue)
    {
        for(ModuleVector::iterator itr = modules_.begin();
            itr != modules_.end();
            ++itr)
        {
            (*itr)->visitRenderQueue(*this, queue);
        }
    }

    template<class Base>
    inline void NodeModule<Base>::renderModules(RenderContext& context)
    {
        for(ModuleVector::iterator itr = modules_.begin();
            itr != modules_.end();
            ++itr)
        {
            (*itr)->render(context);
        }
    }
}
}
#endif //INC_LSCENE_MIXIN_MODULE_H__
