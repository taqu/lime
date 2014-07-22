/**
@file Dialog.cpp
@author t-sakai
@date 2011/09/23
*/
#include "stdafx.h"
#include "Dialog.h"

#include <Commdlg.h>
#include "Resource.h"

#include "FontPackerDF.h"

namespace font
{
    FontDialog::FontDialog()
    {
        fontInfo_.height_ = 128;
        fontInfo_.width_ = 128;
        fontInfo_.outline_ = 0;
        fontInfo_.asciiOnly_ = false;

        fontInfo_.distanceField_ = false;
        fontInfo_.distanceScale_ = 4;
        fontInfo_.distanceSpread_ = 3;
    }

    FontDialog::~FontDialog()
    {
    }

    bool FontDialog::initImpl()
    {
        return true;
    }

    INT_PTR FontDialog::initDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
        outline_.set( GetDlgItem(hDlg, IDC_COMBO_OUTLINE) );
        outline_.resetContent();
        outline_.addString(_T("0"));
        outline_.addString(_T("1"));
        outline_.addString(_T("2"));
        outline_.addString(_T("3"));
        outline_.addString(_T("4"));
        outline_.addString(_T("5"));
        outline_.setCurSel(0);
        outline_.setTopIndex(0);

        resolution_.set( GetDlgItem(hDlg, IDC_COMBO_RESOLUTION) );
        resolution_.resetContent();
        resolution_.addString(_T("128"));
        resolution_.addString(_T("256"));
        resolution_.addString(_T("512"));
        resolution_.addString(_T("1024"));
        resolution_.addString(_T("2048"));
        resolution_.setCurSel(4);
        resolution_.setTopIndex(4);

        distanceScale_.set( GetDlgItem(hDlg, IDC_COMBO_DISTANCESCALE) );
        distanceScale_.resetContent();
        distanceScale_.addString(_T("1"));
        distanceScale_.addString(_T("2"));
        distanceScale_.addString(_T("3"));
        distanceScale_.addString(_T("4"));
        distanceScale_.addString(_T("5"));
        distanceScale_.addString(_T("6"));
        distanceScale_.addString(_T("7"));
        distanceScale_.addString(_T("8"));
        distanceScale_.setCurSel(1);
        distanceScale_.setTopIndex(1);

        distanceSpread_.set( GetDlgItem(hDlg, IDC_COMBO_DISTANCESPREAD) );
        distanceSpread_.resetContent();
        distanceSpread_.addString(_T("1"));
        distanceSpread_.addString(_T("2"));
        distanceSpread_.addString(_T("3"));
        distanceSpread_.addString(_T("4"));
        distanceSpread_.addString(_T("5"));
        distanceSpread_.addString(_T("6"));
        distanceSpread_.addString(_T("7"));
        distanceSpread_.addString(_T("8"));
        distanceSpread_.addString(_T("9"));
        distanceSpread_.addString(_T("10"));
        distanceSpread_.addString(_T("11"));
        distanceSpread_.addString(_T("12"));
        distanceSpread_.addString(_T("13"));
        distanceSpread_.addString(_T("14"));
        distanceSpread_.addString(_T("15"));
        distanceSpread_.addString(_T("16"));
        distanceSpread_.setCurSel(7);
        distanceSpread_.setTopIndex(7);

        checkDistanceField_.set( GetDlgItem(hDlg, IDC_CHECK_DISTANCE) );
        checkDistanceField_.setCheck();

        checkASCIIOnly_.set( GetDlgItem(hDlg, IDC_CHECK_ASCII_ONLY) );

        if(fontInfo_.asciiOnly_){
            checkASCIIOnly_.setCheck();
        }else{
            checkASCIIOnly_.setUnCheck();
        }

        return (INT_PTR)TRUE;
    }

    INT_PTR FontDialog::command(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch(LOWORD(wParam))
        {
        case IDC_OK:
            {
                if(fontInfo_.face_.size()>0){
                    readBack();

                    //fontInfo_.distanceField_ = true;
                    if(fontInfo_.distanceField_){
                        FontPackerDF fontPacker;
                        //fontInfo_.height_ = 1024;
                        //fontInfo_.width_ = 1024;
                        //fontInfo_.outline_ = 0;
                        //fontInfo_.distanceScale_ = 1;
                        //fontInfo_.distanceSpread_ = 1;

                        fontPacker.create(hDlg, fontInfo_);
                        fontPacker.save(_T("out.bmp"), _T("out.def"));
                    }else{
                        FontPacker fontPacker;
                        fontPacker.create(hDlg, fontInfo_);
                        fontPacker.save(_T("out.bmp"), _T("out.def"));
                    }
                }
            }
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
            break;

        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
            break;

        case IDC_BUTTON_FONT:
            {
                LOGFONT logFont;
                ZeroMemory(&logFont, sizeof(LOGFONT));
                if(fontInfo_.face_.size()>0){
                    logFont.lfHeight = fontInfo_.size_;
                    logFont.lfWeight = (fontInfo_.bold_)? FW_BOLD : FW_NORMAL;

                    for(size_t i=0; i<fontInfo_.face_.size(); ++i){
                        logFont.lfFaceName[i] = fontInfo_.face_.at(i);
                    }
                }

                CHOOSEFONT cf;
                ZeroMemory(&cf, sizeof(CHOOSEFONT));
                cf.lStructSize = sizeof(CHOOSEFONT);
                cf.hwndOwner = hDlg;
                cf.lpLogFont = &logFont;
                cf.Flags = CF_SCREENFONTS;
                cf.rgbColors = RGB(0, 0, 0);
                cf.nFontType = SCREEN_FONTTYPE;

                BOOL ret = ChooseFont(&cf);
                size_t l = _tcslen(logFont.lfFaceName);
                if(TRUE == ret && l>0){

                    fontInfo_.face_ = logFont.lfFaceName;
                    fontInfo_.size_ = logFont.lfHeight;
                    fontInfo_.bold_ = (logFont.lfWeight >= FW_BOLD);
                }
            }
            break;

        default:
            break;
        };

        return (INT_PTR)FALSE;
    }

    void FontDialog::readBack()
    {
        fontInfo_.outline_ = outline_.getCurSel();

        fontInfo_.distanceField_ = (checkDistanceField_.getCheck() != 0);
        fontInfo_.asciiOnly_ = (checkASCIIOnly_.getCheck() != 0);

        LRESULT curSel = resolution_.getCurSel();
        switch(curSel)
        {
        case 0:
            fontInfo_.height_ = fontInfo_.width_ = 128;
            break;

        case 1:
            fontInfo_.height_ = fontInfo_.width_ = 256;
            break;

        case 2:
            fontInfo_.height_ = fontInfo_.width_ = 512;
            break;

        case 3:
            fontInfo_.height_ = fontInfo_.width_ = 1024;
            break;

        default:
            fontInfo_.height_ = fontInfo_.width_ = 2048;
            break;
        };

        curSel = distanceScale_.getCurSel();
        if(curSel<0){
            fontInfo_.distanceScale_ = 1;
        }else if(curSel>=MaxDistanceScale){
            fontInfo_.distanceScale_ = MaxDistanceScale;
        }else{
            fontInfo_.distanceScale_ = curSel+1;
        }

        curSel = distanceSpread_.getCurSel();
        if(curSel<0){
            fontInfo_.distanceSpread_ = 1;
        }else if(curSel>=MaxDistanceSpread){
            fontInfo_.distanceSpread_ = MaxDistanceSpread;
        }else{
            fontInfo_.distanceSpread_ = curSel+1;
        }
    }
}
