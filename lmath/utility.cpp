/**
@file utility.cpp
@author t-sakai
@date 2009/11/10 create
*/
#include "lmathcore.h"
#include "Matrix43.h"
#include "Quaternion.h"
#include "utility.h"

namespace lmath
{
    // 慣性座標からオブジェクト座標へのオイラー角を取得
    void getEulerInertialToObject(f32& head, f32& pitch, f32& bank, const lmath::Matrix43& m)
    {
        f32 sinPitch = -m._elem[1][2];
        if(sinPitch <= 1.0f){
            pitch = -PI_2;
        }else if(sinPitch >= 1.0f){
            pitch = PI_2;
        }else{
            pitch = asin(sinPitch);
        }

        // sin(pitch)が１ならバンクとヘディングがジンバルロック
        if(absolute(sinPitch) > F32_ANGLE_LIMIT1){
            // バンクを0として、ヘディングを計算
            bank = 0.0f;
            head = atan2(-m._elem[2][0], m._elem[0][0]);
        }else{
            head = atan2(m._elem[0][2], m._elem[2][2]);

            bank = atan2(m._elem[1][0], m._elem[1][1]);
        }
    }

    // 慣性座標からオブジェクト座標へのオイラー角を取得
    void getEulerInertialToObject(f32& head, f32& pitch, f32& bank, const lmath::Quaternion& q)
    {
        f32 sinPitch = -2.0f * (q.y_*q.z_ + q.w_*q.x_);

        // sin(pitch)が１ならバンクとヘディングがジンバルロック
        if(absolute(sinPitch) > F32_ANGLE_LIMIT1){
            pitch = PI_2 * sinPitch;

            // バンクを0として、ヘディングを計算
            bank = 0.0f;
            head = atan2(-q.x_*q.z_ - q.w_*q.y_, 0.5f-q.y_*q.y_-q.z_*q.z_);
        }else{
            pitch = asin(sinPitch);
            head = atan2(q.x_*q.z_ - q.w_*q.y_, 0.5f-q.x_*q.x_-q.y_*q.y_);
            bank = atan2(q.x_*q.y_ - q.w_*q.z_, 0.5f-q.x_*q.x_-q.z_*q.z_);
        }
    }


    // オブジェクト座標から慣性座標へのオイラー角を取得
    void getEulerObjectToInertial(f32& head, f32& pitch, f32& bank, const lmath::Matrix43& m)
    {
        f32 sinPitch = -m._elem[2][1];
        if(sinPitch <= 1.0f){
            pitch = -PI_2;
        }else if(sinPitch >= 1.0f){
            pitch = PI_2;
        }else{
            pitch = asin(sinPitch);
        }

        // sin(pitch)が１ならバンクとヘディングがジンバルロック
        if(absolute(sinPitch) > F32_ANGLE_LIMIT1){
            // バンクを0として、ヘディングを計算
            bank = 0.0f;
            head = atan2(-m._elem[0][2], m._elem[0][0]);
        }else{
            head = atan2(m._elem[2][0], m._elem[2][2]);

            bank = atan2(m._elem[0][1], m._elem[1][1]);
        }
    }

    // オブジェクト座標から慣性座標へのオイラー角を取得
    void getEulerObjectToInertial(f32& head, f32& pitch, f32& bank, const lmath::Quaternion& q)
    {
        f32 sinPitch = -2.0f * (q.y_*q.z_ - q.w_*q.x_);

        // sin(pitch)が１ならバンクとヘディングがジンバルロック
        if(absolute(sinPitch) > F32_ANGLE_LIMIT1){
            pitch = PI_2 * sinPitch;

            // バンクを0として、ヘディングを計算
            bank = 0.0f;
            head = atan2(-q.x_*q.z_ + q.w_*q.y_, 0.5f-q.y_*q.y_-q.z_*q.z_);
        }else{
            pitch = asin(sinPitch);
            head = atan2(q.x_*q.z_ + q.w_*q.y_, 0.5f-q.x_*q.x_-q.y_*q.y_);
            bank = atan2(q.x_*q.y_ + q.w_*q.z_, 0.5f-q.x_*q.x_-q.z_*q.z_);
        }
    }


    // オイラー角から四元数を取得
    void getQuaternionObjectToInertial(Quaternion& q, f32 head, f32 pitch, f32 bank)
    {
        //角度半分にする
        head *= 0.5f;
        pitch *= 0.5f;
        bank *= 0.5f;

        f32 csh = cosf(head);
        f32 snh = sinf(head);

        f32 csp = cosf(pitch);
        f32 snp = sinf(pitch);

        f32 csb = cosf(bank);
        f32 snb = sinf(bank);

        q.w_ = csh*csp*csb + snh*snp*snb;
        q.x_ = csh*snp*csb + snh*csp*snb;
        q.y_ = snh*csp*csb - csh*snp*snb;
        q.z_ = csh*csp*snb - snh*snp*csb;
    }

    // オイラー角から四元数を取得
    void getQuaternionInertialToObject(Quaternion& q, f32 head, f32 pitch, f32 bank)
    {
        //角度半分にする
        head *= 0.5f;
        pitch *= 0.5f;
        bank *= 0.5f;

        f32 csh = cosf(head);
        f32 snh = sinf(head);

        f32 csp = cosf(pitch);
        f32 snp = sinf(pitch);

        f32 csb = cosf(bank);
        f32 snb = sinf(bank);

        q.w_ =  csh*csp*csb + snh*snp*snb;
        q.x_ = -csh*snp*csb - snh*csp*snb;
        q.y_ =  csh*snp*snb - snh*csp*csb;
        q.z_ =  snh*snp*csb - csh*csp*snb;
    }
}
