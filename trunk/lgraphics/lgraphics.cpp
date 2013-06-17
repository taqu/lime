/**
@file lgraphics.cpp
@author t-sakai
@date 2009/01/19 create
*/
#include "lgraphics.h"

namespace lgraphics
{
    GraphicsDeviceRef Graphics::device_;

    bool Graphics::initialize(const InitParam& initParam)
    {
        bool isInit = device_.initialize(initParam);
        return isInit;
    }

    void Graphics::terminate()
    {
        device_.terminate();
    }

}
