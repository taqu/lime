#ifndef INC_LINPUT_DEVICEBASE_H__
#define INC_LINPUT_DEVICEBASE_H__
/**
@file DeviceBase.h
@author t-sakai
@date 2011/02/18 create

*/
#include "Device.h"

namespace linput
{
    class DeviceBase
    {
    public:
        virtual ~DeviceBase(){}

        virtual void poll() = 0;

        bool acquire();
        void unacquire();
    protected:
        Device device_;
    };
}
#endif //INC_LINPUT_DEVICEBASE_H__
