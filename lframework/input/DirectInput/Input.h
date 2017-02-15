#ifndef INC_LFW_LINPUT_DINPUT_INPUT_H__
#define INC_LFW_LINPUT_DINPUT_INPUT_H__
/**
@file Input.h
@author t-sakai
@date 2016/11/07 create
*/
#include "../linput.h"

namespace linput
{
    class Device;

    class Input
    {
    public:
        Input();
        ~Input();

        /**
        */
        Error initialize(const InitParam& param);
        bool initialize(linput::DeviceType type, WINDOW_HANDLE hWnd);
        void terminate();

        void update();
        void clear();

        const Keyboard* getKeyboard() const{ return reinterpret_cast<const Keyboard*>( devices_[DevType_Keyboard] );}
        const Mouse* getMouse() const{ return reinterpret_cast<const Mouse*>( devices_[DevType_Mouse] );}
        const Joystick* getJoystick() const{ return reinterpret_cast<const Joystick*>( devices_[DevType_Joystick] );}

    private:
        Input(const Input&);
        Input& operator=(const Input&);

        DirectInputDevice* createDevice(const _GUID& deviceID);

        DirectInput* directInput_;
        Device* devices_[DevType_Num];
    };
}
#endif //INC_LFW_LINPUT_DINPUT_INPUT_H__
