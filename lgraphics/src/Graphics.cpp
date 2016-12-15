/**
@file Graphics.cpp
@author t-sakai
@date 2016/11/12 create
*/
#include "Graphics.h"

namespace lgfx
{
    GraphicsDeviceRef Graphics::device_;

    bool initializeGraphics(const InitParam& initParam)
    {
        return Graphics::device_.initialize(initParam);
    }

    void terminateGraphics()
    {
        Graphics::device_.terminate();
    }
}
