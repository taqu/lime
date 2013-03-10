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
        @brief �R���X�g���N�^
        @param hWnd ... �E�B���h�E�n���h��
        */
        explicit inline Window(HWND hWnd = NULL);

        /// �f�X�g���N�^
        inline ~Window();

        /// �L����
        inline bool valid() const;

        /**
        @brief ���

        �������Ȃ��ŏ㏑��
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
        HWND hWnd_; /// �E�B���h�E�n���h��
    };

    // �R���X�g���N�^
    inline Window::Window(HWND hWnd)
        :hWnd_(hWnd)
    {
    }

    // �f�X�g���N�^
    inline Window::~Window()
    {
    }

    // �L����
    inline bool Window::valid() const
    {
        return (hWnd_ != NULL);
    }

    // ���
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
        @brief �R���X�g���N�^
        @param hWnd ... �E�B���h�E�n���h��
        */
        explicit inline Dialog(HWND hWnd = NULL);

        /**
        @brief �_�C�A���O�A�C�e���擾
        @return �E�B���h�E�n���h��
        @param �A�C�e��ID
        */
        inline HWND getItem(s32 id);
    };

    // �R���X�g���N�^
    inline Dialog::Dialog(HWND hWnd)
        :Window(hWnd)
    {
    }

    // �_�C�A���O�A�C�e���擾
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
        @brief �R���X�g���N�^
        @param hWnd ... �E�B���h�E�n���h��
        */
        explicit inline ComboBox(HWND hWnd = NULL)
            :Window(hWnd)
        {}

        /**
        @brief ������}��
        @return �����񂪑}�����ꂽ�C���f�b�N�X
        @return CB_ERR ... �G���[
        @return CB_ERRSPACE ... ��������i�[����̈悪����Ȃ������ꍇ
        @param str ... ������
        */
        inline LRESULT addString(const Char* str);

        /**
        @brief �����񌟍�
        @return ������̃C���f�b�N�X
        @return CB_ERR ... �����񂪂Ȃ��������A���s
        @param index ... �����J�n�ʒu
        @param str ... ����������
        */
        inline LRESULT findString(s32 index, const Char* str);

        /**
        @brief ������폜
        @return �R���{�{�b�N�X�ɂ���c��̕�����
        @return CB_ERR ... �G���[
        @param index ... �폜����C���f�b�N�X
        */
        inline LRESULT deleteString(s32 index);

        /**
        @brief �����񐔎擾
        @return ������
        @return CB_ERR ... �G���[
        */
        inline LRESULT getCount();

        /**
        @brief �I�𒆃C���f�b�N�X�擾
        @return �C���f�b�N�X
        @return CB_ERR ... �G���[
        */
        inline LRESULT getCurSel();

        /**
        @brief �����񒷎擾
        @return �k��������������������
        @return CB_ERR ... �G���[
        @param index ... �C���f�b�N�X
        @param str ... �������Ԃ��o�b�t�@
        */
        inline LRESULT getLBTextLen(s32 index);

        /**
        @brief ������擾
        @return �k��������������������
        @return CB_ERR ... �G���[
        @param index ... �C���f�b�N�X
        */
        inline LRESULT getLBText(s32 index, Char* str);

        /**
        @brief �\������Ă��鍀�ڂ̃C���f�b�N�X�擾
        @return CB_ERR ... �G���[
        */
        inline LRESULT getTopIndex();

        /**
        @brief ������}��
        @return �}������������̃C���f�b�N�X
        @return CB_ERR ... �G���[
        @return CB_ERRSPACE ... ��������i�[����̈悪����Ȃ������ꍇ
        @param index ... �C���f�b�N�X
        @param str ... ������
        */
        inline LRESULT insertString(s32 index, const Char* str);

        /**
        @brief ���ڃN���A
        */
        inline LRESULT resetContent();

        /**
        @brief ���ڂ�I��
        @return �I���������ڂ̃C���f�b�N�X
        @return CB_ERR ... �G���[
        @param �C���f�b�N�X�B-1�Ȃ�I�����N���A
        */
        inline LRESULT setCurSel(s32 index);

        /**
        @brief �\�����鍀�ڃZ�b�g
        @return CB_ERR ... �G���[
        @param �C���f�b�N�X
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
        @brief �R���X�g���N�^
        @param hWnd ... �E�B���h�E�n���h��
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
