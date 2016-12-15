#ifndef INC_LFW_LINPUT_DINPUT_DEVICE_H__
#define INC_LFW_LINPUT_DINPUT_DEVICE_H__
/**
@file Device.h
@author t-sakai
@date 2009/05/13 create
*/
#include "../linput.h"

namespace linput
{
    class Device
    {
    public:
        virtual ~Device();

        inline bool valid() const;
        void terminate();

        inline DirectInputDevice* get();
        inline DirectInputDevice* operator->();

        bool setCooperateLevel(WINDOW_HANDLE hWnd);
        bool setDataFormat(DeviceType type);
        bool setEventNotification(HANDLE handle);

        //アクセス権取得、入力開始
        bool acquire();
        bool unacquire();

        virtual void poll() =0;
        virtual void clear() =0;
    protected:
        Device(const Device&);
        Device& operator=(const Device&);

        Device();

        DirectInputDevice* device_;
    };

    inline bool Device::valid() const
    {
        return (device_ != NULL);
    }

    inline DirectInputDevice* Device::get()
    {
        return device_;
    }

    inline DirectInputDevice* Device::operator->()
    {
        LASSERT(device_ != NULL);
        return device_;
    }

}

#endif //INC_LFW_LINPUT_DINPUT_DEVICE_H__
