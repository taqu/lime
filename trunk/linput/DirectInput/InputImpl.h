#ifndef INC_LINPUT_DX_INPUTIMPL_H__
#define INC_LINPUT_DX_INPUTIMPL_H__
/**
@file InputImpl.h
@author t-sakai
@date 2010/07/22 create

*/
struct _GUID;
struct HWND__;
struct IDirectInput8;
struct IDirectInputDevice8;

namespace linput
{
    class InputImpl
    {
    public:
        InputImpl();
        ~InputImpl();

        bool isInit() const{ return (impl_ != NULL);}
        void initialize(HWND__* hWnd);
        void terminate();

        IDirectInputDevice8 *createDevice(const _GUID& deviceID);
    private:
        // コピー禁止
        InputImpl(const InputImpl&);
        InputImpl& operator=(const InputImpl&);

        IDirectInput8 *impl_;
    };
}
#endif //INC_LINPUT_DX_INPUTIMPL_H__
