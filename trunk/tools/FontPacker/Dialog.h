#ifndef INC_DIALOG_H__
#define INC_DIALOG_H__
/**
@file Dialog.h
@author t-sakai
@date 2011/09/23 create
*/
#include "DialogBase.h"
#include "FontPacker.h"
namespace font
{
    class FontDialog : public widget::DialogBase<FontDialog>
    {
    public:
        FontDialog();
        ~FontDialog();

        bool initImpl();

        INT_PTR initDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
        INT_PTR command(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

    private:
        void readBack();

        FontInfo fontInfo_;

        widget::ComboBox outline_;

        widget::ComboBox resolution_;
        widget::ComboBox distanceScale_;
        widget::ComboBox distanceSpread_;
        widget::Button checkDistanceField_;
        widget::Button checkASCIIOnly_;
    };
}
#endif //INC_DIALOG_H__
