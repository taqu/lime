#ifndef INC_LFW_LINPUT_H_
#define INC_LFW_LINPUT_H_
/**
@file linput.h
@author t-sakai
@date 2009/05/13 create
*/
#include "../Config.h"
#include <lcore/lcore.h>

#ifdef LFW_INPUT_DIRECT_INPUT
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
typedef struct HWND__* WINDOW_HANDLE;
typedef IDirectInput8 DirectInput;
typedef IDirectInputDevice8 DirectInputDevice;
#endif


#define LINPUT_SAFE_RELEASE(p) {if(p!=NULL){(p)->Release();(p)=NULL;}}

namespace linput
{
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;
    using lcore::u8;
    using lcore::u16;
    using lcore::u32;
    using lcore::f32;
    using lcore::f64;

    class Device;
    class Keyboard;
    class Mouse;
    class Joystick;

    enum DeviceType
    {
        DevType_Keyboard =0,
        DevType_Mouse,
        DevType_Joystick,
        DevType_Num,
    };

    enum Error
    {
        Error_None = 0,
        Error_Init,
    };

    struct InitParam
    {
        InitParam()
            :windowHandle_(NULL)
            ,initDevices_{false,false,false}
        {
        }

        WINDOW_HANDLE windowHandle_;
        bool initDevices_[DevType_Num];
    };


    //----------------------------------------------------
    /**
    @brief ビットフラグセット
    */
    template<u32 SizeInByte>
    class BitSet
    {
    public:
        typedef BitSet<SizeInByte> this_type;

        BitSet()
        {
            clear();
        }

        ~BitSet()
        {
        }

        void clear()
        {
            for(u32 i=0; i<SizeInByte; ++i){
                bits_[i] = 0;
            }
        }

        /**
        @brief フラグチェック
        @param flag
        */
        inline bool check(u32 index) const
        {
            u8 bit = index & 0x07U;
            index = index >> 3;
            return ((bits_[index] & (0x01U<<bit)) != 0);
        }

        /**
        @brief フラグ立てる
        */
        inline void set(u32 index)
        {
            u8 bit = index & 0x07U;
            index = index >> 3;
            bits_[index] |= (0x01U<<bit);
        }

        /**
        @brief フラグ
        */
        inline void set(u32 index, bool flag)
        {
            if(flag){
                set(index);
            } else{
                reset(index);
            }
        }

        /**
        @brief フラグ降ろす
        */
        inline void reset(u32 index)
        {
            u8 bit = index & 0x07U;
            index = index >> 3;
            bits_[index] &= ~(0x01U<<bit);
        }

        inline u8 getByte(u32 index) const{ return bits_[index]; }
    private:
        u8 bits_[SizeInByte];
    };

    //----------------------------------------------------
    /**
    @brief ビットフラグセット比較
    */
    template<u32 SizeInByte>
    bool compare(const BitSet<SizeInByte>& b0, const BitSet<SizeInByte>& b1)
    {
        for(u32 i=0; i<SizeInByte; ++i){
            if(b0.getByte(i) != b1.getByte(i)){
                return false;
            }
        }
        return true;
    }
}

#if defined(LFW_INPUT_DIRECT_INPUT)
#include "DirectInput/Input.h"
#endif

#endif //INC_LFW_LINPUTENUM_H_
