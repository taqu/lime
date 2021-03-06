﻿/**
@file Mouse.cpp
@author t-sakai
@date 2009/09/08
*/
#include "input/Mouse.h"

namespace linput
{
    Mouse::Mouse()
        :hWnd_(NULL)
        ,width_(0)
        ,height_(0)
    {
    }

    Mouse::~Mouse()
    {
        hWnd_ = NULL;
    }

    bool Mouse::initialize(WINDOW_HANDLE hWnd, DirectInputDevice* device)
    {
        LASSERT(hWnd != NULL);
        LASSERT(device != NULL);
        if(NULL == device){
            return false;
        }

        terminate();
        device_ = device;
        hWnd_ = hWnd;

        getMousePoint();
        resize();

        if(!setDataFormat(DevType_Mouse)){
            return false;
        }

        if(!setCooperateLevel(hWnd_)){
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

            dipdw.dwData = BufferSize; //バッファリングサイズ
            hr = device_->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
            if(FAILED(hr)){
                return false;
            }
        }

        acquire();

        return true;
    }

    inline void Mouse::getMousePoint()
    {
        GetCursorPos((LPPOINT)&mousePoint_);
        ScreenToClient(hWnd_, (LPPOINT)&mousePoint_);
    }

    void Mouse::poll()
    {
        LASSERT(NULL != device_);

        s32 prevX = mousePoint_.value_[0];
        s32 prevY = mousePoint_.value_[1];
        getMousePoint();

        DWORD items = BufferSize;
        HRESULT hr = device_->GetDeviceData(
            sizeof(DIDEVICEOBJECTDATA), 
            devObjectData_,
            &items, 
            0); 

        if(SUCCEEDED(hr)){
            clickState_ = 0;
            for(int i=0; i<MouseAxis_Num; ++i){
                axisDuration_[i] = 0;
            }

            u32 button;
            bool on;
            for(u32 i=0; i<items; ++i){
                switch(devObjectData_[i].dwOfs)
                {
                case DIMOFS_X:
                    axisDuration_[MouseAxis_X] += *(LONG*)&devObjectData_[i].dwData;
                    break;
                case DIMOFS_Y:
                    axisDuration_[MouseAxis_Y] += *(LONG*)&devObjectData_[i].dwData;
                    break;
                case DIMOFS_Z:
                    axisDuration_[MouseAxis_Z] += *(LONG*)&devObjectData_[i].dwData;
                    break;

                case DIMOFS_BUTTON0:
                case DIMOFS_BUTTON1:
                case DIMOFS_BUTTON2:
                case DIMOFS_BUTTON3:
                    button = devObjectData_[i].dwOfs - DIMOFS_BUTTON0;
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

            if(mousePoint_.value_[0] < 0){
                mousePoint_.value_[0] = 0;
                onState_ = 0;
                clickState_ = 0;
                axisDuration_[MouseAxis_X] = mousePoint_.value_[0] - prevX;
            }else if(width_<=mousePoint_.value_[0]){
                mousePoint_.value_[0] = width_ - 1;
                onState_ = 0;
                clickState_ = 0;
                axisDuration_[MouseAxis_X] = mousePoint_.value_[0] - prevX;
            }

            if(mousePoint_.value_[1] < 0){
                mousePoint_.value_[1] = 0;
                onState_ = 0;
                clickState_ = 0;
                axisDuration_[MouseAxis_Y] = mousePoint_.value_[1] - prevY;
            }else if(height_<=mousePoint_.value_[1]){
                mousePoint_.value_[1] = height_ - 1;
                onState_ = 0;
                clickState_ = 0;
                axisDuration_[MouseAxis_Y] = mousePoint_.value_[1] - prevY;
            }
        }else if(hr == DIERR_NOTACQUIRED || hr == DIERR_INPUTLOST){
            acquire(); //もう一度アクセス権取得
        }
    }

    void Mouse::clear()
    {
        onState_ = 0;
        clickState_ = 0;
        for(int i=0; i<MouseAxis_Num; ++i){
            axisDuration_[i] = 0;
        }
    }

    void Mouse::resize()
    {
        RECT rect;
        if(FALSE == GetClientRect(hWnd_, &rect)){
            return;
        }
        width_ = rect.right - rect.left;
        height_ = rect.bottom - rect.top;
    }

    inline void Mouse::setOn(u32 index)
    {
        onState_ |= (0x01U<<index);
    }

    inline void Mouse::resetOn(u32 index)
    {
        onState_ &= ~(0x01U<<index);
    }

    inline void Mouse::setClick(u32 index)
    {
        clickState_ |= (0x01U<<index);
    }
}
