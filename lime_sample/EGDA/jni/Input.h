#ifndef INC_EGDA_INPUT_H__
#define INC_EGDA_INPUT_H__
/**
@file Input.h
@author t-sakai
@date 2011/02/18 create
*/

namespace egda
{
    class Input
    {
    public:
        enum Analog
        {
            Analog_X =0,
            Analog_Y,
            Analog_Num,
        };

        enum Button
        {
            Button_0 =0,
            Button_1,
            Button_2,
            Button_3,
            Button_Num,
        };

        enum Key
        {
            Key_0 =0, //stop or play
            Key_1,    //change camera mode
            Key_Num,
        };

        enum AngularSensorAxis
        {
            Axis_X =0,
            Axis_Y,
            Axis_Z,
            Axis_Num,
        };

        static void initialize(s32 viewWidth, s32 viewHeight);

        static f32 getAnalogDuration(Analog analog);
        static f32 getAnalogPosition(Analog analog);

        static bool isOn(Button button);
        static bool isClick(Button button);

        static bool isOn(Key key);
        static bool isClick(Key key);

        static f32 getAngle(AngularSensorAxis axis);

        inline static f32* getPositions(){ return impl_.positions_;}
        inline static f32* getDurations(){ return impl_.durations_;}
        inline static f32* getAngles(){ return impl_.angles_;}

        inline static bool* getOnFlags(){ return impl_.isOn_;}
        inline static bool* getClickFlags(){ return impl_.isClick_;}

        inline static void updateOrientation(f32 x, f32 y, f32 z);
        inline static void updateTouch(bool isOn, f32 x, f32 y);
    private:
        class Impl
        {
        public:
            s32 width_;
            s32 height_;
            s32 halfWidth_;
            s32 halfHeight_;
            f32 invHalfWidth_;
            f32 invHalfHeight_;

            f32 positions_[Analog_Num];
            f32 durations_[Analog_Num];
            f32 angles_[Axis_Num];
            bool isOn_[Button_Num];
            bool isClick_[Button_Num];
        };

        static Impl impl_;
    };

    inline void Input::updateOrientation(f32 x, f32 y, f32 z)
    {
        impl_.angles_[Axis_X] = x;
        impl_.angles_[Axis_Y] = y;
        impl_.angles_[Axis_Z] = z;
    }

    inline void Input::updateTouch(bool isOn, f32 x, f32 y)
    {
        impl_.durations_[Analog_X] = 0.0f;
        impl_.durations_[Analog_Y] = 0.0f;

        if(impl_.isOn_[Button_1]){
            impl_.isClick_[Button_1] = !isOn;

            if(isOn){
                impl_.durations_[Analog_X] = x - impl_.positions_[Analog_X];
                impl_.durations_[Analog_Y] = y - impl_.positions_[Analog_Y];
            }
        }

        impl_.isOn_[Button_1] = isOn;
        impl_.positions_[Analog_X] = x;
        impl_.positions_[Analog_Y] = y;
    }
}

#endif //INC_EGDA_INPUT_H__
