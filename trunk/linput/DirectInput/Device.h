#ifndef INC_LINPUT_DX_DEVICE_H__
#define INC_LINPUT_DX_DEVICE_H__
/**
@file Device.h
@author t-sakai
@date 2009/05/13 create
*/
#include "../linputEnum.h"
struct HWND__;
struct IDirectInputDevice8;

namespace linput
{
    class Device
    {
    public:
        struct InitParam
        {
            HWND__ *hWnd_;
            IDirectInputDevice8* device_;
        };

        Device();
        ~Device();

        bool isInit() const{ return (device_ != NULL);}
        void initialize(InitParam& param);
        void terminate();

        IDirectInputDevice8* get(){ return device_;}

        bool setCooperateLevel(HWND__* hWnd);
        bool setDataFormat(DeviceType type);
        bool setProperty();
    private:
        IDirectInputDevice8 *device_;
    };
}

#endif //INC_LINPUT_DX_DEVICE_H__
