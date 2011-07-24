/**
@file Config.cpp
@author t-sakai
@date 2011/03/31 create
*/
#include "stdafx.h"
#include "Config.h"

namespace egda
{
    Config Config::instance_;

    void Config::initialize()
    {
        instance_.screenMode_ = ScreenMode_Rot0;
        instance_.width_ = 300.0f;
        instance_.height_ = 400.0f;
        instance_.aspect_ = (300.0f/400.0f);
        instance_.znear_ = 0.5f;
        instance_.zfar_ = 2000.0f;
    }

    //-------------------------------------------------------------------
    // 
    void Config::perspectiveFovLinearZ(lmath::Matrix44& mat, f32 fovy)
    {
        f32 yscale = 1.0f / lmath::tan(0.5f * fovy);
        f32 xscale = yscale / aspect_;

                                                  mat.m_[0][2] = 0.0f; mat.m_[0][3] = 0.0f;
                                                  mat.m_[1][2] = 0.0f; mat.m_[1][3] = 0.0f;
        mat.m_[2][0] = 0.0f; mat.m_[2][1] = 0.0f; mat.m_[2][2] = 1.0f / (zfar_ - znear_); mat.m_[2][3] = znear_ / (znear_ - zfar_);
        mat.m_[3][0] = 0.0f; mat.m_[3][1] = 0.0f; mat.m_[3][2] = 1.0f; mat.m_[3][3] = 0.0f;

        //ÉÇÅ[ÉhÇ…ÇÊÇ¡ÇƒzâÒì]

        switch(screenMode_)
        {
        case ScreenMode_Rot90:
            {
                mat.m_[0][0] = 0.0f; mat.m_[0][1] = -yscale;
                mat.m_[1][0] = xscale; mat.m_[1][1] = 0.0f;
            }
            break;
        case ScreenMode_Rot180:
            {
                mat.m_[0][0] = -xscale; mat.m_[0][1] = 0.0f;
                mat.m_[1][0] = 0.0f; mat.m_[1][1] = -yscale;
            }
            break;
        case ScreenMode_Rot270:
            {
                mat.m_[0][0] = 0.0f; mat.m_[0][1] = yscale;
                mat.m_[1][0] = -xscale; mat.m_[1][1] = 0.0f;
            }
            break;

        default:
            {
                mat.m_[0][0] = xscale; mat.m_[0][1] = 0.0f;
                mat.m_[1][0] = 0.0f; mat.m_[1][1] = yscale;
            }
            break;
        };
    }
}
