/**
@file DisplayMode.cpp
@author t-sakai
@date 2017/01/09 create
*/
#include "DisplayMode.h"
#include <stdio.h>
#include <lcore/Sort.h>
#include "ConfigDialog.h"

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
    }

    //--------------------------------------------
    //---
    //---
    //---
    //--------------------------------------------
    namespace
    {
        class DisplayConfigDialog
        {
        public:
            static const u32 RefreshRate30 = 30;
            static const u32 RefreshRate60 = 60;
            static const s32 MaxDisplayModes = 64;


            enum WindowMode
            {
                WinMode_Windowed = 0,
                WinMode_Fullscreen,
                WinMode_Num,
            };

            enum BPP
            {
                BPP_24 =0,
                BPP_Num,
            };

            inline static void enableWindow(HWND hWnd, BOOL enable)
            {
                EnableWindow(hWnd, enable);
            }
            inline static bool radioButtonIsChecked(HWND hWnd)
            {
                return 0 != SendMessage(hWnd, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
            }
            inline static void radioButtonSetCheck(HWND hWnd)
            {
                SendMessage(hWnd, BM_SETCHECK, (WPARAM)BST_CHECKED, (LPARAM)0);
            }
            inline static void radioButtonSetUncheck(HWND hWnd)
            {
                SendMessage(hWnd, BM_SETCHECK, (WPARAM)BST_UNCHECKED, (LPARAM)0);
            }

            inline static void comboBoxAddString(HWND hWnd, const Char* str)
            {
                SendMessage(hWnd, CB_ADDSTRING, 0, (LPARAM)str);
            }
            inline static void comboBoxDeleteString(HWND hWnd, s32 index)
            {
                SendMessage(hWnd, CB_DELETESTRING, index, 0);
            }
            inline static void comboBoxClear(HWND hWnd)
            {
                SendMessage(hWnd, CB_RESETCONTENT, 0, 0);
            }
            inline static s32 comboBoxCount(HWND hWnd)
            {
                return static_cast<s32>(SendMessage(hWnd, CB_GETCOUNT, 0, 0));
            }
            inline static s32 comboBoxGetCurSel(HWND hWnd)
            {
                return static_cast<s32>(SendMessage(hWnd, CB_GETCURSEL, 0, 0));
            }
            inline static void comboBoxSetCurSel(HWND hWnd, s32 index)
            {
                SendMessage(hWnd, CB_SETCURSEL, index, 0);
            }

            void initialize(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
            INT_PTR command(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
            void terminate(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

            void setWindowMode();
            void getWindowMode();

            void refreshDisplayMode();

            WindowMode windowMode_;
            DXGIDisplayMode displayMode_;
            WindowMode selectedWindowMode_;
            s32 selectedDisplayMode_;

            HWND radioButtonWindowMode_[WinMode_Num];
            HWND radioButtonBpp_[BPP_Num];
            HWND comboBoxResolution_;
            s32 numDispModes_;
            DXGIDisplayMode dispModes_[MaxDisplayModes];
        };

        void DisplayConfigDialog::initialize(HWND hDlg, UINT /*message*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
        {
            selectedWindowMode_ = windowMode_;
            radioButtonWindowMode_[WinMode_Windowed] = GetDlgItem(hDlg, IDC_RADIO_WINDOWED);
            radioButtonWindowMode_[WinMode_Fullscreen] = GetDlgItem(hDlg, IDC_RADIO_FULLSCREEN);
            setWindowMode();

            radioButtonBpp_[BPP_24] = GetDlgItem(hDlg, IDC_RADIO_24BIT);
            radioButtonSetCheck(radioButtonBpp_[BPP_24]);
            enableWindow(radioButtonBpp_[BPP_24], FALSE);

            comboBoxResolution_ = GetDlgItem(hDlg, IDC_COMBO_RESOLUSION);
            refreshDisplayMode();
        }

        void DisplayConfigDialog::setWindowMode()
        {
            switch(selectedWindowMode_)
            {
            case WinMode_Windowed:
                radioButtonSetCheck(radioButtonWindowMode_[WinMode_Windowed]);
                radioButtonSetUncheck(radioButtonWindowMode_[WinMode_Fullscreen]);
                break;
            case WinMode_Fullscreen:
                radioButtonSetUncheck(radioButtonWindowMode_[WinMode_Windowed]);
                radioButtonSetCheck(radioButtonWindowMode_[WinMode_Fullscreen]);
                break;
            }
        }

        void DisplayConfigDialog::getWindowMode()
        {
            bool windowed = radioButtonIsChecked(radioButtonWindowMode_[WinMode_Windowed]);
            bool fullscreen = radioButtonIsChecked(radioButtonWindowMode_[WinMode_Fullscreen]);
            if(!windowed && !fullscreen){
                selectedWindowMode_ = WinMode_Windowed;
                setWindowMode();

            } else if(windowed){
                selectedWindowMode_ = WinMode_Windowed;

            } else {
                selectedDisplayMode_ = WinMode_Fullscreen;
            }
        }

        void DisplayConfigDialog::refreshDisplayMode()
        {
            selectedDisplayMode_ = 0;
            numDispModes_ = 0;
            comboBoxClear(comboBoxResolution_);

            DXGIFactory factory = DXGIFactory::create();
            if(!factory.valid()){
                return;
            }
            DXGIAdapter adapter = factory.getAdapter(0);
            if(!adapter.valid()){
                return;
            }
            DXGIOutput output = adapter.getOutput(0);
            if(!output.valid()){
                return;
            }
            numDispModes_ = output.getDisplayModes(static_cast<DXGI_FORMAT>(displayMode_.format_), MaxDisplayModes, dispModes_);
            if(numDispModes_<=0){
                static const DXGI_FORMAT formats[] =
                {
                    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
                    DXGI_FORMAT_R8G8B8A8_UNORM,
                    DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
                    DXGI_FORMAT_B8G8R8A8_UNORM,
                    DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,
                    DXGI_FORMAT_B8G8R8X8_UNORM,
                };
                for(s32 i=0; i<6; ++i){
                    numDispModes_ = output.getDisplayModes(formats[i], MaxDisplayModes, dispModes_);
                    if(0<numDispModes_){
                        displayMode_.format_ = formats[i];
                        break;
                    }
                }
                if(numDispModes_<=0){
                    return;
                }
            }
            selectedDisplayMode_ = DXGIOutput::selectClosestMatchingMode(numDispModes_, dispModes_, displayMode_);
            if(selectedDisplayMode_<0){
                selectedDisplayMode_ = 0;
            }
            displayMode_ = dispModes_[selectedDisplayMode_];
            static const s32 BufferSize = 128;
            Char buffer[BufferSize];
            for(s32 i=0; i<numDispModes_; ++i){
                sprintf_s(buffer, BufferSize, "%dx%d %dHz",
                    dispModes_[i].width_,
                    dispModes_[i].height_,
                    dispModes_[i].refreshRate_);
                buffer[BufferSize-1] = lcore::CharNull;
                comboBoxAddString(comboBoxResolution_, buffer);
            }
            comboBoxSetCurSel(comboBoxResolution_, selectedDisplayMode_);
        }

        INT_PTR DisplayConfigDialog::command(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch(LOWORD(wParam))
            {
            case IDOK:
                windowMode_ = selectedWindowMode_;
                if(selectedDisplayMode_<numDispModes_){
                    displayMode_ = dispModes_[selectedDisplayMode_];
                }
                terminate(hDlg, message, wParam, lParam);
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            case IDCANCEL:
                terminate(hDlg, message, wParam, lParam);
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            case IDC_RADIO_WINDOWED:
            case IDC_RADIO_FULLSCREEN:
                getWindowMode();
                break;
            case IDC_COMBO_RESOLUSION:
                selectedDisplayMode_ = comboBoxGetCurSel(comboBoxResolution_);
                break;
            default:
                break;
            }
            return (INT_PTR)FALSE;
        }

        void DisplayConfigDialog::terminate(HWND /*hDlg*/, UINT /*message*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
        {

        }

        static DisplayConfigDialog* displayConfigDialog_ = NULL;

        INT_PTR CALLBACK dialog_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
        {
            UNREFERENCED_PARAMETER(lParam);
            switch(message)
            {
            case WM_INITDIALOG:
                displayConfigDialog_->initialize(hDlg, message, wParam, lParam);
                break;
            case WM_COMMAND:
                return displayConfigDialog_->command(hDlg, message, wParam, lParam);
            }
            return (INT_PTR)FALSE;
        }
    }

    bool getDisplayMode(HINSTANCE hInstance, DXGIDisplayMode& dst, bool& windowed, const DXGIDisplayMode& request)
    {
        LDELETE(displayConfigDialog_);
        displayConfigDialog_ = LNEW DisplayConfigDialog;
        displayConfigDialog_->windowMode_ = (windowed)? DisplayConfigDialog::WinMode_Windowed : DisplayConfigDialog::WinMode_Fullscreen;
        displayConfigDialog_->displayMode_ = request;

        LPCTSTR lpTemplate = MAKEINTRESOURCE(IDD_DISPLAY_CONFIG);
        INT_PTR ret = DialogBox(hInstance, lpTemplate, NULL, dialog_proc);
        dst = displayConfigDialog_->displayMode_;
        windowed = (displayConfigDialog_->windowMode_ == DisplayConfigDialog::WinMode_Windowed);
        LDELETE(displayConfigDialog_);
        return 0<ret;
    }
}
