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
        instance_.znear_ = 1.0f;
        instance_.zfar_ = 200.0f;
    }

    //-------------------------------------------------------------------
    // 
    void Config::perspectiveFovLinearZ(lmath::Matrix44& mat, f32 fovy)
    {
        f32 yscale = 1.0f / lmath::tan(0.5f * fovy);
        f32 xscale = yscale / aspect_;

                                                  mat._elem[0][2] = 0.0f; mat._elem[0][3] = 0.0f;
                                                  mat._elem[1][2] = 0.0f; mat._elem[1][3] = 0.0f;
        mat._elem[2][0] = 0.0f; mat._elem[2][1] = 0.0f; mat._elem[2][2] = 1.0f / (zfar_ - znear_); mat._elem[2][3] = 1.0f;
        mat._elem[3][0] = 0.0f; mat._elem[3][1] = 0.0f; mat._elem[3][2] = znear_ / (znear_ - zfar_); mat._elem[3][3] = 0.0f;

        //ÉÇÅ[ÉhÇ…ÇÊÇ¡ÇƒzâÒì]

        switch(screenMode_)
        {
        case ScreenMode_Rot90:
            {
                mat._elem[0][0] = 0.0f; mat._elem[0][1] = xscale;
                mat._elem[1][0] = -yscale; mat._elem[1][1] = 0.0f;
            }
            break;
        case ScreenMode_Rot180:
            {
                mat._elem[0][0] = -xscale; mat._elem[0][1] = 0.0f;
                mat._elem[1][0] = 0.0f; mat._elem[1][1] = -yscale;
            }
            break;
        case ScreenMode_Rot270:
            {
                mat._elem[0][0] = 0.0f; mat._elem[0][1] = -xscale;
                mat._elem[1][0] = yscale; mat._elem[1][1] = 0.0f;
            }
            break;

        default:
            {
                mat._elem[0][0] = xscale; mat._elem[0][1] = 0.0f;
                mat._elem[1][0] = 0.0f; mat._elem[1][1] = yscale;
            }
            break;
        };
    }
}
