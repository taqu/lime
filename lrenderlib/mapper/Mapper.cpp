/**
@file Mapper.cpp
@author t-sakai
@date 2015/10/05 create
*/
#include "Mapper.h"
#include "texture/Wrap2D.h"

namespace lrender
{
    //--------------------------------------------------------------------
    //---
    //--- SamplerBase
    //---
    //--------------------------------------------------------------------
    void Mapper::SamplerBase::sorty()
    {
        if(p_[1][1] < p_[0][1]){
            lcore::swap(p_[0], p_[1]);
        }
        if(p_[2][1] < p_[0][1]){
            lcore::swap(p_[0], p_[2]);
        }
        if(p_[2][1] < p_[1][1]){
            lcore::swap(p_[1], p_[2]);
        }
    }

    void Mapper::SamplerBase::barycentricCoordinate(
        f32& b0, f32& b1, f32& b2,
        s32 x, s32 y,
        const Vector2& p0,
        const Vector2& p1,
        const Vector2& p2)
    {
        Vector2 p(0.5f+x, 0.5f+y);

        f32 t00 = p0.x_-p2.x_;
        f32 t01 = p1.x_-p2.x_;
        f32 t10 = p0.y_-p2.y_;
        f32 t11 = p1.y_-p2.y_;
        f32 determinant = t00*t11 - t10*t01;
        if(lmath::isZero(determinant)){
            b0 = 1.0f;
            b1 = 0.0f;
            b2 = 0.0f;
            return;
        }

        f32 invDet = 1.0f/determinant;
        b0 = ((p1.y_-p2.y_)*(p.x_-p2.x_) + (p2.x_-p1.x_)*(p.y_-p2.y_))*invDet;
        b1 = ((p2.y_-p0.y_)*(p.x_-p2.x_) + (p0.x_-p2.x_)*(p.y_-p2.y_))*invDet;
        b2 = 1.0f-b0-b1;
    }


    //--------------------------------------------------------------------
    //---
    //--- Sampler
    //---
    //--------------------------------------------------------------------
    Mapper::Sampler::Sampler()
    {
    }

    Mapper::Sampler::~Sampler()
    {
    }

    bool Mapper::Sampler::initialize(s32 width, s32 height)
    {
        phase_ = 0;
        sorty();

        xmin_ = width;
        xmax_ = 0;
        ymin_ = height;
        ymax_ = 0;
        for(s32 i=0; i<3; ++i){
            s32 x = lcore::clamp(static_cast<s32>(p_[i].x_+0.5f), 0, width);
            s32 y = lcore::clamp(static_cast<s32>(p_[i].y_+0.5f), 0, height);
            xmin_ = lcore::minimum(xmin_, x);
            xmax_ = lcore::maximum(xmax_, x);
            ymin_ = lcore::minimum(ymin_, y);
            ymax_ = lcore::maximum(ymax_, y);
        }
        return nextPhase();
    }

    bool Mapper::Sampler::nextPhase()
    {
        switch(phase_)
        {
        case 0:
            {
                ++phase_;
                s32 y0 = lcore::clamp(static_cast<s32>(p_[0].y_+0.5f), ymin_, ymax_);
                s32 y1 = lcore::clamp(static_cast<s32>(p_[1].y_+0.5f), ymin_, ymax_);
                if(y0 == y1){
                    return nextPhase();
                }
                y_ = yminSpan_ = y0;
                ymaxSpan_ = y1;

                q_[0] = q_[1] = p_[0];

                f32 dy = (p_[1].y_ - p_[0].y_);
                f32 idy = (lmath::isZero(dy))? 0.0f : 1.0f/dy;
                dxdy0_ = (p_[1].x_-p_[0].x_)*idy;

                dy = (p_[2].y_ - p_[0].y_);
                idy = (lmath::isZero(dy))? 0.0f : 1.0f/dy;
                dxdy1_ = (p_[2].x_-p_[0].x_)*idy;
                f32 p2x = (p_[0].x_ + dxdy1_*(p_[1].y_-p_[0].y_));
                Vector2 p2 = Vector2(p2x, p_[1].y_);

                if(p2.x_<p_[1].x_){
                    lcore::swap(dxdy0_, dxdy1_);
                }

                nextXSpan();
            }
            return true;

        case 1:
            {
                ++phase_;
                s32 y0 = lcore::clamp(static_cast<s32>(p_[1].y_+0.5f), ymin_, ymax_);
                s32 y1 = lcore::clamp(static_cast<s32>(p_[2].y_+0.5f), ymin_, ymax_);
                if(y0 == y1){
                    return false;
                }
                y_ = yminSpan_ = y0;
                ymaxSpan_ = y1;

                q_[1] = p_[1];

                f32 dy = (p_[2].y_ - p_[0].y_);
                f32 idy = (lmath::isZero(dy))? 0.0f : 1.0f/dy;
                f32 p0x = (p_[0].x_ + (p_[2].x_-p_[0].x_)*idy*(p_[1].y_-p_[0].y_));
                q_[0] = Vector2(p0x, p_[1].y_);

                dy = (p_[2].y_ - p_[1].y_);
                idy = (lmath::isZero(dy))? 0.0f : 1.0f/dy;
                dxdy0_ = (p_[2].x_-q_[0].x_)*idy;
                dxdy1_ = (p_[2].x_-p_[1].x_)*idy;

                if(q_[1].x_<q_[0].x_){
                    lcore::swap(dxdy0_, dxdy1_);
                    lcore::swap(q_[0], q_[1]);
                }
                nextXSpan();
            }
            return true;

        default:
            return false;
        }
    }

    inline void Mapper::Sampler::nextXSpan()
    {
        s32 dy = y_ - yminSpan_;
        xminSpan_ = lcore::clamp(static_cast<s32>(q_[0].x_ + dxdy0_*dy+0.5f), xmin_, xmax_);
        xmaxSpan_ = lcore::clamp(static_cast<s32>(q_[1].x_ + dxdy1_*dy+0.5f), xmin_, xmax_);
        x_ = xminSpan_;
    }

    bool Mapper::Sampler::next(MapperSample& sample)
    {
        if(xmaxSpan_<=x_){
            ++y_;
            if(ymaxSpan_<=y_){
                return nextPhase();
            }
            nextXSpan();

            sample.texcoordu_ = x_;
            sample.texcoordv_ = y_;
            barycentricCoordinate(
                sample.b0_, sample.b1_, sample.b2_,
                sample.texcoordu_, sample.texcoordv_,
                p_[0],
                p_[1],
                p_[2]);
            return true;
        }

        sample.texcoordu_ = x_++;
        sample.texcoordv_ = y_;

        barycentricCoordinate(
            sample.b0_, sample.b1_, sample.b2_,
            sample.texcoordu_, sample.texcoordv_,
            p_[0],
            p_[1],
            p_[2]);
        return true;
    }


    //--------------------------------------------------------------------
    //---
    //--- ConservativeSampler
    //---
    //--------------------------------------------------------------------
    Mapper::ConservativeSampler::ConservativeSampler()
    {
    }

    Mapper::ConservativeSampler::~ConservativeSampler()
    {
    }

    bool Mapper::ConservativeSampler::initialize(s32 width, s32 height)
    {
        phase_ = 0;
        sorty();

        xmin_ = width;
        xmax_ = 0;
        ymin_ = height;
        ymax_ = 0;
        for(s32 i=0; i<3; ++i){
            s32 xmin = lcore::clamp(lmath::floorS32(p_[i].x_), 0, width);
            s32 ymin = lcore::clamp(lmath::floorS32(p_[i].y_), 0, height);
            s32 xmax = lcore::clamp(lmath::ceilS32(p_[i].x_), 0, width);
            s32 ymax = lcore::clamp(lmath::ceilS32(p_[i].y_), 0, height);
            xmin_ = lcore::minimum(xmin_, xmin);
            xmax_ = lcore::maximum(xmax_, xmax);
            ymin_ = lcore::minimum(ymin_, ymin);
            ymax_ = lcore::maximum(ymax_, ymax);
        }
        return nextPhase();
    }

    bool Mapper::ConservativeSampler::nextPhase()
    {
        switch(phase_)
        {
        case 0:
            {
                ++phase_;
                s32 y0 = lcore::clamp(lmath::floorS32(p_[0].y_), ymin_, ymax_);
                s32 y1 = lcore::clamp(lmath::ceilS32(p_[1].y_), ymin_, ymax_);
                if(y0 == y1){
                    return nextPhase();
                }
                y_ = yminSpan_ = y0;
                ymaxSpan_ = y1;

                q_[0] = q_[1] = p_[0];

                f32 dy = (p_[1].y_ - p_[0].y_);
                f32 idy = (lmath::isZero(dy))? 0.0f : 1.0f/dy;
                dxdy0_ = (p_[1].x_-p_[0].x_)*idy;

                dy = (p_[2].y_ - p_[0].y_);
                idy = (lmath::isZero(dy))? 0.0f : 1.0f/dy;
                dxdy1_ = (p_[2].x_-p_[0].x_)*idy;
                f32 p2x = (p_[0].x_ + dxdy1_*(p_[1].y_-p_[0].y_));
                Vector2 p2 = Vector2(p2x, p_[1].y_);

                if(p2.x_<p_[1].x_){
                    lcore::swap(dxdy0_, dxdy1_);
                }

                nextXSpan();
            }
            return true;

        case 1:
            {
                ++phase_;
                s32 y0 = lcore::clamp(lmath::floorS32(p_[1].y_), ymin_, ymax_);
                s32 y1 = lcore::clamp(lmath::ceilS32(p_[2].y_), ymin_, ymax_);
                if(y0 == y1){
                    return false;
                }
                y_ = yminSpan_ = y0;
                ymaxSpan_ = y1;

                q_[1] = p_[1];

                f32 dy = (p_[2].y_ - p_[0].y_);
                f32 idy = (lmath::isZero(dy))? 0.0f : 1.0f/dy;
                f32 p0x = (p_[0].x_ + (p_[2].x_-p_[0].x_)*idy*(p_[1].y_-p_[0].y_));
                q_[0] = Vector2(p0x, p_[1].y_);

                dy = (p_[2].y_ - p_[1].y_);
                idy = (lmath::isZero(dy))? 0.0f : 1.0f/dy;
                dxdy0_ = (p_[2].x_-q_[0].x_)*idy;
                dxdy1_ = (p_[2].x_-p_[1].x_)*idy;

                if(q_[1].x_<q_[0].x_){
                    lcore::swap(dxdy0_, dxdy1_);
                    lcore::swap(q_[0], q_[1]);
                }
                nextXSpan();
            }
            return true;

        default:
            return false;
        }
    }

    inline void Mapper::ConservativeSampler::nextXSpan()
    {
        s32 dy = y_ - yminSpan_;
        xminSpan_ = lcore::clamp(lmath::floorS32(q_[0].x_ + dxdy0_*dy), xmin_, xmax_);
        xmaxSpan_ = lcore::clamp(lmath::ceilS32(q_[1].x_ + dxdy1_*dy), xmin_, xmax_);
        x_ = xminSpan_;
    }

    bool Mapper::ConservativeSampler::next(MapperSample& sample)
    {
        if(xmaxSpan_<=x_){
            ++y_;
            if(ymaxSpan_<=y_){
                return nextPhase();
            }
            nextXSpan();

            sample.texcoordu_ = x_;
            sample.texcoordv_ = y_;
            barycentricCoordinate(
                sample.b0_, sample.b1_, sample.b2_,
                sample.texcoordu_, sample.texcoordv_,
                p_[0],
                p_[1],
                p_[2]);
            return true;
        }

        sample.texcoordu_ = x_++;
        sample.texcoordv_ = y_;

        barycentricCoordinate(
            sample.b0_, sample.b1_, sample.b2_,
            sample.texcoordu_, sample.texcoordv_,
            p_[0],
            p_[1],
            p_[2]);
        return true;
    }


    //--------------------------------------------------------------------
    //---
    //--- Mapper
    //---
    //--------------------------------------------------------------------
    Mapper::Mapper()
        :wrapType_(Texture::WrapType_Clamp)
    {
    }

    Mapper::~Mapper()
    {
    }

    bool Mapper::generateSampler(
        Sampler& sampler,
        s32 width,
        s32 height,
        const Vector2& t0,
        const Vector2& t1,
        const Vector2& t2)
    {
        sampler.p_[0] = Wrap2D::wrap(t0, wrapType_);
        sampler.p_[1] = Wrap2D::wrap(t1, wrapType_);
        sampler.p_[2] = Wrap2D::wrap(t2, wrapType_);
        
        for(s32 i=0; i<3; ++i){
            sampler.p_[i].x_ *= width;
            sampler.p_[i].y_ *= height;
        }
        return sampler.initialize(width, height);
    }

    bool Mapper::generateSampler(
        ConservativeSampler& sampler,
        s32 width,
        s32 height,
        const Vector2& t0,
        const Vector2& t1,
        const Vector2& t2)
    {
        sampler.p_[0] = Wrap2D::wrap(t0, wrapType_);
        sampler.p_[1] = Wrap2D::wrap(t1, wrapType_);
        sampler.p_[2] = Wrap2D::wrap(t2, wrapType_);
        
        for(s32 i=0; i<3; ++i){
            sampler.p_[i].x_ *= width;
            sampler.p_[i].y_ *= height;
        }
        return sampler.initialize(width, height);
    }

    //void Mapper::drawTriangle(
    //    const Vector3& p0,
    //    const Vector3& p1,
    //    const Vector3& p2,
    //    const Vector2& t0,
    //    const Vector2& t1,
    //    const Vector2& t2)
    //{
    //    Vector2 tv[3] ={t0, t1, t2};
    //    Sampler sampler;
    //    for(s32 i=0; i<3; ++i){
    //        sampler.p_[i] = Wrap2D::wrap(tv[i], wrapType_);
    //        sampler.p_[i].x_ *= image_.getWidth();
    //        sampler.p_[i].y_ *= image_.getHeight();
    //    }
    //    if(!sampler.initialize(image_.getWidth(), image_.getHeight())){
    //        return;
    //    }
    //    Sample sample;
    //    while(sampler.next(sample)){
    //        image_.set(sample.u_, sample.v_, sample.w_, sample.texcoord_.x_, sample.texcoord_.y_);
    //    }
    //}
}
