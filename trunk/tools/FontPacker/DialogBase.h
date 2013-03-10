#ifndef INC_DIALOGBASE_H__
#define INC_DIALOGBASE_H__
/**
@file DialogBase.h
@author t-sakai
@date 2011/09/23 create
*/

#include "Widget.h"

namespace widget
{

    template<class Derived>
    class DialogBase
    {
    public:
        template<class T>
        static bool initialize(const T& t)
        {
            if(instance_ == NULL){
                instance_ = WIN_NEW Derived;
            }
            return instance_->initImpl(t);
        }

        static bool initialize()
        {
            if(instance_ == NULL){
                instance_ = WIN_NEW Derived;
            }
            return instance_->initImpl();
        }

        static void terminate()
        {
            WIN_DELETE(instance_);
        }

        static Derived& getInstance()
        {
            return *instance_;
        }

        static INT_PTR create(HINSTANCE hInst, LPCTSTR lpTemplate);

    protected:
        static INT_PTR CALLBACK proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

        //virtual INT_PTR initDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) =0;
        //virtual INT_PTR command(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) =0;


        static Derived* instance_;

        DialogBase();
        virtual ~DialogBase();
    };

    template<class Derived>
    Derived* DialogBase<Derived>::instance_ = NULL;

    template<class Derived>
    INT_PTR DialogBase<Derived>::create(HINSTANCE hInst, LPCTSTR lpTemplate)
    {
        WIN_ASSERT(instance_ != NULL);
        
        INT_PTR ret = DialogBox(hInst, lpTemplate, NULL, proc);

        return ret;
    }

    template<class Derived>
    DialogBase<Derived>::DialogBase()
    {
    }

    template<class Derived>
    DialogBase<Derived>::~DialogBase()
    {
    }

    template<class Derived>
    INT_PTR DialogBase<Derived>::proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
        UNREFERENCED_PARAMETER(lParam);
        switch (message)
        {
        case WM_INITDIALOG:
            return instance_->initDialog(hDlg, message, wParam, lParam);

        case WM_COMMAND:
            return instance_->command(hDlg, message, wParam, lParam);
        }
        return (INT_PTR)FALSE;
    }
}

#endif //INC_DIALOGBASE_H__
