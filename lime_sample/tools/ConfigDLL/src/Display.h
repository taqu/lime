#ifndef INC_DISPLAY_H__
#define INC_DISPLAY_H__
/**
@file Display.h
@author t-sakai
@date 2011/08/14 create
*/
#include "types.h"
#include <d3d9.h>

namespace config
{
    extern D3DFORMAT toD3DFormat[Buffer_Num];

    struct DisplayMode
    {
        u32 width_;
        u32 height_;
        u32 refreshRate_;
        u32 format_;
    };

    static const u32 AdapterDefault = 0;

    class Display
    {
    public:
        Display();
        ~Display();

        bool create();
        void release();

        u32 getAdapterCount();
        u32 getAdapterModeCount(u32 adapter, BufferFormat format);
        bool enumAdapterModes(u32 adapter, BufferFormat format, u32 mode, DisplayMode* modes);

        bool checkDeviceType(u32 adapter, BufferFormat format, BufferFormat backFormat, bool hal, s32 windowed);

        bool getAdapterDisplayMode(u32 adapter, DisplayMode& mode);
    private:
        Display(const Display&);
        Display& operator=(const Display&);

        friend class DisplayList;

        IDirect3D9 *dx9_;
    };

}
#endif //INC_DISPLAY_H__
