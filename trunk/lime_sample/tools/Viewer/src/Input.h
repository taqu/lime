#ifndef INC_VIEWER_INPUT_H__
#define INC_VIEWER_INPUT_H__
/**
@file Input.h
@author t-sakai
@date 2011/02/18 create
*/
struct HWND__;

namespace viewer
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
            Axis_Yaw =0,
            Axis_Pitch,
            Axis_Roll,
            Axis_Num,
        };

        static void initialize(HWND__* hWnd, u32 viewWidth, u32 viewHeight);
        static void terminate();

        static void acquire();
        static void unacquire();

        static void update();

        static f32 getAnalogDuration(Analog analog);
        static f32 getAnalogPosition(Analog analog);

        static bool isOn(Button button);
        static bool isClick(Button button);

        static bool isOn(Key key);
        static bool isClick(Key key);

        static f32 getAngle(AngularSensorAxis axis);
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
            f32 angle_[Axis_Num];
        };

        static Impl impl_;
    };
}

#endif //INC_VIEWER_INPUT_H__
