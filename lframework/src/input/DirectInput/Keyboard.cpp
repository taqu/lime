/**
@file Keyboard.cpp
@author t-sakai
@date 2009/05/13
*/
#include "input/Keyboard.h"

namespace linput
{
    Keyboard::Keyboard()
    {
    }

    Keyboard::~Keyboard()
    {
    }

    bool Keyboard::initialize(WINDOW_HANDLE hWnd, DirectInputDevice* device)
    {
        LASSERT(hWnd != NULL);
        LASSERT(device != NULL);
        if(NULL == device){
            return false;
        }
        terminate();
        device_ = device;

        if(!setDataFormat(DevType_Keyboard)){
            return false;
        }

        if(!setCooperateLevel(hWnd)){
            return false;
        }

        DIPROPDWORD dipdw;
        dipdw.diph.dwSize = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj = 0;
        dipdw.diph.dwHow = DIPH_DEVICE;

        dipdw.dwData = BufferSize; //バッファリングサイズ
        HRESULT hr = device_->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
        if(FAILED(hr)){
            return false;
        }

        acquire();

        return true;
    }

    void Keyboard::poll()
    {
        LASSERT(NULL != device_);

        DWORD items = BufferSize;
        HRESULT hr = device_->GetDeviceData(
            sizeof(DIDEVICEOBJECTDATA), 
            devObjectData_,
            &items, 
            0); 

        if(SUCCEEDED(hr)){
            clickState_.clear();

            u32 key;
            bool on;
            for(u32 i=0; i<items; ++i){
                key = devObjectData_[i].dwOfs;
                on = ((devObjectData_[i].dwData & 0xF0U) != 0);
                if(on){
                    onState_.set(key);
                }else{
                    onState_.reset(key);
                    clickState_.set(key);
                }
            }

        }else if(hr == DIERR_NOTACQUIRED || hr == DIERR_INPUTLOST){
            acquire(); //もう一度アクセス権取得
        }
    }

    void Keyboard::clear()
    {
        onState_.clear();
        clickState_.clear();
    }

}
