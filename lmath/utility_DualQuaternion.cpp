/**
@file utility_DualQuaternion.cpp
@author t-sakai
@date 2009/11/10 create

=====================================================================================
Original Code:
dqconv.c

  Conversion routines between (regular quaternion, translation) and dual quaternion.

  Version 1.0.0, February 7th, 2007

  Copyright (C) 2006-2007 University of Dublin, Trinity College, All Rights 
  Reserved

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the author(s) be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Author: Ladislav Kavan, kavanl@cs.tcd.ie
=====================================================================================
*/
#include "lmathcore.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "utility.h"

namespace lmath
{
    // 単位四元数と移動ベクトルを単位デュアルクウォータニオンに変換
    void TransQuat2UnitDualQuat(const Quaternion& quat, const Vector3& trans, Quaternion dq[2])
    {
        // 四元数部分
        for(int i=0; i<4; ++i){
            dq[0][i] = quat[i];
        }

        // デュアルパート
        dq[1][0] = -0.5f * ( trans._x * quat.x_ + trans._y * quat.y_ + trans._z * quat.z_);

        dq[1][1] =  0.5f * ( trans._x * quat.w_ + trans._y * quat.z_ - trans._z * quat.y_);

        dq[1][2] =  0.5f * (-trans._x * quat.z_ + trans._y * quat.w_ + trans._z * quat.x_);

        dq[1][3] =  0.5f * ( trans._x * quat.y_ - trans._y * quat.x_ + trans._z * quat.w_);

    }

    // 単位四元数と移動ベクトルを単位デュアルクウォータニオンに変換
    void QuatTrans2UnitDualQuat(const Quaternion& quat, const Vector3& trans, Quaternion dq[2])
    {
        // 四元数部分
        for(int i=0; i<4; ++i){
            dq[0][i] = quat[i];
        }

        // デュアルパート
        dq[1][0] = 0.0f;

        dq[1][1] = 0.5f * trans._x;

        dq[1][2] = 0.5f * trans._y;

        dq[1][3] = 0.5f * trans._z;

    }

    // 単位デュアルクウォータニオンを単位四元数と移動ベクトルに変換
    void UnitDualQuat2QuatTrans(const f32 dq[2][4], Quaternion& quat, Vector3& trans)
    {
        // 四元数
        quat.set(dq[0][0], dq[0][1], dq[0][2], dq[0][3]);

        // 移動ベクトル
        trans._x = 2.0f * (-dq[1][0]*dq[0][1] + dq[1][1]*dq[0][0] - dq[1][2]*dq[0][3] + dq[1][3]*dq[0][2]);
        trans._y = 2.0f * (-dq[1][0]*dq[0][2] + dq[1][1]*dq[0][3] + dq[1][2]*dq[0][0] - dq[1][3]*dq[0][1]);
        trans._z = 2.0f * (-dq[1][0]*dq[0][3] - dq[1][1]*dq[0][2] + dq[1][2]*dq[0][1] + dq[1][3]*dq[0][0]);
    }

    // デュアルクウォータニオンを単位四元数と移動ベクトルに変換
    void DualQuaternion2QuatTrans(const f32 dq[2][4], Quaternion& quat, Vector3& trans)
    {
        f32 length = 0.0f;
        for(int i=0; i<4; ++i){
            length += dq[0][i] * dq[0][i];
        }

        LASSERT(lmath::isEqual(length, 0.0f) == false);

        length = 1.0f / lmath::sqrt(length);

        quat.set(dq[0][0]*length, dq[0][1]*length, dq[0][2]*length, dq[0][3]*length);

        // 移動ベクトル
        length *= 2.0f;
        trans._x = length * (-dq[1][0]*dq[0][1] + dq[1][1]*dq[0][0] - dq[1][2]*dq[0][3] + dq[1][3]*dq[0][2]);
        trans._y = length * (-dq[1][0]*dq[0][2] + dq[1][1]*dq[0][3] + dq[1][2]*dq[0][0] - dq[1][3]*dq[0][1]);
        trans._z = length * (-dq[1][0]*dq[0][3] - dq[1][1]*dq[0][2] + dq[1][2]*dq[0][1] + dq[1][3]*dq[0][0]);
    }
}
