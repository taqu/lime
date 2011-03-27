/**
@file Mouse.cpp
@author t-sakai
@date 2009/09/08
*/
#include "Mouse.h"

namespace linput
{
    Mouse::Mouse()
        :hWnd_(NULL)
    {
    }

    Mouse::~Mouse()
    {
        hWnd_ = NULL;
    }

    bool Mouse::initialize(HWND__* hWnd, IDirectInputDevice8* device)
    {
        LASSERT(hWnd != NULL);
        LASSERT(device != NULL);

        hWnd_ = hWnd;
        {
            Device devTmp(device);
            device_.swap(devTmp);
        }

        getMousePoint();

        if(false == device_.setDataFormat(DevType_Mouse)){
            return false;
        }

        if(false == device_.setCooperateLevel(hWnd_)){
            return false;
        }

        {
            DIPROPDWORD dipdw; 
            dipdw.diph.dwSize = sizeof(DIPROPDWORD); 
            dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
            dipdw.diph.dwObj = 0; 
            dipdw.diph.dwHow = DIPH_DEVICE; 

            dipdw.dwData = DIPROPAXISMODE_REL; //相対値モード
            HRESULT hr = device_->SetProperty(DIPROP_AXISMODE, &dipdw.diph);
            if(FAILED(hr)){
                return false;
            }
        }

        for(int i=0; i<MouseButton_Num; ++i){
            buttonState_[i] = 0;
        }
        clear();

        //アクセス権取得、入力開始
        HRESULT hr = device_->Acquire();
        //if(FAILED(hr)){
        //    return false;
        //}

        return true;
    }

    void Mouse::clear()
    {
        //for(int i=0; i<MouseButton_Num; ++i){
        //    buttonState_[i] = 0;
        //}

        for(int i=0; i<MouseAxis_Num; ++i){
            axisDuration_[i] = 0;
        }
    }

    bool Mouse::isOn(MouseButton button) const
    {
        LASSERT(MouseButton_0<= button && button < MouseButton_Num);
        return buttonState_[button] & 0x01U;
    }

    bool Mouse::isClick(MouseButton button) const
    {
        LASSERT(MouseButton_0<= button && button < MouseButton_Num);

        return (buttonState_[button] & 0x02U) != 0;
    }

    s32 Mouse::getDuration(MouseAxis axis) const
    {
        LASSERT(0<= axis && axis < MouseAxis_Num);
        return axisDuration_[axis];
    }

    inline void Mouse::getMousePoint()
    {
        GetCursorPos((LPPOINT)&mousePoint_);
        ScreenToClient(hWnd_, (LPPOINT)&mousePoint_);
    }

    void Mouse::poll()
    {
        getMousePoint();

        DIMOUSESTATE2 state;

        clear();

        //HRESULT hr = device_->Acquire();
        //if(FAILED(hr)){
        //    return;
        //}

        HRESULT hr = device_->GetDeviceState(sizeof(DIMOUSESTATE2), &state);
        if(FAILED(hr)){
            device_->Acquire(); //もう一度アクセス権取得
            return;
        }

        axisDuration_[MouseAxis_X] += state.lX;
        axisDuration_[MouseAxis_Y] += state.lY;
        axisDuration_[MouseAxis_Z] += state.lZ;

        for(u32 i=0; i<MouseButton_Num; ++i){
            bool on = ((state.rgbButtons[i] & 0x80U) != 0);
            if(on){
                buttonState_[i] = 0x01U;
            }else{
                buttonState_[i] = (buttonState_[i] == 1)? 2 : 0;
            }
        }
    }
}
