/**
@file Keyboard.cpp
@author t-sakai
@date 2009/05/13
*/
#include <cstring>
#include "../linputAPIInclude.h"
#include "../Keyboard.h"

namespace linput
{
    Keyboard::Keyboard()
        :currentBuffer_(0),
        oldBuffer_(0)
    {
    }

    Keyboard::~Keyboard()
    {
        device_.terminate();
    }

    bool Keyboard::initialize(Device::InitParam& param)
    {
        device_.initialize(param);
        if(false == device_.isInit()){
            return false;
        }

        //create(GUID_SysKeyboard);

        if(false == device_.setDataFormat(DevType_Keyboard)){
            return false;
        }

        if(false == device_.setCooperateLevel(param.hWnd_)){
            return false;
        }

        for(u32 i=0; i<BUFFER_NUM; ++i){
            std::memset(keystate_[i], 0, KEYSTATE_NUM*sizeof(char));
        }
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

        IDirectInputDevice8 *device = device_.get();

        HRESULT hr = device->Acquire();
        if(FAILED(hr)){
            return;
        }

        hr = device->GetDeviceState(KEYSTATE_NUM, (LPVOID)&(keystate_[currentBuffer_][0]));
        if(FAILED(hr)){
            return;
        }
    }
}
