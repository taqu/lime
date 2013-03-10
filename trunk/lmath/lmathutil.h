#ifndef INC_LMATH_LMATHUTIL_H__
#define INC_LMATH_LMATHUTIL_H__
/**
@file lmathutil.h
@author t-sakai
@date 2010/09/09 create

*/
#include "lmathcore.h"

namespace lmath
{
    class Vector3;
    class Vector4;
    class Matrix34;
    class Quaternion;

    void calcBBox(Vector3& bmin, Vector3& bmax, const Vector3& v0, const Vector3& v1, const Vector3& v2);

    void calcMedian(Vector3& median, const Vector3& v0, const Vector3& v1, const Vector3& v2);

    void calcAABBPoints(Vector4* AABB, const Vector4& aabbMin, const Vector4& aabbMax);

    /**
    @brief Ferguson-Coons�Ȑ�
    @param dst ... �o�́B
    @param t ... 0-1
    @param p0 ... �_0
    @param v0 ... �_0�ł̑��x
    @param p1 ... �_1
    @param v1 ... �_1�ł̑��x
    */
    void fergusonCoonsCurve(Vector4& dst, f32 t, const Vector4& p0, const Vector4& v0, const Vector4& p1, const Vector4& v1);

    /**
    @brief Bezier�Ȑ�
    @param dst ... �o�́B
    @param t ... 0-1
    @param p0 ... �_0
    @param p1 ... �_1
    @param p2 ... �_2
    @param p3 ... �_3
    */
    void cubicBezierCurve(Vector4& dst, f32 t, const Vector4& p0, const Vector4& p1, const Vector4& p2, const Vector4& p3);

    /**
    @brief Catmull-Rom�Ȑ�
    @param dst ... �o�́B
    @param t ... 0-1
    @param p0 ... �_0
    @param p1 ... �_1
    @param p2 ... �_2
    @param p3 ... �_3
    */
    void catmullRomCurve(Vector4& dst, f32 t, const Vector4& p0, const Vector4& p1, const Vector4& p2, const Vector4& p3);
    void catmullRomCurveStartPoint(Vector4& dst, f32 t, const Vector4& p1, const Vector4& p2, const Vector4& p3);
    void catmullRomCurveEndPoint(Vector4& dst, f32 t, const Vector4& p0, const Vector4& p1, const Vector4& p2);



    //--------------------------------------------------------
    //--- �l����
    //--------------------------------------------------------
    /**
    @brief �P�ʎl�����ƈړ��x�N�g����P�ʃf���A���N�E�H�[�^�j�I���ɕϊ�

    �ړ����Ă���A��]
    */
    void TransQuat2UnitDualQuat(const Quaternion& quat, const Vector3& trans, Quaternion dq[2]);

    /**
    @brief �P�ʎl�����ƈړ��x�N�g����P�ʃf���A���N�E�H�[�^�j�I���ɕϊ�

    ��]���Ă���A�ړ�
    */
    void QuatTrans2UnitDualQuat(const Quaternion& quat, const Vector3& trans, Quaternion dq[2]);


    /**
    @brief �P�ʃf���A���N�E�H�[�^�j�I����P�ʎl�����ƈړ��x�N�g���ɕϊ�
    */
    void UnitDualQuat2QuatTrans(const f32 dq[2][4], Quaternion& quat, Vector3& trans);


    /**
    @brief �f���A���N�E�H�[�^�j�I����P�ʎl�����ƈړ��x�N�g���ɕϊ�

    �f���A���N�E�H�[�^�j�I���͔�[���̔�f���A���p�[�g�i�l�����j�̏ꍇ
    */
    void DualQuaternion2QuatTrans(const f32 dq[2][4], Quaternion& quat, Vector3& trans);

    /**
    @brief �������W����I�u�W�F�N�g���W�ւ̃I�C���[�p���擾

    �������W�́A���_���I�u�W�F�N�g���W�Ɠ������A�������[���h���W�ƕ��s�ȍ��W�n
    */
    void getEulerInertialToObject(f32& head, f32& pitch, f32& bank, const lmath::Matrix34& m);

    /**
    @brief �������W����I�u�W�F�N�g���W�ւ̃I�C���[�p���擾

    �������W�́A���_���I�u�W�F�N�g���W�Ɠ������A�������[���h���W�ƕ��s�ȍ��W�n
    */
    void getEulerInertialToObject(f32& head, f32& pitch, f32& bank, const lmath::Quaternion& q);


    /**
    @brief �I�u�W�F�N�g���W���犵�����W�ւ̃I�C���[�p���擾

    �������W�́A���_���I�u�W�F�N�g���W�Ɠ������A�������[���h���W�ƕ��s�ȍ��W�n
    */
    void getEulerObjectToInertial(f32& head, f32& pitch, f32& bank, const lmath::Matrix34& m);

    /**
    @brief �I�u�W�F�N�g���W���犵�����W�ւ̃I�C���[�p���擾

    �������W�́A���_���I�u�W�F�N�g���W�Ɠ������A�������[���h���W�ƕ��s�ȍ��W�n
    */
    void getEulerObjectToInertial(f32& head, f32& pitch, f32& bank, const lmath::Quaternion& q);

    /**
    @brief �I�C���[�p����l�������擾
    */
    void getQuaternionObjectToInertial(Quaternion& q, f32 head, f32 pitch, f32 bank);

    /**
    @brief �I�C���[�p����l�������擾
    */
    void getQuaternionInertialToObject(Quaternion& q, f32 head, f32 pitch, f32 bank);
}
#endif //INC_LMATH_LMATHUTIL_H__
