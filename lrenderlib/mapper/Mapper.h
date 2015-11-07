#ifndef INC_LRENDER_MAPPER_H__
#define INC_LRENDER_MAPPER_H__
/**
@file Mapper.h
@author t-sakai
@date 2015/10/05 create
*/
#include "../lrender.h"
#include "texture/Texture.h"

namespace lrender
{
    class Mapper
    {
    public:
        class SamplerBase
        {
        public:

        protected:
            friend class Mapper;

            void sorty();

            static void barycentricCoordinate(
                f32& b0, f32& b1, f32& b2,
                s32 x, s32 y,
                const Vector2& p0,
                const Vector2& p1,
                const Vector2& p2);

            s32 phase_;
            Vector2 p_[3];
            Vector2 q_[2];
            f32 dxdy0_;
            f32 dxdy1_;

            s32 xmin_;
            s32 xmax_;
            s32 ymin_;
            s32 ymax_;
            s32 xminSpan_;
            s32 xmaxSpan_;
            s32 yminSpan_;
            s32 ymaxSpan_;
            s32 x_, y_;
        };

        class Sampler : public SamplerBase
        {
        public:
            Sampler();
            ~Sampler();
            bool next(MapperSample& sample);

        private:
            friend class Mapper;

            bool initialize(s32 width, s32 height);
            bool nextPhase();

            inline void nextXSpan();
        };

        class ConservativeSampler : public SamplerBase
        {
        public:
            ConservativeSampler();
            ~ConservativeSampler();
            bool next(MapperSample& sample);

        private:
            friend class Mapper;

            bool initialize(s32 width, s32 height);
            bool nextPhase();

            inline void nextXSpan();
        };

        Mapper();
        ~Mapper();

        bool generateSampler(
            Sampler& sampler,
            s32 width,
            s32 height,
            const Vector2& t0,
            const Vector2& t1,
            const Vector2& t2);

        bool generateSampler(
            ConservativeSampler& sampler,
            s32 width,
            s32 height,
            const Vector2& t0,
            const Vector2& t1,
            const Vector2& t2);

        //void drawTriangle(
        //    const Vector3& p0,
        //    const Vector3& p1,
        //    const Vector3& p2,
        //    const Vector2& t0,
        //    const Vector2& t1,
        //    const Vector2& t2);

        Texture::WrapType wrapType_;
    };
}
#endif //INC_LRENDER_MAPPER_H__
