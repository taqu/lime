#ifndef INC_LGRAPHICS_GRAPHICS_H__
#define INC_LGRAPHICS_GRAPHICS_H__
/**
@file Graphics.h
@author t-sakai
@date 2016/11/12 create
*/
#include "lgraphics.h"
#include "GraphicsDeviceRef.h"

namespace lgfx
{
    class Graphics
    {
    public:

    private:
        Graphics(const Graphics&);
        Graphics& operator=(const Graphics&);

        friend bool initializeGraphics(const InitParam& initParam);
        friend void terminateGraphics();
        friend GraphicsDeviceRef& getDevice();

        static GraphicsDeviceRef device_;
    };

    bool initializeGraphics(const InitParam& initParam);
    void terminateGraphics();
    inline GraphicsDeviceRef& getDevice()
    {
        return Graphics::device_;
    }
}
#endif //INC_LGRAPHICS_GRAPHICS_H__
