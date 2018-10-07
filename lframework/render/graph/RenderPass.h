#ifndef INC_LFW_RENDERPASS_H_
#define INC_LFW_RENDERPASS_H_
/**
@file RenderPass.h
@author t-sakai
@date 2017/04/16 create
*/
#include "../../lframework.h"

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

        inline s32 getID() const;
        inline void setID(s32 id);

    protected:
        RenderPass();
        explicit RenderPass(s32 id);

        RenderPass(const RenderPass&) = delete;
        RenderPass& operator=(const RenderPass&) = delete;

        s32 id_;
    };

    inline s32 RenderPass::getID() const
    {
        return id_;
    }

    inline void RenderPass::setID(s32 id)
    {
        id_ = id;
    }
}
}
#endif //INC_LFW_RENDERPASS_H_
