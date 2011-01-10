#ifndef INC_LGRAPHICS_DX11_GRAPHICSDEVICE_H__
#define INC_LGRAPHICS_DX11_GRAPHICSDEVICE_H__
/**
@file GraphicsDevice.h
@author t-sakai
@date 2010/03/23 create
*/
#include "../lgraphicscore.h"

struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;

namespace lgraphics
{
    /**
    @brief Direct3D11 グラフィクスデバイス
    */
    class GraphicsDevice
    {
    public:

    private:
        friend class Graphics;

        GraphicsDevice();
        ~GraphicsDevice();

        bool initialize(const InitParam& initParam);
        void terminate();


        ID3D11Device *device_; /// デバイス実態
        ID3D11DeviceContext *context_; /// デバイスコンテキスト
        IDXGISwapChain *swapChain_; /// スワップチェイン
    };
}
#endif //INC_LGRAPHICS_DX11_GRAPHICSDEVICE_H__
