#ifndef INC_WIDGET_H__
#define INC_WIDGET_H__
/**
@file Widget.h
@author t-sakai
@date 2011/08/15 create
*/
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <cassert>

#define WIN_ASSERT(exp) assert(exp)

#define WIN_NEW new
#define WIN_DELETE(ptr) {delete (ptr); (ptr) = NULL;}
#define WIN_DELETE_ARRAY(ptr) {delete[] (ptr); (ptr) = NULL;}

namespace widget
{
#ifdef _WIN32
    typedef __int8 s8;
    typedef __int16 s16;
    typedef __int32 s32;
    typedef __int64 s64;

    typedef unsigned __int8 u8;
    typedef unsigned __int16 u16;
    typedef unsigned __int32 u32;
    typedef unsigned __int64 u64;

    typedef float f32;
    typedef double f64;

    typedef char Char;

    typedef HINSTANCE HMODULE;

#elif defined(ANDROID) || defined(__linux__)
    typedef int8_t s8;
    typedef int16_t s16;
    typedef int64_t s64;

    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint64_t u64;

    typedef float f32;
    typedef double f64;

    typedef char Char;

    typedef void* HMODULE;

#else
    typedef char s8;
    typedef short s16;
    typedef long long s64;

    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef unsigned long long u64;

    typedef float f32;
    typedef double f64;

    typedef char Char;

    typedef void* HMODULE;
#endif


    template<class T, class U>
    inline LRESULT SendMessageTmp(HWND hWnd, u32 msg, T wparam, U lparam)
    {
        return SendMessage(hWnd, msg, (WPARAM)wparam, (LPARAM)lparam);
    }

    //---------------------------------------------------
    //---
    //--- Window
    //---
    //---------------------------------------------------
    class Window
    {
    public:
        /**
        @brief コンストラクタ
        @param hWnd ... ウィンドウハンドル
        */
        explicit inline Window(HWND hWnd = NULL);

        /// デストラクタ
        inline ~Window();

        /// 有効か
        inline bool valid() const;

        /**
        @brief 代入

        何もしないで上書き
        */
        inline void set(HWND hWnd);

        HWND getHWnd()
        {
            WIN_ASSERT(hWnd_ != NULL);
            return hWnd_;
        }

        const HWND getHWnd() const
        {
            WIN_ASSERT(hWnd_ != NULL);
            return hWnd_;
        }

        inline BOOL enable(BOOL e);
    protected:
        HWND hWnd_; /// ウィンドウハンドル
    };

    // コンストラクタ
    inline Window::Window(HWND hWnd)
        :hWnd_(hWnd)
    {
    }

    // デストラクタ
    inline Window::~Window()
    {
    }

    // 有効か
    inline bool Window::valid() const
    {
        return (hWnd_ != NULL);
    }

    // 代入
    void Window::set(HWND hWnd)
    {
        hWnd_ = hWnd;
    }

    inline BOOL Window::enable(BOOL e)
    {
        return EnableWindow(hWnd_, e);
    }


    //---------------------------------------------------
    //---
    //--- Dialog
    //---
    //---------------------------------------------------
    class Dialog : public Window
    {
    public:
        /**
        @brief コンストラクタ
        @param hWnd ... ウィンドウハンドル
        */
        explicit inline Dialog(HWND hWnd = NULL);

        /**
        @brief ダイアログアイテム取得
        @return ウィンドウハンドル
        @param アイテムID
        */
        inline HWND getItem(s32 id);
    };

    // コンストラクタ
    inline Dialog::Dialog(HWND hWnd)
        :Window(hWnd)
    {
    }

    // ダイアログアイテム取得
    inline HWND Dialog::getItem(s32 id)
    {
        return GetDlgItem(getHWnd(), id);
    }


    //---------------------------------------------------
    //---
    //--- ComboBox
    //---
    //---------------------------------------------------
    class ComboBox : public Window
    {
    public:
        /**
        @brief コンストラクタ
        @param hWnd ... ウィンドウハンドル
        */
        explicit inline ComboBox(HWND hWnd = NULL)
            :Window(hWnd)
        {}

        /**
        @brief 文字列挿入
        @return 文字列が挿入されたインデックス
        @return CB_ERR ... エラー
        @return CB_ERRSPACE ... 文字列を格納する領域が足りなかった場合
        @param str ... 文字列
        */
        inline LRESULT addString(const Char* str);

        /**
        @brief 文字列検索
        @return 文字列のインデックス
        @return CB_ERR ... 文字列がなかったか、失敗
        @param index ... 検索開始位置
        @param str ... 検索文字列
        */
        inline LRESULT findString(s32 index, const Char* str);

        /**
        @brief 文字列削除
        @return コンボボックスにある残りの文字列数
        @return CB_ERR ... エラー
        @param index ... 削除するインデックス
        */
        inline LRESULT deleteString(s32 index);

        /**
        @brief 文字列数取得
        @return 文字列数
        @return CB_ERR ... エラー
        */
        inline LRESULT getCount();

        /**
        @brief 選択中インデックス取得
        @return インデックス
        @return CB_ERR ... エラー
        */
        inline LRESULT getCurSel();

        /**
        @brief 文字列長取得
        @return ヌル文字を除いた文字列長
        @return CB_ERR ... エラー
        @param index ... インデックス
        @param str ... 文字列を返すバッファ
        */
        inline LRESULT getLBTextLen(s32 index);

        /**
        @brief 文字列取得
        @return ヌル文字を除いた文字列長
        @return CB_ERR ... エラー
        @param index ... インデックス
        */
        inline LRESULT getLBText(s32 index, Char* str);

        /**
        @brief 表示されている項目のインデックス取得
        @return CB_ERR ... エラー
        */
        inline LRESULT getTopIndex();

        /**
        @brief 文字列挿入
        @return 挿入した文字列のインデックス
        @return CB_ERR ... エラー
        @return CB_ERRSPACE ... 文字列を格納する領域が足りなかった場合
        @param index ... インデックス
        @param str ... 文字列
        */
        inline LRESULT insertString(s32 index, const Char* str);

        /**
        @brief 項目クリア
        */
        inline LRESULT resetContent();

        /**
        @brief 項目を選択
        @return 選択した項目のインデックス
        @return CB_ERR ... エラー
        @param インデックス。-1なら選択をクリア
        */
        inline LRESULT setCurSel(s32 index);

        /**
        @brief 表示する項目セット
        @return CB_ERR ... エラー
        @param インデックス
        */
        inline LRESULT setTopIndex(s32 index);
    };

    inline LRESULT ComboBox::addString(const Char* str)
    {
        WIN_ASSERT(str != NULL);
        return SendMessageTmp(getHWnd(), CB_ADDSTRING, 0, str);
    }

    inline LRESULT ComboBox::deleteString(s32 index)
    {
        return SendMessageTmp(getHWnd(), CB_DELETESTRING, index, 0);
    }

    inline LRESULT ComboBox::findString(s32 index, const Char* str)
    {
        return SendMessageTmp(getHWnd(), CB_FINDSTRING, index, str);
    }

    inline LRESULT ComboBox::getCount()
    {
        return SendMessageTmp(getHWnd(), CB_GETCOUNT, 0, 0);
    }

    inline LRESULT ComboBox::getCurSel()
    {
        return SendMessageTmp(getHWnd(), CB_GETCURSEL, 0, 0);
    }

    inline LRESULT ComboBox::getLBTextLen(s32 index)
    {
        return SendMessageTmp(getHWnd(), CB_GETLBTEXTLEN, index, 0);
    }

    inline LRESULT ComboBox::getLBText(s32 index, Char* str)
    {
        return SendMessageTmp(getHWnd(), CB_GETLBTEXT, index, str);
    }

    inline LRESULT ComboBox::getTopIndex()
    {
        return SendMessageTmp(getHWnd(), CB_GETTOPINDEX, 0, 0);
    }

    inline LRESULT ComboBox::insertString(s32 index, const Char* str)
    {
        return SendMessageTmp(getHWnd(), CB_INSERTSTRING, index, str);
    }

    inline LRESULT ComboBox::resetContent()
    {
        return SendMessageTmp(getHWnd(), CB_RESETCONTENT, 0, 0);
    }

    inline LRESULT ComboBox::setCurSel(s32 index)
    {
        return SendMessageTmp(getHWnd(), CB_SETCURSEL, index, 0);
    }

    inline LRESULT ComboBox::setTopIndex(s32 index)
    {
        return SendMessageTmp(getHWnd(), CB_SETTOPINDEX, index, 0);
    }


    //---------------------------------------------------
    //---
    //--- Button
    //---
    //---------------------------------------------------
    class Button : public Window
    {
    public:
        /**
        @brief コンストラクタ
        @param hWnd ... ウィンドウハンドル
        */
        explicit inline Button(HWND hWnd = NULL)
            :Window(hWnd)
        {}

        inline LRESULT getCheck();
        inline LRESULT setCheck();
        inline LRESULT setUnCheck();
    };

    inline LRESULT Button::getCheck()
    {
        return SendMessageTmp(getHWnd(), BM_GETCHECK, 0, 0);
    }

    inline LRESULT Button::setCheck()
    {
        return SendMessageTmp(getHWnd(), BM_SETCHECK, BST_CHECKED, 0);
    }

    inline LRESULT Button::setUnCheck()
    {
        return SendMessageTmp(getHWnd(), BM_SETCHECK, BST_UNCHECKED, 0);
    }

}
#endif //INC_WIDGET_H__
