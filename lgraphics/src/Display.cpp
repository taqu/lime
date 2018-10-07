/**
@file Display.cpp
@author t-sakai
@date 2013/05/16 create
*/
#include "Display.h"
#include <stdio.h>
#include <lcore/Sort.h>

namespace lgfx
{
namespace
{
    struct DXGIDisplayModeSelect
    {
        s32 index_;
        DXGIDisplayMode displayMode_;

        static bool cmp_size(const DXGIDisplayModeSelect& x0, const DXGIDisplayModeSelect& x1)
        {
            s32 s0 = x0.displayMode_.width_*x0.displayMode_.height_;
            s32 s1 = x1.displayMode_.width_*x1.displayMode_.height_;

            return s0<s1;
        }
        static bool cmp_refreshrate(const DXGIDisplayModeSelect& x0, const DXGIDisplayModeSelect& x1)
        {
            return x0.displayMode_.refreshRate_<x1.displayMode_.refreshRate_;
        }
    };

    f32 calcRefreshRate(const DXGI_RATIONAL& rational)
    {
        return static_cast<f32>(rational.Numerator)/rational.Denominator;
    }

    s32 selectSameSize(s32 numModes, const DXGIDisplayMode* modes, const DXGIDisplayMode& request)
    {
        s32 count = 0;
        DXGIDisplayModeSelect tmpModes[DXGIOutput::NumMaxModes];

        s32 size = request.width_*request.height_;
        for(s32 i=0; i<numModes; ++i){
            s32 s = modes[i].width_*modes[i].height_;
            if(s!=size){
                continue;
            }
            if(DXGIOutput::NumMaxModes<=count){
                break;
            }
            tmpModes[count].index_ = i;
            tmpModes[count].displayMode_ = modes[i];
            ++count;
        }
        for(s32 i=0; i<count; ++i){
            if(tmpModes[i].displayMode_.format_ == request.format_
                && tmpModes[i].displayMode_.refreshRate_ == request.refreshRate_)
            {
                return tmpModes[i].index_;
            }
        }

        if(0<count){
            lcore::introsort(count, tmpModes, DXGIDisplayModeSelect::cmp_refreshrate);
            for(s32 i=count-1; 0<=i; --i){
                if(tmpModes[i].displayMode_.format_ == request.format_){
                    return tmpModes[i].index_;
                }
            }
            return tmpModes[count-1].index_;
        }
        return -1;
    }

    s32 selectLargerSize(s32 numModes, const DXGIDisplayMode* modes, const DXGIDisplayMode& request)
    {
        s32 count = 0;
        DXGIDisplayModeSelect tmpModes[DXGIOutput::NumMaxModes];

        s32 size = request.width_*request.height_;
        for(s32 i=0; i<numModes; ++i){
            s32 s = modes[i].width_*modes[i].height_;
            if(s<=size){
                continue;
            }
            if(DXGIOutput::NumMaxModes<=count){
                break;
            }
            tmpModes[count].index_ = i;
            tmpModes[count].displayMode_ = modes[i];
            ++count;
        }
        lcore::introsort(count, tmpModes, DXGIDisplayModeSelect::cmp_size);
        for(s32 i=0; i<count; ++i){
            if(tmpModes[i].displayMode_.format_ == request.format_
                && tmpModes[i].displayMode_.refreshRate_ == request.refreshRate_)
            {
                return tmpModes[i].index_;
            }
        }
        if(0<count){
            lcore::introsort(count, tmpModes, DXGIDisplayModeSelect::cmp_refreshrate);
            tmpModes[count-1].index_;
        }
        return -1;
    }
}

    //--------------------------------------------
    //---
    //--- DXGIOutput
    //---
    //--------------------------------------------
    s32 DXGIOutput::selectClosestMatchingMode(s32 numModes, const DXGIDisplayMode* modes, const DXGIDisplayMode& request)
    {
        LASSERT(0<=numModes);
        LASSERT(NULL != modes);
        s32 index = selectSameSize(numModes, modes, request);
        if(0<=index){
            return index;
        }
        return selectLargerSize(numModes, modes, request);
    }

    DXGIOutput::DXGIOutput()
        :output_(NULL)
    {
    }

    DXGIOutput::DXGIOutput(const DXGIOutput& rhs)
        :output_(rhs.output_)
    {
        if(NULL != output_){
            output_->AddRef();
        }
    }

    DXGIOutput::DXGIOutput(IDXGIOutput* output)
        :output_(output)
    {
    }

    DXGIOutput::~DXGIOutput()
    {
        LDXSAFE_RELEASE(output_);
    }

    bool DXGIOutput::valid() const
    {
        return NULL != output_;
    }

    s32 DXGIOutput::getDisplayModes(DXGI_FORMAT format, s32 numModes, DXGIDisplayMode* modes)
    {
        LASSERT(NULL != modes);
        if(!valid()){
            return 0;
        }
        DXGI_MODE_DESC tmp[NumMaxModes];
        u32 num = (NumMaxModes<numModes)? NumMaxModes : numModes;
        HRESULT hr = output_->GetDisplayModeList(format, 0, &num, tmp);
        if(FAILED(hr)){
            return 0;
        }
        numModes = static_cast<s32>(num);
        s32 count = 0;
        for(s32 i=0; i<numModes; ++i){
            bool found = false;
            u32 refreshRate = static_cast<u32>(calcRefreshRate(tmp[i].RefreshRate));
            for(s32 j=0; j<count; ++j){
                if(modes[j].width_ == tmp[i].Width
                    && modes[j].height_ == tmp[i].Height
                    && modes[j].refreshRate_ == refreshRate)
                {
                    found = true;
                    break;
                }
            }
            if(found){
                continue;
            }
            modes[count].width_ = tmp[i].Width;
            modes[count].height_ = tmp[i].Height;
            modes[count].format_ = tmp[i].Format;
            modes[count].refreshRate_ = refreshRate;
            modes[count].scanlineOrdering_ = tmp[i].ScanlineOrdering;
            modes[count].scaling_ = tmp[i].Scaling;
            ++count;
        }
        return count;
    }

    bool DXGIOutput::findClosestMatchingMode(DXGIDisplayMode& dst, const DXGIDisplayMode& request)
    {
        if(!valid()){
            return false;
        }
        DXGI_MODE_DESC tmpDst = {0};
        DXGI_MODE_DESC tmpRequest = {0};
        tmpRequest.Width = request.width_;
        tmpRequest.Height = request.height_;
        tmpRequest.Format = static_cast<DXGI_FORMAT>(request.format_);
        tmpRequest.RefreshRate.Numerator = request.refreshRate_;
        tmpRequest.RefreshRate.Denominator = 1;
        tmpRequest.ScanlineOrdering = request.scanlineOrdering_;
        tmpRequest.Scaling = request.scaling_;
        HRESULT hr = output_->FindClosestMatchingMode(&tmpRequest, &tmpDst, NULL);
        if(FAILED(hr)){
            return false;
        }
        dst.width_ = tmpDst.Width;
        dst.height_ = tmpDst.Height;
        dst.format_ = tmpDst.Format;
        dst.refreshRate_ = static_cast<u32>(calcRefreshRate(tmpDst.RefreshRate));
        dst.scanlineOrdering_ = tmpDst.ScanlineOrdering;
        dst.scaling_ = tmpDst.Scaling;
        return true;
    }

    DXGIOutput& DXGIOutput::operator=(const DXGIOutput& rhs)
    {
        LDXSAFE_RELEASE(output_);
        output_ = rhs.output_;
        if(NULL != output_){
            output_->AddRef();
        }
        return *this;
    }

    //--------------------------------------------
    //---
    //--- DXGIAdapter
    //---
    //--------------------------------------------
    DXGIAdapter::DXGIAdapter()
        :adapter_(NULL)
        ,outputCount_(0)
    {
    }

    DXGIAdapter::DXGIAdapter(const DXGIAdapter& rhs)
        :adapter_(rhs.adapter_)
    {
        if(NULL != adapter_){
            adapter_->AddRef();
        }
        refresh();
    }

    DXGIAdapter::DXGIAdapter(IDXGIAdapter1* adapter)
        :adapter_(adapter)
    {
        refresh();
    }

    DXGIAdapter::~DXGIAdapter()
    {
        LDXSAFE_RELEASE(adapter_);
    }

    bool DXGIAdapter::valid() const
    {
        return NULL != adapter_;
    }

    void DXGIAdapter::refresh()
    {
        outputCount_ = 0;
        if(NULL == adapter_){
            return;
        }
        IDXGIOutput* output;
        for(u32 i=0; i<NumMaxOutputs; ++i){
            if(adapter_->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND){
                output->Release();
                ++outputCount_;
            }
        }
    }

    u32 DXGIAdapter::getOutputCount() const
    {
        return outputCount_;
    }

    DXGIOutput DXGIAdapter::getOutput(u32 id)
    {
        LASSERT(0<=id && id<outputCount_);
        IDXGIOutput *output; 
        for(u32 i=0; i<outputCount_; ++i){
            if(adapter_->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND){
                if(i == id){
                    return DXGIOutput(output);
                }
                output->Release();
            }
        }
        return DXGIOutput();
    }

    DXGIAdapter& DXGIAdapter::operator=(const DXGIAdapter& rhs)
    {
        LDXSAFE_RELEASE(adapter_);
        adapter_ = rhs.adapter_;
        if(NULL != adapter_){
            adapter_->AddRef();
        }
        refresh();
        return *this;
    }

    //--------------------------------------------
    //---
    //--- DXGIFactory
    //---
    //--------------------------------------------
    DXGIFactory::DXGIFactory()
        :factory_(NULL)
        ,adapterCount_(0)
    {
    }

    DXGIFactory::DXGIFactory(const DXGIFactory& rhs)
        :factory_(rhs.factory_)
    {
        if(NULL != factory_){
            factory_->AddRef();
        }
        refresh();
    }

    DXGIFactory::DXGIFactory(IDXGIFactory1* factory)
        :factory_(factory)
    {
        refresh();
    }

    DXGIFactory::~DXGIFactory()
    {
        LDXSAFE_RELEASE(factory_);
    }

    DXGIFactory DXGIFactory::create()
    {
        IDXGIFactory1* factory = NULL;
        CreateDXGIFactory1(__uuidof(IDXGIFactory1), (VOID**)&factory);
        DXGIFactory result(factory);
        if(result.valid()){
            result.refresh();
        }
        return result;
    }

    bool DXGIFactory::valid() const
    {
        return NULL != factory_;
    }

    void DXGIFactory::refresh()
    {
        adapterCount_ = 0;
        if(NULL == factory_){
            return;
        }
        IDXGIAdapter1 *adapter; 
        for(u32 i=0; i<NumMaxAdapters; ++i){
            if(factory_->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND){
                adapter->Release();
                ++adapterCount_;
            }
        }
    }

    u32 DXGIFactory::getAdapterCount() const
    {
        return adapterCount_;
    }

    DXGIAdapter DXGIFactory::getAdapter(u32 id)
    {
        LASSERT(0<=id && id<adapterCount_);
        IDXGIAdapter1 *adapter; 
        for(u32 i=0; i<adapterCount_; ++i){
            if(factory_->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND){
                if(i == id){
                    return DXGIAdapter(adapter);
                }
                adapter->Release();
            }
        }
        return DXGIAdapter();
    }

    DXGIFactory& DXGIFactory::operator=(const DXGIFactory& rhs)
    {
        LDXSAFE_RELEASE(factory_);
        factory_ = rhs.factory_;
        if(NULL != factory_){
            factory_->AddRef();
        }
        refresh();
        return *this;
    }
}
