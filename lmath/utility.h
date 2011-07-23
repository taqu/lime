#ifndef INC_LMATH_UTILITY_H__
#define INC_LMATH_UTILITY_H__
/**
@file utility.h
@author t-sakai
@date 2009/11/10 create

*/
namespace lmath
{
    class Vector3;
    class Matrix34;
    class Quaternion;

    /**
    @brief 単位四元数と移動ベクトルを単位デュアルクウォータニオンに変換

    移動してから、回転
    */
    void TransQuat2UnitDualQuat(const Quaternion& quat, const Vector3& trans, Quaternion dq[2]);

    /**
    @brief 単位四元数と移動ベクトルを単位デュアルクウォータニオンに変換

    回転してから、移動
    */
    void QuatTrans2UnitDualQuat(const Quaternion& quat, const Vector3& trans, Quaternion dq[2]);


    /**
    @brief 単位デュアルクウォータニオンを単位四元数と移動ベクトルに変換
    */
    void UnitDualQuat2QuatTrans(const f32 dq[2][4], Quaternion& quat, Vector3& trans);


    /**
    @brief デュアルクウォータニオンを単位四元数と移動ベクトルに変換

    デュアルクウォータニオンは非ゼロの非デュアルパート（四元数）の場合
    */
    void DualQuaternion2QuatTrans(const f32 dq[2][4], Quaternion& quat, Vector3& trans);

    /**
    @brief 慣性座標からオブジェクト座標へのオイラー角を取得

    慣性座標は、原点がオブジェクト座標と等しく、軸がワールド座標と平行な座標系
    */
    void getEulerInertialToObject(f32& head, f32& pitch, f32& bank, const lmath::Matrix34& m);

    /**
    @brief 慣性座標からオブジェクト座標へのオイラー角を取得

    慣性座標は、原点がオブジェクト座標と等しく、軸がワールド座標と平行な座標系
    */
    void getEulerInertialToObject(f32& head, f32& pitch, f32& bank, const lmath::Quaternion& q);


    /**
    @brief オブジェクト座標から慣性座標へのオイラー角を取得

    慣性座標は、原点がオブジェクト座標と等しく、軸がワールド座標と平行な座標系
    */
    void getEulerObjectToInertial(f32& head, f32& pitch, f32& bank, const lmath::Matrix34& m);

    /**
    @brief オブジェクト座標から慣性座標へのオイラー角を取得

    慣性座標は、原点がオブジェクト座標と等しく、軸がワールド座標と平行な座標系
    */
    void getEulerObjectToInertial(f32& head, f32& pitch, f32& bank, const lmath::Quaternion& q);

    /**
    @brief オイラー角から四元数を取得
    */
    void getQuaternionObjectToInertial(Quaternion& q, f32 head, f32 pitch, f32 bank);

    /**
    @brief オイラー角から四元数を取得
    */
    void getQuaternionInertialToObject(Quaternion& q, f32 head, f32 pitch, f32 bank);
}

#endif //INC_LMATH_UTILITY_H__
