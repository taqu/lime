#ifndef INC_LFW_RENDERPASS_H__
#define INC_LFW_RENDERPASS_H__
/**
@file RenderPass.h
@author t-sakai
@date 2017/04/16 create
*/
#include "lframework.h"

namespace lfw
{
    class Camera;
    class RenderContext;

namespace graph
{
    //---------------------------------------------------------
    //---
    //--- RenderPass
    //---
    //---------------------------------------------------------
    class RenderPass
    {
    public:
        virtual ~RenderPass();

        virtual void create(const Camera& camera) =0;
        virtual void destroy() =0;
        virtual void execute(RenderContext& renderContext, Camera& camera) =0;

        inline u32 getID() const;
        inline void setID(u32 id);

    protected:
        RenderPass();

        RenderPass(const RenderPass&) = delete;
        RenderPass& operator=(const RenderPass&) = delete;

        u32 id_;
    };

    inline u32 RenderPass::getID() const
    {
        return id_;
    }

    inline void RenderPass::setID(u32 id)
    {
        id_ = id;
    }
}
}
#endif //INC_LFW_RENDERPASS_H__
