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

            dipdw.dwData = BufferSize; //バッファリングサイズ
            hr = device_->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
            if(FAILED(hr)){
                return false;
            }
        }

        acquire();

        return true;
    }

    void Mouse::clear()
    {
        onState_ = 0;
        clickState_ = 0;
        for(int i=0; i<MouseAxis_Num; ++i){
            axisDuration_[i] = 0;
        }
    }

    void Mouse::acquire()
    {
        clear();

        //アクセス権取得、入力開始
        device_->Acquire();
    }

    inline void Mouse::getMousePoint()
    {
        GetCursorPos((LPPOINT)&mousePoint_);
        ScreenToClient(hWnd_, (LPPOINT)&mousePoint_);
    }

    void Mouse::poll()
    {
        getMousePoint();

        DIMOUSESTATE state;

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

        }else if(hr == DIERR_NOTACQUIRED || hr == DIERR_INPUTLOST){
            acquire(); //もう一度アクセス権取得
        }
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
