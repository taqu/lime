/**
@file DualQuaternion.cpp
@author t-sakai
@date 2009/11/16
*/
#include "DualQuaternion.h"
#include "Matrix34.h"

namespace lmath
{
    void DualQuaternion::invert()
    {
        Quaternion invRot = dq_[0];
        invRot.invert();
        dq_[0] = invRot;

        invRot *= invRot;

        dq_[1] *= -invRot;
    }

    DualQuaternion& DualQuaternion::mul(const DualQuaternion& d0, const DualQuaternion& d1)
    {
        lmath::Quaternion rot = d0.dq_[0];
        rot *= d1.dq_[0];

        lmath::Quaternion t0;
        t0.mul(d0.dq_[0], d1.dq_[1]);

        lmath::Quaternion t1;
        t1.mul(d0.dq_[1], d1.dq_[0]);

        dq_[0] = rot;
        dq_[1] = t0;
        dq_[1] += t1;

        return *this;
    }

    void DualQuaternion::getMatrix(lmath::Matrix34& mat) const
    {
        f32 length2 = dq_[0].lengthSqr();

        LASSERT(lmath::isEqual(length2, 0.0f) == false);

        const f32 w = dq_[0].w_;
        const f32 x = dq_[0].x_;
        const f32 y = dq_[0].y_;
        const f32 z = dq_[0].z_;

        const f32 t0 = dq_[1].w_;
        const f32 t1 = dq_[1].x_;
        const f32 t2 = dq_[1].y_;
        const f32 t3 = dq_[1].z_;

        mat.m_[0][0] = w*w + x*x - y*y - z*z;
        mat.m_[0][1] = 2.0f * (x*y - w*z);
        mat.m_[0][2] = 2.0f * (x*z + w*y);

        mat.m_[1][0] = 2.0f * (x*y + w*z);
        mat.m_[1][1] = w*w + y*y - x*x - z*z;
        mat.m_[1][2] = 2.0f * (y*z - w*x);

        mat.m_[2][0] = 2.0f * (x*z - w*y);
        mat.m_[2][1] = 2.0f * (y*z + w*x);
        mat.m_[2][2] = w*w + z*z - x*x - y*y;

        mat.m_[0][3] = 2.0f * (-t0*x + t1*w - t2*z + t3*y);
        mat.m_[1][3] = 2.0f * (-t0*y + t1*z - t3*x + t2*w);
        mat.m_[2][3] = 2.0f * (-t0*z + t2*x + t3*w - t1*y);

        f32 invLeng2 = 1.0f/length2;
        mat *= invLeng2;
    }


    void DualQuaternion::transform(lmath::Vector3& v) const
    {
        // fast dqs 
   // float length = sqrt(dual[0].w * dual[0].w + dual[0].x * dual[0].x + dual[0].y * dual[0].y + dual[0].z * dual[0].z) ;
    //dual = dual / length ; 
    //position = input.vPos.xyz + 2.0 * cross(dual[0].xyz, cross(dual[0].xyz, input.vPos.xyz) + dual[0].w * input.vPos.xyz) ;
    //translation = 2.0 * (dual[0].w * dual[1].xyz - dual[1].w * dual[0].xyz + cross(dual[0].xyz, dual[1].xyz)) ; 
    //position += translation ; 

        f32 rLength = dq_[0].lengthSqr();
        rLength = lmath::rsqrt_22bit(rLength);

        f32 rw = dq_[0].w_ * rLength;
        f32 tw = dq_[1].w_ * rLength;

        Vector3 rot(dq_[0].x_, dq_[0].y_, dq_[0].z_);
        rot *= rLength;

        Vector3 trans(dq_[1].x_, dq_[1].y_, dq_[1].z_);
        trans *= rLength;

        Vector3 v0;
        v0.cross(rot, v);
        Vector3 v1(v);
        v1 *= rw;

        v0 += v1;

        v0.cross(rot, v0);
        v0 *= 2.0f;
        v += v0;

        v0.cross(rot, trans);
        rot *= tw;
        trans *= rw;
        v1 = trans;
        v1 -= rot;
        v1 += v0;
        v1 *= 2.0f;

        v += v1;
    }


    // 線形補間。q = (1-t)*q1 + t*q2
    DualQuaternion& DualQuaternion::lerp(const DualQuaternion& q0, const DualQuaternion& q1, f32 t)
    {

        DualQuaternion ret;
        ret.dq_[0].lerp(q0.dq_[0], q1.dq_[0], t);

        ret.dq_[1] = q0.dq_[1];
        ret.dq_[1] *= 1.0f -t;

        Quaternion t1 = q1.dq_[1];
        t1 *= t;
        ret.dq_[1] += t1;

        *this = ret;

        return *this;
    }
}
