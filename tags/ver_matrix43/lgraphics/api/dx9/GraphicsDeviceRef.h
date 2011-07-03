#ifndef INC_LGRAPHICS_DX9_GRAPHICSDEVICEREF_H__
#define INC_LGRAPHICS_DX9_GRAPHICSDEVICEREF_H__
/**
@file GraphicsDeviceRef.h
@author t-sakai
@date 2009/01/18 create
*/
#include "../../lgraphicscore.h"
#include "Enumerations.h"

//---------------------------------
struct IDirect3DDevice9;
struct IDirect3D9;

struct IDirect3DSurface9;

struct IDirect3DBaseTexture9;
struct IDirect3DTexture9;

struct IDirect3DVertexDeclaration9;

struct IDirect3DIndexBuffer9;
struct IDirect3DVertexBuffer9;

struct IDirect3DPixelShader9;
struct IDirect3DVertexShader9;

struct IDirect3DStateBlock9;

namespace lmath
{
    class Matrix44;
}

namespace lgraphics
{
    class Graphics;
    struct InitParam;
    class RenderState;

    class GraphicsDeviceRef
    {
    public:
        static const u32 Default_AlphaTestRef = 128;


        GraphicsDeviceRef(const GraphicsDeviceRef& rhs);

        GraphicsDeviceRef& operator=(const GraphicsDeviceRef& rhs)
        {
            GraphicsDeviceRef tmp(rhs);
            tmp.swap(*this);
            return *this;
        }

        bool valid() const{ return (d3d_ != NULL && d3dDevice_ != NULL);}

        inline IDirect3DDevice9* getD3DDevice();

        inline void beginScene();
        inline void endScene();

        inline void clear(u32 target);
        inline void setClearColor(u32 color);
        inline void setClearDepth(f32 depth);
        inline void setClearStencil(u32 stencil);

        inline void present(const RECT* srcRect, const RECT* dstRect, HWND dstWindow);

        void setViewport(s32 x, s32 y, u32 width, u32 height);

        inline void setTransform(TransformType type, const lmath::Matrix44& matrix);


        // Draw系
        //-----------------------------------------------------------------------------
        inline void draw(PrimitiveType type, u32 primCount, u32 offsetVertex = 0);
        inline void drawIndexed(PrimitiveType type, u32 vertexNum, u32 primCount, u32 offsetIndex = 0);
        inline void drawIndexed(PrimitiveType type, u32 baseVertexIndex, u32 minVertexIndex, u32 vertexNum, u32 offsetIndex, u32 primCount);

        inline void drawUP(PrimitiveType type, u32 primCount, const void* vertices, u32 stride);
        inline void drawIndexedUP(
            PrimitiveType type,
            u32 minIndex,
            u32 vertexNum,
            u32 primCount,
            const void* indices,
            BufferFormat indexFormat,
            const void* vertices,
            u32 stride);

        // Render Target
        //-------------------------------------------------------------------------
        inline IDirect3DSurface9* createRenderTarget(
            u32 width,
            u32 height,
            BufferFormat format,
            MutiSampleType multiSample,
            u32 multiSampleQuality,
            bool lockable);

        inline bool getRenderTarget(u32 index, IDirect3DSurface9** target);

        inline bool getRenderTargetData(IDirect3DSurface9* src, IDirect3DSurface9* dst);
        inline void setRenderTarget(u32 index, IDirect3DSurface9* target);
        inline bool getFrontBufferData(u32 swapChain, IDirect3DSurface9* surface);
        inline IDirect3DSurface9* getBackBuffer(u32 swapChain, u32 backBuffer);

        inline IDirect3DSurface9* createOffscreenPlaneSurface(
            u32 width,
            u32 height,
            BufferFormat format);

        // Texture
        //-------------------------------------------------------------------------------
        inline IDirect3DTexture9* createTexture(u32 width, u32 height, u32 levels, Usage usage, BufferFormat format, Pool poolType);
        inline void setTexture(u32 sampler, IDirect3DBaseTexture9* texture);
        inline void updateTexture(IDirect3DBaseTexture9* src, IDirect3DBaseTexture9* dst);

        // Vertex Declaration
        //-------------------------------------------------------------------------------
        inline IDirect3DVertexDeclaration9* createVertexDeclaration(const D3DVERTEXELEMENT9* vertexElements);
        inline void setVertexDeclaration(IDirect3DVertexDeclaration9* decl);


        // Buffers
        //----------------------------------------------------------------------------
        inline IDirect3DIndexBuffer9* createIndexBuffer(
            u32 length,
            u32 usage,
            BufferFormat format,
            Pool pool);

        inline void setIndices(IDirect3DIndexBuffer9* indices);

        inline IDirect3DVertexBuffer9* createVertexBuffer(
            u32 length,
            u32 usage,
            u32 fvf,
            Pool pool);

        inline void setStreamSource(u32 streamNumber, IDirect3DVertexBuffer9* vb, u32 offset, u32 stride);
        inline void setStreamSourceFreq(u32 streamNumber, u32 frequency);


        // Shaders
        //------------------------------------------------------------------
        inline IDirect3DPixelShader9* createPixelShader(const u32* function);
        inline void setPixelShader(IDirect3DPixelShader9* shader);

        inline IDirect3DVertexShader9* createVertexShader(const u32* function);
        inline void setVertexShader(IDirect3DVertexShader9* shader);

        inline void setAlphaBlendEnable(bool enable);

        // RenderState
        //------------------------------------------------------------------
        inline IDirect3DStateBlock9* createStateBlock(StateType type);
        inline bool beginStateBlock();
        inline void endStateBlock(IDirect3DStateBlock9** stateBlock);

        inline void setRenderState(u32 type, u32 value);
        inline u32 getRenderState(u32 type);

        // SamplerState
        //------------------------------------------------------------------
        inline void setSamplerState(u32 sampler, u32 type, u32 value);
        inline u32 getSamplerState(u32 sampler, u32 type);

        void swap(GraphicsDeviceRef& rhs)
        {
            lcore::swap(d3d_, rhs.d3d_);
            lcore::swap(d3dDevice_, rhs.d3dDevice_);

            lcore::swap(_clearColor,  rhs._clearColor);
            lcore::swap(_clearDepth,  rhs._clearDepth);
            lcore::swap(_clearStencil, rhs._clearStencil);
        }
    private:
        friend class Graphics;

        GraphicsDeviceRef();
        ~GraphicsDeviceRef();

        bool initialize(const InitParam& initParam);
        void terminate();

        static const u32 FULL_SCREEN_REFRESH_RATE = 60;
        IDirect3D9* d3d_;
        IDirect3DDevice9 *d3dDevice_;

        u32 _clearColor;
        f32 _clearDepth;
        u32 _clearStencil;
    };


    inline IDirect3DDevice9* GraphicsDeviceRef::getD3DDevice()
    {
        return d3dDevice_;
    }

    inline void GraphicsDeviceRef::beginScene()
    {
        d3dDevice_->BeginScene();
    }

    inline void GraphicsDeviceRef::endScene()
    {
        d3dDevice_->EndScene();
    }

    inline void GraphicsDeviceRef::clear(u32 target)
    {
        d3dDevice_->Clear(0, NULL, target, _clearColor, _clearDepth, _clearStencil);
    }

    inline void GraphicsDeviceRef::setClearColor(u32 color)
    {
        _clearColor = color;
    }

    inline void GraphicsDeviceRef::setClearDepth(f32 depth)
    {
        _clearDepth = depth;
    }

    inline void GraphicsDeviceRef::setClearStencil(u32 stencil)
    {
        _clearStencil = stencil;
    }

    inline void GraphicsDeviceRef::present(const RECT* srcRect, const RECT* dstRect, HWND dstWindow)
    {
        d3dDevice_->Present(srcRect, dstRect, dstWindow, NULL);
    }

    inline void GraphicsDeviceRef::setTransform(TransformType type, const lmath::Matrix44& matrix)
    {
        d3dDevice_->SetTransform((D3DTRANSFORMSTATETYPE)type, (const D3DMATRIX*)&matrix);
    }

    // Draw系
    //------------------------------------------------------------------------------------------
    inline void GraphicsDeviceRef::draw(PrimitiveType type, u32 primCount, u32 offsetVertex)
    {
        d3dDevice_->DrawPrimitive((D3DPRIMITIVETYPE)type, offsetVertex, primCount);
    }

    inline void GraphicsDeviceRef::drawIndexed(PrimitiveType type, u32 vertexNum, u32 primCount, u32 offsetIndex)
    {
        d3dDevice_->DrawIndexedPrimitive((D3DPRIMITIVETYPE)type, 0, 0, vertexNum, offsetIndex, primCount);
    }

    inline void GraphicsDeviceRef::drawIndexed(PrimitiveType type, u32 baseVertexIndex, u32 minVertexIndex, u32 vertexNum, u32 offsetIndex, u32 primCount)
    {
        d3dDevice_->DrawIndexedPrimitive((D3DPRIMITIVETYPE)type, baseVertexIndex, minVertexIndex, vertexNum, offsetIndex, primCount);
    }

    inline void GraphicsDeviceRef::drawUP(PrimitiveType type, u32 primCount, const void* vertices, u32 stride)
    {
        d3dDevice_->DrawPrimitiveUP((D3DPRIMITIVETYPE)type, primCount, vertices, stride);
    }

    inline void GraphicsDeviceRef::drawIndexedUP(
            PrimitiveType type,
            u32 minIndex,
            u32 vertexNum,
            u32 primCount,
            const void* indices,
            BufferFormat indexFormat,
            const void* vertices,
            u32 stride)
    {
        d3dDevice_->DrawIndexedPrimitiveUP(
            (D3DPRIMITIVETYPE)type,
            minIndex,
            vertexNum,
            primCount,
            indices,
            (D3DFORMAT)indexFormat,
            vertices,
            stride);
    }


    // Render Target
    //------------------------------------------------------------------------------------------
    inline bool GraphicsDeviceRef::getRenderTarget(u32 index, IDirect3DSurface9** target)
    {
        HRESULT hr = d3dDevice_->GetRenderTarget(index, target);
        return SUCCEEDED(hr);
    }

    inline IDirect3DSurface9* GraphicsDeviceRef::createRenderTarget(
        u32 width,
        u32 height,
        BufferFormat format,
        MutiSampleType multiSample,
        u32 multiSampleQuality,
        bool lockable)
    {
        IDirect3DSurface9 *surface = NULL;
        HRESULT hr = d3dDevice_->CreateRenderTarget(
            width,
            height,
            (D3DFORMAT)format,
            (D3DMULTISAMPLE_TYPE)multiSample,
            multiSampleQuality,
            lockable,
            &surface,
            NULL);

        return (FAILED(hr))? NULL : surface;
    }

    inline bool GraphicsDeviceRef::getRenderTargetData(IDirect3DSurface9* src, IDirect3DSurface9* dst)
    {
        LASSERT(src != NULL);
        LASSERT(dst != NULL);
        HRESULT hr = d3dDevice_->GetRenderTargetData(src, dst);
        return SUCCEEDED(hr);
    }

    inline void GraphicsDeviceRef::setRenderTarget(u32 index, IDirect3DSurface9* target)
    {
        LASSERT(target != NULL);
        d3dDevice_->SetRenderTarget(index, target);
    }

    inline bool GraphicsDeviceRef::getFrontBufferData(u32 swapChain, IDirect3DSurface9* surface)
    {
        LASSERT(surface != NULL);
        HRESULT hr = d3dDevice_->GetFrontBufferData(swapChain, surface);
        return SUCCEEDED(hr);
    }

    inline IDirect3DSurface9* GraphicsDeviceRef::getBackBuffer(u32 swapChain, u32 backBuffer)
    {
        IDirect3DSurface9 *surface = NULL;
        HRESULT hr = d3dDevice_->GetBackBuffer(swapChain, backBuffer, D3DBACKBUFFER_TYPE_MONO, &surface);
        return (FAILED(hr))? NULL : surface;
    }


    inline IDirect3DSurface9* GraphicsDeviceRef::createOffscreenPlaneSurface(
            u32 width,
            u32 height,
            BufferFormat format)
    {
        IDirect3DSurface9 *surface = NULL;
        HRESULT hr = d3dDevice_->CreateOffscreenPlainSurface(
            width,
            height,
            (D3DFORMAT)format,
            (D3DPOOL)Pool_Scratch,
            &surface,
            NULL);

        return (FAILED(hr))? NULL : surface;
    }

    // Texture
    //-------------------------------------------------------------------------------
    inline IDirect3DTexture9* GraphicsDeviceRef::createTexture(u32 width, u32 height, u32 levels, Usage usage, BufferFormat format, Pool poolType)
    {
        IDirect3DTexture9 *d3dtex = NULL;
        HRESULT hr = d3dDevice_->CreateTexture(
            width,
            height,
            levels,
            usage,
            (D3DFORMAT)format,
            (D3DPOOL)poolType,
            &d3dtex,
            NULL);

        return (FAILED(hr))? NULL : d3dtex;
    }

    inline void GraphicsDeviceRef::setTexture(u32 sampler, IDirect3DBaseTexture9* texture)
    {
        d3dDevice_->SetTexture(sampler, texture);
    }

    inline void GraphicsDeviceRef::updateTexture(IDirect3DBaseTexture9* src, IDirect3DBaseTexture9* dst)
    {
        d3dDevice_->UpdateTexture(src, dst);
    }

    // Vertex Declaration
    //-------------------------------------------------------------------------------
    inline IDirect3DVertexDeclaration9* GraphicsDeviceRef::createVertexDeclaration(const D3DVERTEXELEMENT9* vertexElements)
    {
        IDirect3DVertexDeclaration9 *vertexDecl = NULL;
        HRESULT hr = d3dDevice_->CreateVertexDeclaration(vertexElements, &vertexDecl);
        return (FAILED(hr))? NULL : vertexDecl;
    }

    inline void GraphicsDeviceRef::setVertexDeclaration(IDirect3DVertexDeclaration9* decl)
    {
        d3dDevice_->SetVertexDeclaration(decl);
    }


    // Buffers
    //----------------------------------------------------------------------------
    inline IDirect3DIndexBuffer9* GraphicsDeviceRef::createIndexBuffer(
        u32 length,
        u32 usage,
        BufferFormat format,
        Pool pool)
    {
        IDirect3DIndexBuffer9 *indices = NULL;
        HRESULT hr = d3dDevice_->CreateIndexBuffer(
            length,
            usage,
            (D3DFORMAT)format,
            (D3DPOOL)pool,
            &indices,
            NULL);
        return (FAILED(hr))? NULL : indices;
    }

    inline void GraphicsDeviceRef::setIndices(IDirect3DIndexBuffer9* indices)
    {
        LASSERT(indices != NULL);
        d3dDevice_->SetIndices(indices);
    }

    inline IDirect3DVertexBuffer9* GraphicsDeviceRef::createVertexBuffer(
        u32 length,
        u32 usage,
        u32 fvf,
        Pool pool)
    {
        IDirect3DVertexBuffer9 *vb = NULL;
        HRESULT hr = d3dDevice_->CreateVertexBuffer(
            length,
            usage,
            fvf,
            (D3DPOOL)pool,
            &vb,
            NULL);
        return (FAILED(hr))? NULL : vb;
    }

    inline void GraphicsDeviceRef::setStreamSource(u32 streamNumber, IDirect3DVertexBuffer9* vb, u32 offset, u32 stride)
    {
        LASSERT(vb!=NULL);
        d3dDevice_->SetStreamSource(streamNumber, vb, offset, stride);
    }

    inline void GraphicsDeviceRef::setStreamSourceFreq(u32 streamNumber, u32 frequency)
    {
        d3dDevice_->SetStreamSourceFreq(streamNumber, frequency);
    }


    // Shaders
    //------------------------------------------------------------------
    inline IDirect3DPixelShader9* GraphicsDeviceRef::createPixelShader(const u32* function)
    {
        IDirect3DPixelShader9 *shader = NULL;
        HRESULT hr = d3dDevice_->CreatePixelShader(reinterpret_cast<const DWORD*>(function), &shader);
        return (FAILED(hr))? NULL : shader;
    }

    inline void GraphicsDeviceRef::setPixelShader(IDirect3DPixelShader9* shader)
    {
        //LASSERT(shader != NULL);
        d3dDevice_->SetPixelShader(shader);
    }

    inline IDirect3DVertexShader9* GraphicsDeviceRef::createVertexShader(const u32* function)
    {
        IDirect3DVertexShader9 *shader = NULL;
        HRESULT hr = d3dDevice_->CreateVertexShader(reinterpret_cast<const DWORD*>(function), &shader);
        return (FAILED(hr))? NULL : shader;
    }

    inline void GraphicsDeviceRef::setVertexShader(IDirect3DVertexShader9* shader)
    {
        //LASSERT(shader != NULL);
        d3dDevice_->SetVertexShader(shader);
    }

    // RenderState
    //------------------------------------------------------------------
    inline IDirect3DStateBlock9* GraphicsDeviceRef::createStateBlock(StateType type)
    {
        IDirect3DStateBlock9 *stateBlock = NULL;
        HRESULT hr = d3dDevice_->CreateStateBlock((D3DSTATEBLOCKTYPE)type, &stateBlock);
        return (FAILED(hr))? NULL : stateBlock;
    }

    inline bool GraphicsDeviceRef::beginStateBlock()
    {
        HRESULT hr = d3dDevice_->BeginStateBlock();
        return SUCCEEDED(hr);
    }

    inline void GraphicsDeviceRef::endStateBlock(IDirect3DStateBlock9** stateBlock)
    {
        LASSERT(stateBlock != NULL);

        d3dDevice_->EndStateBlock(stateBlock);
    }

    inline void GraphicsDeviceRef::setRenderState(u32 type, u32 value)
    {
        d3dDevice_->SetRenderState((D3DRENDERSTATETYPE)type, value);
    }

    inline u32 GraphicsDeviceRef::getRenderState(u32 type)
    {
        DWORD value = 0;
        d3dDevice_->GetRenderState((D3DRENDERSTATETYPE)type, &value);
        return value;
    }

    inline void GraphicsDeviceRef::setAlphaBlendEnable(bool enable)
    {
        d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, (enable)?TRUE : FALSE);
    }


    // SamplerState
    //------------------------------------------------------------------
    inline void GraphicsDeviceRef::setSamplerState(u32 sampler, u32 type, u32 value)
    {
        d3dDevice_->SetSamplerState(sampler, (D3DSAMPLERSTATETYPE)type, value);
    }

    inline u32 GraphicsDeviceRef::getSamplerState(u32 sampler, u32 type)
    {
        DWORD value = 0;
        d3dDevice_->GetSamplerState(sampler, (D3DSAMPLERSTATETYPE)type, &value);
        return value;
    }
}


//---------------------------------------------------
//---
//--- lcore::swap特殊化
//---
//---------------------------------------------------
namespace lcore
{
    template<>
    inline void swap<lgraphics::GraphicsDeviceRef>(lgraphics::GraphicsDeviceRef& l, lgraphics::GraphicsDeviceRef& r)
    {
        l.swap(r);
    }
}
#endif //INC_LGRAPHICS_DX9_GRAPHICSDEVICEREF_H__
