#ifndef INC_LSCENE_NODEANIMOBJECT_H__
#define INC_LSCENE_NODEANIMOBJECT_H__
/**
@file NodeAnimObject.h
@author t-sakai
@date 2014/12/29 create
*/
#include "lscene.h"
#include "NodeTransform.h"
#include "anim/Animation.h"

namespace lgraphics
{
    class ContextRef;

    class ConstantBufferRef;

    class VertexShaderRef;
    class GeometryShaderRef;
    class PixelShaderRef;
}

namespace lscene
{
namespace lrender
{
    class AnimObject;
    class Material;
}

    class NodeAnimObject : public lscene::NodeTransform
    {
    public:
        explicit NodeAnimObject(const Char* name = NULL, u16 group=Group_None, u16 type=NodeType_AnimObject);
        virtual ~NodeAnimObject();

        lrender::AnimObject* getAnimObject();
        virtual void setAnimObject(lrender::AnimObject* obj);

        virtual void resetAttributes();
        virtual void resetShaders();

        virtual void update();
        virtual void visitRenderQueue(lscene::RenderContext& renderContext);
        virtual void render(lscene::RenderContext& renderContext);

        //Animation
        //--------------------------------------------
        inline void createAnimation(s16 numClips);
        inline void setSkeleton(lanim::Skeleton::pointer skeleton);

        inline void setClip(s16 index, lanim::AnimationClip* clip);

        inline bool isInPlay(s16 clip) const;
        inline void setLoop(s16 clip, bool enable);
        inline void play(s16 clip, f32 weight=1.0f);
        inline void playDelay(s16 clip, f32 delay, f32 weight=1.0f);
        inline void stop(s16 clip);
        inline void stopDelay(s16 clip, f32 delay);
        inline void stopAll();

        inline f32 getTime(s16 clip) const;
        inline void setTime(s16 clip, f32 time);

        inline void fadeIn(s16 clip, f32 time);
        inline void fadeOut(s16 clip, f32 time);

        inline void crossFrade(s16 clip, f32 time);
    protected:

        void calcNodeMatrices();
        void renderDepth(lscene::RenderContext& renderContext);
        void renderSolid(lscene::RenderContext& renderContext);
        void renderTransparent(lscene::RenderContext& renderContext);

        inline void setTexture(lgraphics::ContextRef& context, lrender::Material& material);
 
        inline void setSkinningMatrices(lscene::RenderContext& renderContext);

        virtual void createMatrices(s32 num);
        virtual void releaseMatrices();

        lrender::AnimObject* object_;
        lanim::Animation animation_;
        s32 numSkinningMatrices_;
        lmath::Matrix34* skinningMatrices_;
    };

    inline void NodeAnimObject::createAnimation(s16 numClips)
    {
        animation_.create(numClips);
    }

    inline void NodeAnimObject::setSkeleton(lanim::Skeleton::pointer skeleton)
    {
        animation_.setSkeleton(skeleton);
    }

    inline void NodeAnimObject::setClip(s16 index, lanim::AnimationClip* clip)
    {
        animation_.setClip(index, clip);
    }

    inline bool NodeAnimObject::isInPlay(s16 clip) const
    {
        return animation_.isInPlay(clip);
    }

    inline void NodeAnimObject::setLoop(s16 clip, bool enable)
    {
        animation_.getState(clip).setLoop(enable);
    }

    inline void NodeAnimObject::play(s16 clip, f32 weight)
    {
        animation_.play(clip, weight);
    }

    inline void NodeAnimObject::playDelay(s16 clip, f32 delay, f32 weight)
    {
        animation_.playDelay(clip, delay, weight);
    }

    inline void NodeAnimObject::stop(s16 clip)
    {
        animation_.stop(clip);
    }

    inline void NodeAnimObject::stopDelay(s16 clip, f32 delay)
    {
        animation_.stopDelay(clip, delay);
    }

    inline void NodeAnimObject::stopAll()
    {
        animation_.stopAll();
    }

    inline f32 NodeAnimObject::getTime(s16 clip) const
    {
        return animation_.getState(clip).getTime();
    }

    inline void NodeAnimObject::setTime(s16 clip, f32 time)
    {
        animation_.getState(clip).setTime(time);
    }

    inline void NodeAnimObject::fadeIn(s16 clip, f32 time)
    {
        animation_.fadeIn(clip, time);
    }

    inline void NodeAnimObject::fadeOut(s16 clip, f32 time)
    {
        animation_.fadeOut(clip, time);
    }

    inline void NodeAnimObject::crossFrade(s16 clip, f32 time)
    {
        animation_.crossFrade(clip, time);
    }
}
#endif //INC_LSCENE_NODEANIMOBJECT_H__
