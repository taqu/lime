#ifndef INC_EGDA_CONFIG_H__
#define INC_EGDA_CONFIG_H__
/**
@file Config.h
@author t-sakai
@date 2011/03/31 create
*/
namespace lmath
{
    class Matrix44;
}

namespace egda
{
    enum ScreenMode
    {
        ScreenMode_Rot0 =0,
        ScreenMode_Rot90,
        ScreenMode_Rot180,
        ScreenMode_Rot270,
    };


    class Config
    {
    public:
        static Config& getInstance(){ return instance_;}
        static void initialize();

        inline void setScreenMode(ScreenMode screenMode);
        ScreenMode getScreeMode() const{ return screenMode_;}

        inline void setViewportSize(s32 width, s32 height);

        void setAspect(f32 aspect){ aspect_ = aspect;}
        f32 getAspect() const{ return aspect_;}

        void setZNear(f32 znear){ znear_ = znear;}
        f32 getZNear() const{ return znear_;}

        void setZFar(f32 zfar){ zfar_ = zfar;}
        f32 getZFar() const{ return zfar_;}

        void perspectiveFovLinearZ(lmath::Matrix44& mat, f32 fovy);
    private:
        inline void updateAspect();

        static Config instance_;

        ScreenMode screenMode_;
        f32 width_;
        f32 height_;
        f32 aspect_;
        f32 znear_;
        f32 zfar_;
    };

    inline void Config::setScreenMode(ScreenMode screenMode)
    {
        screenMode_ = screenMode;
        updateAspect();
    }

    inline void Config::setViewportSize(s32 width, s32 height)
    {
        instance_.width_ = static_cast<f32>(width);
        instance_.height_ = static_cast<f32>(height);
        updateAspect();
    }

    inline void Config::updateAspect()
    {
        //モードによってアスペクト比が違う
        switch(screenMode_)
        {
        case ScreenMode_Rot90:
        case ScreenMode_Rot270:
            {
                instance_.aspect_ = (instance_.height_/instance_.width_);
            }
            break;

        default:
            {
                instance_.aspect_ = (instance_.width_/instance_.height_);
            }
            break;
        };
    }
}
#endif //INC_EGDA_CONFIG_H__
