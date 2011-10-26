#include "types.h"
#include "resource.h"

#include <stdio.h>

#include "Config.h"
#include "Dialog.h"
#include "Display.h"

namespace config
{
namespace
{
    struct Size
    {
        u32 width_;
        u32 height_;
    };

    static const u32 NumWinResos = 6;

    Size WinResos[NumWinResos] =
    {
        {640, 480},
        {800, 600},
        {1024, 768},
        {1280, 1024},
        {1366, 768},
        {1920, 1080},
    };

    static const u32 NumBackFormats = 2;
    BufferFormat BackFormats[NumBackFormats] =
    {
        Buffer_R5G6B5,
        Buffer_A8R8G8B8,
    };
}


    ConfigDialog::ConfigDialog()
        :adapter_(config::AdapterDefault)
        ,numDispModes_(0)
        ,displayModes_(NULL)
    {
        formatForBPP_[BPP_16] = Buffer_Num;
        formatForBPP_[BPP_24] = Buffer_Num;
    }

    ConfigDialog::~ConfigDialog()
    {
        CONF_DELETE_ARRAY(displayModes_);
    }

    bool ConfigDialog::initImpl(const Config& config)
    {
        config_ = config;

        if(false == display_.create()){
            //MessageBox(NULL, "Cannot create Direct3D 9", "Error", MB_OK);
            return false;
        }

        config_.rate_ = DefaultRefreshRate;

        if(display_.getAdapterModeCount(adapter_, Buffer_R5G6B5) > 0){
            formatForBPP_[BPP_16] = Buffer_R5G6B5;

        }else if(display_.getAdapterModeCount(adapter_, Buffer_X1R5G5B5) > 0){
            formatForBPP_[BPP_16] = Buffer_X1R5G5B5;
        }

        if(display_.getAdapterModeCount(adapter_, Buffer_X8R8G8B8) > 0){
            formatForBPP_[BPP_24] = Buffer_X8R8G8B8;

        }

        //動作可能なものがひとつもなければ失敗
        bool success = false;
        for(u32 i=0; i<BPP_Num; ++i){
            success |= (formatForBPP_[i] != Buffer_Num);
        }

        return success;
    }


    INT_PTR ConfigDialog::initDialog(HWND hDlg, UINT /*message*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
    {
        dialog_.set(hDlg);

        windowMode_[WinMode_Windowed].set( dialog_.getItem(IDC_RADIO_WINDOWED) );
        windowMode_[WinMode_Fullscreen].set( dialog_.getItem(IDC_RADIO_FULLSCREEN) );

        bpp_[BPP_16].set( dialog_.getItem(IDC_RADIO_16BIT) );
        bpp_[BPP_24].set( dialog_.getItem(IDC_RADIO_24BIT) );

        comboResolution_.set( dialog_.getItem(IDC_COMBO_RESOLUSION) );

        //各初期値セット
        setWindowMode();

        setBPP();

        setResolution();
        

        return (INT_PTR)TRUE;
    }

    INT_PTR ConfigDialog::command(HWND hDlg, UINT /*message*/, WPARAM wParam, LPARAM /*lParam*/)
    {
        switch(LOWORD(wParam))
        {
        case IDOK:
        case IDCANCEL:
            readBack();
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
            break;

        case IDC_RADIO_WINDOWED:
        case IDC_RADIO_FULLSCREEN:
            {
                switch(HIWORD(wParam))
                {
                case BN_CLICKED:
                    {
                        if(windowMode_[WinMode_Windowed].getCheck() == BST_CHECKED){
                            if(false == config_.windowed_){
                                config_.windowed_ = true;
                                setBPP();
                                setResolution();
                            }
                        }else if(windowMode_[WinMode_Fullscreen].getCheck() == BST_CHECKED){
                            if(config_.windowed_){
                                config_.windowed_ = false;
                                setBPP();
                                setResolution();
                            }
                        }
                    }
                    break;
                };
            }
            break;

        case IDC_RADIO_16BIT:
        case IDC_RADIO_24BIT:
            {
                switch(HIWORD(wParam))
                {
                case BN_CLICKED:
                    {
                        if(bpp_[BPP_16].getCheck() == BST_CHECKED){
                            if(config_.format_ != formatForBPP_[BPP_16]){
                                config_.format_ = formatForBPP_[BPP_16];
                                setResolution();
                            }

                        }else if(bpp_[BPP_24].getCheck() == BST_CHECKED){
                            if(config_.format_ != formatForBPP_[BPP_24]){
                                config_.format_ = formatForBPP_[BPP_24];
                                setResolution();
                            }

                        }
                    }
                    break;
                };
            }
            break;

        case IDC_COMBO_RESOLUSION:
            {
                s32 selected = comboResolution_.getCurSel();
                if(0<=selected && selected<numDispModes_){
                    config_.width_ = displayModes_[selected].width_;
                    config_.height_ = displayModes_[selected].height_;
                }
            }
            break;

        default:
            break;
        };

        return (INT_PTR)FALSE;
    }

    bool ConfigDialog::isWindowModeValid(WindowMode mode)
    {
        s32 windowed = (mode == WinMode_Windowed)? TRUE : FALSE;

        for(s32 i=0; i<BPP_Num; ++i){
            for(u32 j=0; j<NumBackFormats; ++j){
                if(display_.checkDeviceType(adapter_, formatForBPP_[i], BackFormats[j], true, windowed)){
                    return true;
                }
            }
        }
        return false;
    }

    bool ConfigDialog::isBPPValid(BPP bpp)
    {
        s32 windowed = config_.windowed_;

        for(u32 j=0; j<NumBackFormats; ++j){
            if(display_.checkDeviceType(adapter_, formatForBPP_[bpp], BackFormats[j], true, windowed)){
                return true;
            }
        }
        return false;
    }


    void ConfigDialog::setWindowMode()
    {
        //初期化時にチェックしているので、どちらかは使用できる
        if(false == isWindowModeValid(WinMode_Windowed)){
            windowMode_[WinMode_Windowed].enable(FALSE);
            config_.windowed_ = FALSE;
        }

        if(false == isWindowModeValid(WinMode_Fullscreen)){
            windowMode_[WinMode_Fullscreen].enable(FALSE);
            config_.windowed_ = TRUE;
        }

        if(config_.windowed_){
            windowMode_[WinMode_Windowed].enable(TRUE);
            windowMode_[WinMode_Windowed].setCheck();
        }else{
            windowMode_[WinMode_Fullscreen].enable(TRUE);
            windowMode_[WinMode_Fullscreen].setCheck();
        }
    }

    void ConfigDialog::setBPP()
    {
        if(false == isBPPValid(BPP_16)){
            bpp_[BPP_16].enable(FALSE);
            config_.format_ = formatForBPP_[BPP_24];
        }

        if(false == isBPPValid(BPP_24)){
            bpp_[BPP_24].enable(FALSE);
            config_.format_ = formatForBPP_[BPP_16];
        }

        switch(config_.format_)
        {
        case Buffer_A8R8G8B8:
        case Buffer_X8R8G8B8:
            {
                bpp_[BPP_24].enable(TRUE);
                bpp_[BPP_24].setCheck();
                bpp_[BPP_16].setUnCheck();
            }
            break;

        case Buffer_R5G6B5:
        case Buffer_X1R5G5B5:
        case Buffer_A1R5G5B5:
            {
                bpp_[BPP_16].enable(TRUE);
                bpp_[BPP_16].setCheck();
                bpp_[BPP_24].setUnCheck();
            }
            break;

        default:
            CONF_ASSERT(false);
            break;
        };
    }

    void ConfigDialog::setResolution()
    {
        comboResolution_.resetContent();
        CONF_DELETE_ARRAY(displayModes_);
        numDispModes_ = 0;

        s32 index = 0;
        if(config_.windowed_){
            displayModes_ = CONF_NEW DisplayMode[NumWinResos];

            for(u32 i=0; i<NumWinResos; ++i){
                displayModes_[index].width_ = WinResos[i].width_;
                displayModes_[index].height_ = WinResos[i].height_;
                displayModes_[index].refreshRate_ = DefaultRefreshRate;
                displayModes_[index].format_ = config_.format_;

                if(addComboText(WinResos[i].width_, WinResos[i].height_)){
                    ++index;
                }
            }

        }else{
            u32 total = display_.getAdapterModeCount(adapter_, (BufferFormat)config_.format_);

            displayModes_ = CONF_NEW DisplayMode[total];

            DisplayMode dispMode;

            s32 index = 0;
            for(u32 i=0; i<total; ++i){
                if(false == display_.enumAdapterModes(adapter_, (BufferFormat)config_.format_, i, &dispMode)){
                    continue;
                }
                if(dispMode.refreshRate_ != DefaultRefreshRate){
                    continue;
                }

                if(false == existSameSize(index, dispMode)){
                    if(addComboText(dispMode)){
                        displayModes_[index] = dispMode;
                        ++index;
                    }
                }
            }
        }

        numDispModes_ = index;
        selectNearestResolution();
    }

    bool ConfigDialog::addComboText(const DisplayMode& mode)
    {
        return addComboText(mode.width_, mode.height_);
    }

    bool ConfigDialog::addComboText(u32 width, u32 height)
    {
        static const u32 TextLength = 64;
        Char str[TextLength];
        sprintf_s(str, TextLength, "%dx%d", width, height);
        return (0 <= comboResolution_.addString(str));
    }

    void ConfigDialog::selectNearestResolution()
    {
        for(s32 i=numDispModes_-1; i>=0; --i){
            if(config_.width_==displayModes_[i].width_
                && config_.height_==displayModes_[i].height_)
            {

                comboResolution_.setCurSel(i);
                comboResolution_.setTopIndex(i);
                return;
            }
        }
        comboResolution_.setCurSel(0);
        comboResolution_.setTopIndex(0);
    }

    bool ConfigDialog::existSameSize(s32 num, const DisplayMode& mode)
    {
        for(s32 i=0; i<num; ++i){
            if(displayModes_[i].width_ == mode.width_
                && displayModes_[i].height_ == mode.height_)
            {
                return true;
            }
        }
        return false;
    }

    void ConfigDialog::readBack()
    {
        if(windowMode_[WinMode_Windowed].getCheck() == BST_CHECKED){
            config_.windowed_ = TRUE;
        }else{
            config_.windowed_ = FALSE;
        }

        if(bpp_[BPP_16].getCheck() == BST_CHECKED){
            config_.format_ = formatForBPP_[BPP_16];
        }else{
            config_.format_ = formatForBPP_[BPP_24];
        }


        s32 selected = comboResolution_.getCurSel();
        if(0<=selected && selected<numDispModes_){
            config_.width_ = displayModes_[selected].width_;
            config_.height_ = displayModes_[selected].height_;
            config_.rate_ = displayModes_[selected].refreshRate_;
        }
    }
}
