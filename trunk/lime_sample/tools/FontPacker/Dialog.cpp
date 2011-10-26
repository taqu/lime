/**
@file Dialog.cpp
@author t-sakai
@date 2011/09/23
*/
#include "Dialog.h"

#include <Commdlg.h>
#include "Resource.h"

namespace font
{
    FontDialog::FontDialog()
    {
        fontInfo_.height_ = 256;
        fontInfo_.width_ = 256;
        fontInfo_.outline_ = 0;
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


        return (INT_PTR)TRUE;
    }

    INT_PTR FontDialog::command(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch(LOWORD(wParam))
        {
        case IDC_OK:
            {
                if(fontInfo_.face_.size()>0){
                    fontInfo_.outline_ = outline_.getCurSel();
                    fontPacker_.create(hDlg, fontInfo_);
                    fontPacker_.save("out.bmp", "out.def");
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
}
