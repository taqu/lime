#ifndef INC_EGDA_CAMERA_H__
#define INC_EGDA_CAMERA_H__
/**
@file Camera.h
@author t-sakai
@date 2011/02/18 create
*/
namespace pmm
{
    class CameraAnimPack;
}

namespace egda
{
    //------------------------------------------------
    //---
    //--- Camera
    //---
    //------------------------------------------------
    class Camera
    {
    public:
        static const f32 TickZoomRatio;
        static const f32 TickRotationRatio;
        
        enum Mode
        {
            Mode_Manual =0,
            Mode_FrameAnim,
            Mode_Num,
        };

        Camera();
        ~Camera();

        inline Mode getMode() const;
        inline void setMode(Mode mode);
        inline void changeMode();

        inline void update(u32 counter);
        inline void reset();
        inline void reset(Mode mode);

        void resetProjection();

        void setInitial(const lmath::Vector3& position, const lmath::Vector3& target, f32 fov);
        void setCameraAnim(pmm::CameraAnimPack* pack);
    private:
        Camera(const Camera&);
        Camera& operator=(const Camera&);

        class Manual;
        class FrameAnim;

        class Base
        {
        public:
            virtual ~Base(){}
            virtual void initialize() = 0;
            virtual void update(u32 counter) =0;
            virtual void reset() =0;
        };

        Mode mode_;
        Base* impl_[Mode_Num];
    };

    inline Camera::Mode Camera::getMode() const
    {
        return mode_;
    }

    inline void Camera::setMode(Mode mode)
    {
        LASSERT(0<=mode && mode<Mode_Num);
        mode_ = mode;
    }

    inline void Camera::changeMode()
    {
        s32 mode = mode_;
        if(++mode>=Mode_Num){
            mode = 0;
        }
        mode_ = static_cast<Mode>(mode);
    }

    inline void Camera::update(u32 counter)
    {
        impl_[mode_]->update(counter);
    }

    inline void Camera::reset()
    {
        for(s32 i=0; i<Mode_Num; ++i){
            impl_[i]->reset();
        }
    }

    inline void Camera::reset(Mode mode)
    {
        LASSERT(0<=mode && mode<Mode_Num);
        impl_[mode]->reset();
    }
}
#endif //INC_EGDA_CAMERA_H__
