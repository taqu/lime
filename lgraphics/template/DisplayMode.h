#ifndef INC_LGRAPHICS_DX11_DISPLAYMODE_H__
#define INC_LGRAPHICS_DX11_DISPLAYMODE_H__
/**
@file DisplayMode.h
@author t-sakai
@date 2017/01/09 create
*/
#include <lgraphics/Display.h>

struct IDXGIFactory1;
struct IDXGIAdapter1;
struct IDXGIOutput;

namespace lgfx
{
    //--------------------------------------------
    //---
    //---
    //---
    //--------------------------------------------
    bool getDisplayMode(HINSTANCE hInstance, DXGIDisplayMode& dst, bool& windowed, const DXGIDisplayMode& request);
}

#endif //INC_LGRAPHICS_DX11_DISPLAYMODE_H__
