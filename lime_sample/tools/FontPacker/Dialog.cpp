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
        outline_.addString("0");
        outline_.addString("1");
        outline_.addString("2");
        outline_.addString("3");
        outline_.addString("4");
        outline_.addString("5");
        outline_.setCurSel(0);
        outline_.setTopIndex(0);

        resolution_.set( GetDlgItem(hDlg, IDC_COMBO_RESOLUTION) );
        resolution_.resetContent();
        resolution_.addString("128");
        resolution_.addString("256");
        resolution_.addString("512");
        resolution_.addString("1024");
        resolution_.addString("2048");
        resolution_.setCurSel(1);
        resolution_.setTopIndex(1);

        distanceScale_.set( GetDlgItem(hDlg, IDC_COMBO_DISTANCESCALE) );
        distanceScale_.resetContent();
        distanceScale_.addString("1");
        distanceScale_.addString("2");
        distanceScale_.addString("3");
        distanceScale_.addString("4");
        distanceScale_.addString("5");
        distanceScale_.addString("6");
        distanceScale_.addString("7");
        distanceScale_.addString("8");
        distanceScale_.setCurSel(7);
        distanceScale_.setTopIndex(7);

        distanceSpread_.set( GetDlgItem(hDlg, IDC_COMBO_DISTANCESPREAD) );
        distanceSpread_.resetContent();
        distanceSpread_.addString("1");
        distanceSpread_.addString("2");
        distanceSpread_.addString("3");
        distanceSpread_.addString("4");
        distanceSpread_.addString("5");
        distanceSpread_.addString("6");
        distanceSpread_.addString("7");
        distanceSpread_.addString("8");
        distanceSpread_.setCurSel(7);
        distanceSpread_.setTopIndex(7);

        checkDistanceField_.set( GetDlgItem(hDlg, IDC_CHECK_DISTANCE) );
        checkDistanceField_.setUnCheck();

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

                    if(fontInfo_.distanceField_){
                        FontPackerDF fontPacker;
                        fontPacker.create(hDlg, fontInfo_);
                        fontPacker.save("out.bmp", "out.def");
                    }else{
                        FontPacker fontPacker;
                        fontPacker.create(hDlg, fontInfo_);
                        fontPacker.save("out.bmp", "out.def");
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
                size_t l = strlen(logFont.lfFaceName);
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
        }else if(curSel>5){
            fontInfo_.distanceScale_ = 6;
        }else{
            fontInfo_.distanceScale_ = curSel+1;
        }

        curSel = distanceSpread_.getCurSel();
        if(curSel<0){
            fontInfo_.distanceSpread_ = 1;
        }else if(curSel>5){
            fontInfo_.distanceSpread_ = 6;
        }else{
            fontInfo_.distanceSpread_ = curSel+1;
        }
    }
}
