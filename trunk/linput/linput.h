#ifndef INC_LINPUT_H__
#define INC_LINPUT_H__
/**
@file linput.h
@author t-sakai
@date 2009/05/13 create
*/
#include "linputEnum.h"
#include "linputcore.h"

#if defined(LIME_DINPUT)
#include "DirectInput/InputImpl.h"
#endif

namespace linput
{
    class Device;
    class Keyboard;
    class Mouse;

    struct InitParam;

    /**
    Inputマネージャ
    */
    class Input
    {
    public:
        /**
        */
        static void initialize(InitParam& param);
        static void terminate();

        static Input& getInstance();

        void update();

        const Keyboard* getKeyboard() const{ return keyboard_;}
        const Mouse* getMouse() const{ return mouse_;}

    private:
        Input();
        ~Input();

        Input(const Input&);
        Input& operator=(const Input&);

        InputImpl impl_;

        Keyboard *keyboard_;
        Mouse *mouse_;
    };
}

#endif //INC_LINPUT_H__
