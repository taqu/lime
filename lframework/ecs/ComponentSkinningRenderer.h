#ifndef INC_LFRAMEWORK_COMPONENTSKINNINGRENDERER_H__
#define INC_LFRAMEWORK_COMPONENTSKINNINGRENDERER_H__
/**
@file ComponentSkinningRenderer.h
@author t-sakai
@date 2016/10/18 create
*/
#include "ComponentRenderer.h"
#include <lmath/Matrix44.h>
#include "render/Model.h"
#include "animation/Skeleton.h"
#include "animation/Animation.h"

namespace lfw
{
    class ComponentSkinningRenderer : public ComponentRenderer
    {
    public:
        ComponentSkinningRenderer();
        virtual ~ComponentSkinningRenderer();

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void postUpdate();
        virtual void onDestroy();
        virtual void addQueue(RenderQueue& queue);
        virtual void drawDepth(RenderContext& renderContext);
        virtual void drawOpaque(RenderContext& renderContext);
        virtual void drawTransparent(RenderContext& renderContext);

        inline Model::pointer& getMesh();
        inline Skeleton::pointer& getSkeleton();
        void setMesh(Model::pointer& model, Skeleton::pointer& skeleton);

        void pushMatrix();


        //Animation
        //--------------------------------------------
        inline void createAnimation(s16 numClips);

        inline void setClip(s16 index, AnimationClip* clip);

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
        ComponentSkinningRenderer(const ComponentSkinningRenderer&);
        ComponentSkinningRenderer& operator=(const ComponentSkinningRenderer&);

        void createMatrices(s32 num);
        void releaseMatrices();
        void calcNodeMatrices();
        void calcPrevNodeMatrices();
        void resetShaderSet();
        void resetFlags();

        Model::pointer model_;
        Skeleton::pointer skeleton_;
        lmath::Matrix44 prevMatrix_;
        Animation animation_;
        s32 numSkinningMatrices_;
        lmath::Matrix34* skinningMatrices_;
        lmath::Matrix34* prevSkinningMatrices_;
    };

    inline Model::pointer& ComponentSkinningRenderer::getMesh()
    {
        return model_;
    }

    inline Skeleton::pointer& ComponentSkinningRenderer::getSkeleton()
    {
        return skeleton_;
    }

    inline void ComponentSkinningRenderer::createAnimation(s16 numClips)
    {
        animation_.create(numClips);
    }

    inline void ComponentSkinningRenderer::setClip(s16 index, AnimationClip* clip)
    {
        AnimationClip::pointer ptr(clip);
        animation_.setClip(index, ptr);
    }

    inline bool ComponentSkinningRenderer::isInPlay(s16 clip) const
    {
        return animation_.isInPlay(clip);
    }

    inline void ComponentSkinningRenderer::setLoop(s16 clip, bool enable)
    {
        animation_.getState(clip).setLoop(enable);
    }

    inline void ComponentSkinningRenderer::play(s16 clip, f32 weight)
    {
        animation_.play(clip, weight);
    }

    inline void ComponentSkinningRenderer::playDelay(s16 clip, f32 delay, f32 weight)
    {
        animation_.playDelay(clip, delay, weight);
    }

    inline void ComponentSkinningRenderer::stop(s16 clip)
    {
        animation_.stop(clip);
    }

    inline void ComponentSkinningRenderer::stopDelay(s16 clip, f32 delay)
    {
        animation_.stopDelay(clip, delay);
    }

    inline void ComponentSkinningRenderer::stopAll()
    {
        animation_.stopAll();
    }

    inline f32 ComponentSkinningRenderer::getTime(s16 clip) const
    {
        return animation_.getState(clip).getTime();
    }

    inline void ComponentSkinningRenderer::setTime(s16 clip, f32 time)
    {
        animation_.getState(clip).setTime(time);
    }

    inline void ComponentSkinningRenderer::fadeIn(s16 clip, f32 time)
    {
        animation_.fadeIn(clip, time);
    }

    inline void ComponentSkinningRenderer::fadeOut(s16 clip, f32 time)
    {
        animation_.fadeOut(clip, time);
    }

    inline void ComponentSkinningRenderer::crossFrade(s16 clip, f32 time)
    {
        animation_.crossFrade(clip, time);
    }
}
#endif //INC_LFRAMEWORK_COMPONENTSKINNINGRENDERER_H__
