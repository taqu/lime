/**
@file Mouse.cpp
@author t-sakai
@date 2009/09/08
*/
#include "../linputAPIInclude.h"
#include "../Mouse.h"

namespace linput
{
    Mouse::Mouse()
    {
    }

    Mouse::~Mouse()
    {
        device_.terminate();
        data_.hWnd_ = NULL;
    }

    bool Mouse::initialize(Device::InitParam& param)
    {
        device_.initialize(param);
        if(false == device_.isInit()){
            return false;
        }

        getMousePoint();

        //create(GUID_SysMouse);

        if(false == device_.setDataFormat(DevType_Mouse)){
            return false;
        }

        if(false == device_.setCooperateLevel(param.hWnd_)){
            return false;
        }

        if(false == device_.setProperty()){
            return false;
        }

        for(int i=0; i<MouseButton_Num; ++i){
            buttonState_[i] = 0;
        }
        clear();

        data_.hWnd_ = param.hWnd_;
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

    s32 Mouse::getX() const
    {
        return mousePoint_.x_;
    }

    s32 Mouse::getY() const
    {
        return mousePoint_.y_;
    }

    inline void Mouse::getMousePoint()
    {
        GetCursorPos((LPPOINT)&mousePoint_);
        ScreenToClient(data_.hWnd_, (LPPOINT)&mousePoint_);
    }

    void Mouse::poll()
    {
        getMousePoint();

        DIMOUSESTATE state;

        clear();


        IDirectInputDevice8 *device = device_.get();
        HRESULT hr = device->Acquire();
        if(FAILED(hr)){
            return;
        }

        hr = device->GetDeviceState(sizeof(DIMOUSESTATE), &state);
        if(FAILED(hr)){
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
