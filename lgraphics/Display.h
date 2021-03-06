#ifndef INC_LGRAPHICS_DX11_DISPLAY_H_
#define INC_LGRAPHICS_DX11_DISPLAY_H_
/**
@file Display.h
@author t-sakai
@date 2013/05/16 create
*/
#include "lgraphics.h"

struct IDXGIFactory1;
struct IDXGIAdapter1;
struct IDXGIOutput;

namespace lgfx
{
    struct DXGIDisplayMode
    {
        u32 width_;
        u32 height_;
        u32 format_;
        u32 refreshRate_;
        DXGI_MODE_SCANLINE_ORDER scanlineOrdering_;
        DXGI_MODE_SCALING scaling_;
    };

    inline bool operator==(const DXGIDisplayMode& mode0, const DXGIDisplayMode& mode1)
    {
        return (mode0.width_ == mode1.width_)
            && (mode0.height_ == mode1.width_)
            && (mode0.format_ == mode1.format_)
            && (mode0.refreshRate_ == mode1.refreshRate_);
    }

    inline bool operator!=(const DXGIDisplayMode& mode0, const DXGIDisplayMode& mode1)
    {
        return (mode0.width_ != mode1.width_)
            || (mode0.height_ != mode1.width_)
            || (mode0.format_ != mode1.format_)
            || (mode0.refreshRate_ != mode1.refreshRate_);
    }

    //--------------------------------------------
    //---
    //--- DXGIOutput
    //---
    //--------------------------------------------
    class DXGIOutput
    {
    public:
        static const u32 NumMaxOutputs = 8;
        static const u32 NumMaxModes = 64;

        static s32 selectClosestMatchingMode(s32 numModes, const DXGIDisplayMode* modes, const DXGIDisplayMode& request);

        DXGIOutput();
        DXGIOutput(const DXGIOutput& rhs);
        explicit DXGIOutput(IDXGIOutput* output);
        ~DXGIOutput();

        bool valid() const;
        s32 getDisplayModes(DXGI_FORMAT format, s32 numModes, DXGIDisplayMode* modes);
        bool findClosestMatchingMode(DXGIDisplayMode& dst, const DXGIDisplayMode& request);

        DXGIOutput& operator=(const DXGIOutput& rhs);
    private:
        IDXGIOutput* output_;
    };

    //--------------------------------------------
    //---
    //--- DXGIAdapter
    //---
    //--------------------------------------------
    class DXGIAdapter
    {
    public:
        static const u32 NumMaxOutputs = 8;

        DXGIAdapter();
        DXGIAdapter(const DXGIAdapter& rhs);
        explicit DXGIAdapter(IDXGIAdapter1* adapter);
        ~DXGIAdapter();

        bool valid() const;
        void refresh();
        u32 getOutputCount() const;

        DXGIOutput getOutput(u32 id);

        DXGIAdapter& operator=(const DXGIAdapter& rhs);

    private:
        IDXGIAdapter1* adapter_;
        u32 outputCount_;
    };

    //--------------------------------------------
    //---
    //--- DXGIFactory
    //---
    //--------------------------------------------
    class DXGIFactory
    {
    public:
        static const u32 NumMaxAdapters = 8;

        static DXGIFactory create();

        DXGIFactory();
        DXGIFactory(const DXGIFactory& rhs);
        ~DXGIFactory();

        bool valid() const;
        void refresh();
        u32 getAdapterCount() const;

        DXGIAdapter getAdapter(u32 id);

        DXGIFactory& operator=(const DXGIFactory& rhs);
    private:
        explicit DXGIFactory(IDXGIFactory1* factory);

        IDXGIFactory1* factory_;
        u32 adapterCount_;
    };
}
#endif //INC_LGRAPHICS_DX11_DISPLAY_H_
