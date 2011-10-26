#ifndef INC_CONFIG_H__
#define INC_CONFIG_H__
/**
@file Config.h
@author t-sakai
@date 2011/08/15 create
*/
#include "Display.h"
#include "Dialog.h"

namespace config
{
    class ConfigDialog : public config::DialogBase<ConfigDialog>
    {
    public:
        static const u32 DefaultRefreshRate = 60;

        enum WindowMode
        {
            WinMode_Windowed = 0,
            WinMode_Fullscreen,
            WinMode_Num,
        };

        enum BPP
        {
            BPP_16 =0,
            BPP_24,
            BPP_Num,
            BPP_FORCEDWORD = 0xFFFFFFFFU,
        };

        ConfigDialog();
        ~ConfigDialog();

        bool initImpl(const Config& config);

        INT_PTR initDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
        INT_PTR command(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

        const Config& getConfig()
        {
            return config_;
        }

        void setWindowMode();
        void setBPP();
        void setResolution();
        /**
        @brief 指定したウィンドウモードが使用できるか
        @param mode
        */
        bool isWindowModeValid(WindowMode mode);

        /**
        @brief 現在のウィンドウモード、指定したビットサイズで使用できるか
        */
        bool isBPPValid(BPP bpp);


        bool addComboText(const DisplayMode& mode);
        bool addComboText(u32 width, u32 height);

        void selectNearestResolution();

        bool existSameSize(s32 num, const DisplayMode& mode);

        void readBack();

        config::Display display_;

        u32 adapter_;
        Config config_;

        BufferFormat formatForBPP_[BPP_Num]; //ビット数毎のバッファフォーマット

        //コントロール
        //---------------------------------
        Dialog dialog_;

        Button windowMode_[WinMode_Num];
        Button bpp_[BPP_Num];

        ComboBox comboResolution_;
        s32 numDispModes_;
        config::DisplayMode* displayModes_;
    };
}
#endif //INC_CONFIG_H__
