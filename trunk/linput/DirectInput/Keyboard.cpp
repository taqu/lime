/**
@file Keyboard.cpp
@author t-sakai
@date 2009/05/13
*/
#include <lcore/lcore.h>
#include <lcore/clibrary.h>

#include "Keyboard.h"

namespace linput
{
    Keyboard::Keyboard()
        :currentBuffer_(0)
        ,oldBuffer_(0)
    {
    }

    Keyboard::~Keyboard()
    {
    }

    bool Keyboard::initialize(HWND__* hWnd, IDirectInputDevice8* device)
    {
        LASSERT(hWnd != NULL);
        LASSERT(device != NULL);

        {
            Device devTmp(device);
            device_.swap(devTmp);
        }

        if(false == device_.setDataFormat(DevType_Keyboard)){
            return false;
        }

        if(false == device_.setCooperateLevel(hWnd)){
            return false;
        }

        {
            DIPROPDWORD dipdw; 
            dipdw.diph.dwSize = sizeof(DIPROPDWORD); 
            dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
            dipdw.diph.dwObj = 0; 
            dipdw.diph.dwHow = DIPH_DEVICE; 

            dipdw.dwData = 8; //バッファリングサイズ
            //HRESULT hr = device_->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
            //if(FAILED(hr)){
            //    return false;
            //}
        }

        for(u32 i=0; i<BUFFER_NUM; ++i){
            lcore::memset(keystate_[i], 0, KEYSTATE_NUM*sizeof(char));
        }

        //アクセス権取得、入力開始
        HRESULT hr = device_->Acquire();
        //if(FAILED(hr)){
        //    return false;
        //}
        return true;
    }

    bool Keyboard::isOn(KeyCode key) const
    {
        LASSERT(0<= key && key < KEYSTATE_NUM);
        return ((keystate_[currentBuffer_][key] & 0x80U) != 0);
    }

    inline bool Keyboard::isOldOn(KeyCode key) const
    {
        LASSERT(0<= key && key < KEYSTATE_NUM);
        return ((keystate_[oldBuffer_][key] & 0x80U) != 0);
    }

    bool Keyboard::isClick(KeyCode key) const
    {
        return !isOn(key) && isOldOn(key);
    }

    void Keyboard::poll()
    {
        // バッファ２個専用
        oldBuffer_ = currentBuffer_;
        currentBuffer_ = (currentBuffer_ ^ 0x01U) & 0x01U;

        //HRESULT hr = device_->Acquire();
        //if(FAILED(hr)){
        //    return;
        //}

        HRESULT hr = device_->GetDeviceState(KEYSTATE_NUM, (LPVOID)&(keystate_[currentBuffer_][0]));
        if(FAILED(hr)){
            device_->Acquire(); //もう一度アクセス権取得
            return;
        }
    }
}
