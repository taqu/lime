/**
@file Joystick.cpp
@author t-sakai
@date 2012/09/30 create
*/
#include "input/Joystick.h"

namespace linput
{
    namespace
    {
        BOOL CALLBACK EnumAxes(LPCDIDEVICEOBJECTINSTANCE instance, LPVOID context)
        {
            DirectInputDevice* device = (DirectInputDevice*)context;
            //遊び
            DIPROPDWORD dipdw; 
            dipdw.diph.dwSize = sizeof(DIPROPDWORD); 
            dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
            dipdw.diph.dwObj = instance->dwType; 
            dipdw.diph.dwHow = DIPH_BYID;
            dipdw.dwData = 600;
            device->SetProperty(DIPROP_DEADZONE , &dipdw.diph);

            //最大
            dipdw.dwData = 9500;
            device->SetProperty(DIPROP_SATURATION , &dipdw.diph);

            //軸の範囲設定
            DIPROPRANGE diprange;
            diprange.diph.dwSize = sizeof(DIPROPRANGE); 
            diprange.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
            diprange.diph.dwObj = instance->dwType;
            diprange.diph.dwHow = DIPH_BYID;
            diprange.lMin = -1000;
            diprange.lMax = 1000;
            device->SetProperty(DIPROP_RANGE, &diprange.diph);

            return DIENUM_CONTINUE;
        }
    }

    Joystick::Joystick()
    {
    }

    Joystick::~Joystick()
    {
    }

    bool Joystick::initialize(WINDOW_HANDLE hWnd, DirectInputDevice* device)
    {
        LASSERT(hWnd != NULL);
        LASSERT(device != NULL);
        if(NULL == device){
            return false;
        }

        terminate();
        device_ = device;


        if(!setDataFormat(DevType_Joystick)){
            return false;
        }

        if(!setCooperateLevel(hWnd)){
            return false;
        }

        //軸プロパティ設定
        device_->EnumObjects(EnumAxes, (LPVOID)device, DIDFT_AXIS);

        DIPROPDWORD dipdw; 
        dipdw.diph.dwSize = sizeof(DIPROPDWORD); 
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        dipdw.diph.dwObj = 0; 
        dipdw.diph.dwHow = DIPH_DEVICE; 

        dipdw.dwData = DIPROPAXISMODE_ABS; // 絶対値モード
        HRESULT hr = device_->SetProperty(DIPROP_AXISMODE, &dipdw.diph);
        if(FAILED(hr)){
            return false;
        }

        dipdw.dwData = BufferSize; //バッファリングサイズ
        hr = device_->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
        if(FAILED(hr)){
            return false;
        }

        acquire();

        return true;
    }

    void Joystick::clear()
    {
        for(int i=0; i<JoystickButton_Num; ++i){
            onState_[i] = false;
        }
        for(int i=0; i<JoystickAxis_Num; ++i){
            axisPosition_[i] = 0;
            axisDuration_[i] = 0;
        }

        for(int i=0; i<JoystickButton_Num; ++i){
            clickState_[i] = false;
        }
    }

    void Joystick::poll()
    {
        LASSERT(NULL != device_);
        DWORD items = BufferSize;
        HRESULT hr = device_->GetDeviceData(
            sizeof(DIDEVICEOBJECTDATA), 
            devObjectData_,
            &items, 
            0); 

        if(SUCCEEDED(hr)){
            for(int i=0; i<JoystickButton_Num; ++i){
                clickState_[i] = false;
            }

            LONG oldPosition[JoystickAxis_Num] = {axisPosition_[JoystickAxis_X], axisPosition_[JoystickAxis_Y]};

            u32 button;
            bool on;
            for(u32 i=0; i<items; ++i){
                switch(devObjectData_[i].dwOfs)
                {
                case DIJOFS_X :
                    axisPosition_[JoystickAxis_X] = *(LONG*)&devObjectData_[i].dwData;
                    break;
                case DIJOFS_Y:
                    axisPosition_[JoystickAxis_Y] = *(LONG*)&devObjectData_[i].dwData;
                    break;

                case DIJOFS_BUTTON0:
                case DIJOFS_BUTTON1:
                case DIJOFS_BUTTON2:
                case DIJOFS_BUTTON3:
                case DIJOFS_BUTTON4:
                case DIJOFS_BUTTON5:
                    button = devObjectData_[i].dwOfs - DIJOFS_BUTTON0;
                    on = ((devObjectData_[i].dwData & 0xF0U) != 0);
                    if(on){
                        setOn(button);
                    }else{
                        resetOn(button);
                        setClick(button);
                    }
                    break;
                }
            }

            for(int i=0; i<JoystickAxis_Num; ++i){
                axisDuration_[i] = axisPosition_[i] - oldPosition[i];
            }

        }else if(hr == DIERR_NOTACQUIRED || hr == DIERR_INPUTLOST){
            acquire(); //もう一度アクセス権取得
        }
    }

    inline void Joystick::setOn(u32 index)
    {
        onState_[index] = true;
    }

    inline void Joystick::resetOn(u32 index)
    {
        onState_[index] = false;
    }

    inline void Joystick::setClick(u32 index)
    {
        clickState_[index] = true;
    }
}
