/**
@file Texture2D.cpp
@author t-sakai
@date 2015/10/01 create
*/
#include "texture/Texture2D.h"
#include "core/Intersection.h"
#include "texture/Wrap2D.h"

namespace lrender
{
    Texture2D::Texture2D()
    {
    }

    Texture2D::Texture2D(Image& image)
    {
        image_.swap(image);
    }

    Texture2D::~Texture2D()
    {
    }

    void Texture2D::setImage(Image& image)
    {
        image_.swap(image);
    }

    Color3 Texture2D::sample(const Intersection& intersection) const
    {
        if(intersection.hasDifferentials()){
            //f32 u = intersection.uv_.x_;
            //f32 v = intersection.uv_.y_;
            //f32 dudx = intersection.dudx_;
            //f32 dvdx = intersection.dvdx_;
            //f32 dudy = intersection.dudy_;
            //f32 dvdy = intersection.dvdy_;
            return sample(intersection.uv_);
        } else{
            return sample(intersection.uv_);
        }
    }

    Color3 Texture2D::sample(const Vector2& uv) const
    {
        f32 u,v;
        Wrap2D::wrap(u, v, uv, wrapType_);
        return image_.sampleLinearRGB(u, v);
    }

    void Texture2D::getResolution(s32 resolution[3]) const
    {
        resolution[0] = image_.getWidth();
        resolution[1] = image_.getHeight();
        resolution[2] = 1;
    }

    Color3 Texture2D::avarage() const
    {
        Color4 c = image_.average();        
        return Color3(c[0], c[1], c[2]);
    }

    //Color3 Texture2D::triangle(const Vector2& uv)
    //{
    //    f32 s = uv.x_ * image_.getWidth() - 0.5f;
    //    f32 t = uv.y_ * image_.getHeight() - 0.5f;
    //    s32 s0 = lmath::floorS32(s);
    //    s32 t0 = lmath::floorS32(t);
    //    f32 ds = s-s0;
    //    f32 dt = t-t0;
    //    image_.
    //}

    Color3 Texture2D::sample(const Vector2& uv, Vector2 /*d0*/, Vector2 /*d1*/) const
    {
        f32 u,v;
        Wrap2D::wrap(u, v, uv, wrapType_);
        return image_.sampleLinearRGB(u, v);
        //f32 majorLength = d0.length();
        //f32 minorLength = d1.length();
        //if(majorLength<minorLength){
        //    lcore::swap(d0, d1);
        //    lcore::swap(majorLength, minorLength);
        //}

        //const f32 maxAnisotropy = 1.0f;
        //if(minorLength*maxAnisotropy<majorLength && 0.0f<minorLength){
        //    f32 scale = majorLength/(minorLength*maxAnisotropy);
        //    d1 *= scale;
        //    minorLength *= scale;
        //}
    }
}
