#include <iostream>
#include "lmath.h"
#include "Quaternion.h"

using namespace lmath;

namespace
{
    std::ostream& operator<<(std::ostream& os, const lmath::Vector3& v)
    {
        os << v.x_ << ", " << v.y_ << ", " << v.z_ << "\n";
        os << std::endl;
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const lmath::Vector4& v)
    {
        os << v.x_ << ", " << v.y_ << ", " << v.z_ << ", " << v.w_ << "\n";
        os << std::endl;
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const lmath::Matrix34& m)
    {
        for(int i=0; i<3; ++i){
            os << m.m_[i][0] << ", " << m.m_[i][1] << ", " << m.m_[i][2] << ", " << m.m_[i][3] << "\n";
        }
        os << std::endl;
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const lmath::Matrix44& m)
    {
        for(int i=0; i<4; ++i){
            os << m.m_[i][0] << ", " << m.m_[i][1] << ", " << m.m_[i][2] << ", " << m.m_[i][3] << "\n";
        }
        os << std::endl;
        return os;
    }
}

int main(int argc, char** argv)
{
    std::cout << "===== Matrix34 =====" << std::endl;
    {
        Vector3 v;

        Matrix34 mat;

        v.set(0.0f, 1.0f, 0.0f);
        mat.identity();
        mat.rotateZ(PI*0.5f);
        v.mul(mat, v);
        std::cout << v;

        v.set(0.0f, 1.0f, 0.0f);
        mat.setRotateAxis(0.0f, 0.0f, 1.0f, PI*0.5f);
        v.mul(mat, v);
        std::cout << v;

        v.set(0.0f, 1.0f, 0.0f);
        mat.identity();
        mat.rotateZ(PI*0.5f);
        v.mul(v, mat);
        std::cout << v;

        v.set(0.0f, 1.0f, 0.0f);
        mat.setRotateAxis(0.0f, 0.0f, 1.0f, PI*0.5f);
        v.mul(v, mat);
        std::cout << v;
    }

    {
        Vector3 v;

        Matrix34 mat;

        v.set(1.0f, 0.0f, 0.0f);
        mat.identity();
        mat.rotateY(PI*0.5f);
        v.mul(mat, v);
        std::cout << v;

        v.set(1.0f, 0.0f, 0.0f);
        mat.setRotateAxis(0.0f, 1.0f, 0.0f, PI*0.5f);
        v.mul(mat, v);
        std::cout << v;

        v.set(1.0f, 0.0f, 0.0f);
        mat.identity();
        mat.rotateY(PI*0.5f);
        v.mul(v, mat);
        std::cout << v;

        v.set(1.0f, 0.0f, 0.0f);
        mat.setRotateAxis(0.0f, 1.0f, 0.0f, PI*0.5f);
        v.mul(v, mat);
        std::cout << v;
    }

    {
        Vector3 v;

        Matrix34 mat;

        v.set(0.0f, 0.0f, 1.0f);
        mat.identity();
        mat.rotateX(PI*0.5f);
        v.mul(mat, v);
        std::cout << v;

        v.set(0.0f, 0.0f, 1.0f);
        mat.setRotateAxis(1.0f, 0.0f, 0.0f, PI*0.5f);
        v.mul(mat, v);
        std::cout << v;

        v.set(0.0f, 0.0f, 1.0f);
        mat.identity();
        mat.rotateX(PI*0.5f);
        v.mul(v, mat);
        std::cout << v;

        v.set(0.0f, 0.0f, 1.0f);
        mat.setRotateAxis(1.0f, 0.0f, 0.0f, PI*0.5f);
        v.mul(v, mat);
        std::cout << v;
    }


    std::cout << "===== Matrix44 =====" << std::endl;
    {
        Vector4 v;

        Matrix44 mat;

        v.set(0.0f, 1.0f, 0.0f, 1.0f);
        mat.identity();
        mat.rotateZ(PI*0.5f);
        v.mul(mat, v);
        std::cout << v;

        v.set(0.0f, 1.0f, 0.0f, 1.0f);
        mat.setRotateAxis(0.0f, 0.0f, 1.0f, PI*0.5f);
        v.mul(mat, v);
        std::cout << v;

        v.set(0.0f, 1.0f, 0.0f, 1.0f);
        mat.identity();
        mat.rotateZ(PI*0.5f);
        v.mul(v, mat);
        std::cout << v;

        v.set(0.0f, 1.0f, 0.0f, 1.0f);
        mat.setRotateAxis(0.0f, 0.0f, 1.0f, PI*0.5f);
        v.mul(v, mat);
        std::cout << v;
    }

    {
        Vector4 v;

        Matrix44 mat;

        v.set(1.0f, 0.0f, 0.0f, 1.0f);
        mat.identity();
        mat.rotateY(PI*0.5f);
        v.mul(mat, v);
        std::cout << v;

        v.set(1.0f, 0.0f, 0.0f, 1.0f);
        mat.setRotateAxis(0.0f, 1.0f, 0.0f, PI*0.5f);
        v.mul(mat, v);
        std::cout << v;

        v.set(1.0f, 0.0f, 0.0f, 1.0f);
        mat.identity();
        mat.rotateY(PI*0.5f);
        v.mul(v, mat);
        std::cout << v;

        v.set(1.0f, 0.0f, 0.0f, 1.0f);
        mat.setRotateAxis(0.0f, 1.0f, 0.0f, PI*0.5f);
        v.mul(v, mat);
        std::cout << v;
    }

    {
        Vector4 v;

        Matrix44 mat;

        v.set(0.0f, 0.0f, 1.0f, 1.0f);
        mat.identity();
        mat.rotateX(PI*0.5f);
        v.mul(mat, v);
        std::cout << v;

        v.set(0.0f, 0.0f, 1.0f, 1.0f);
        mat.setRotateAxis(1.0f, 0.0f, 0.0f, PI*0.5f);
        v.mul(mat, v);
        std::cout << v;

        v.set(0.0f, 0.0f, 1.0f, 1.0f);
        mat.identity();
        mat.rotateX(PI*0.5f);
        v.mul(v, mat);
        std::cout << v;

        v.set(0.0f, 0.0f, 1.0f, 1.0f);
        mat.setRotateAxis(1.0f, 0.0f, 0.0f, PI*0.5f);
        v.mul(v, mat);
        std::cout << v;
    }


    std::cout << "===== Quaternion =====" << std::endl;
    {
        Quaternion rot;

        Vector3 v;

        Matrix34 mat;

        v.set(0.0f, 1.0f, 0.0f);
        rot.setRotateZ(PI*0.5f);
        rot.getMatrix(mat);
        v.mul(mat, v);
        std::cout << v;

        v.set(1.0f, 0.0f, 0.0f);
        rot.setRotateY(PI*0.5f);
        rot.getMatrix(mat);
        v.mul(mat, v);
        std::cout << v;

        v.set(0.0f, 0.0f, 1.0f);
        rot.setRotateX(PI*0.5f);
        rot.getMatrix(mat);
        v.mul(mat, v);
        std::cout << v;

        ///////////////////////////////
        v.set(0.0f, 1.0f, 0.0f);
        rot.setRotateAxis(0.0f, 0.0f, 1.0f, PI*0.5f);
        rot.getMatrix(mat);
        v.mul(mat, v);
        std::cout << v;

        v.set(1.0f, 0.0f, 0.0f);
        rot.setRotateAxis(0.0f, 1.0f, 0.0f, PI*0.5f);
        rot.getMatrix(mat);
        v.mul(mat, v);
        std::cout << v;

        v.set(0.0f, 0.0f, 1.0f);
        rot.setRotateAxis(1.0f, 0.0f, 0.0f, PI*0.5f);
        rot.getMatrix(mat);
        v.mul(mat, v);
        std::cout << v;

        ///////////////////////////////
        v.set(0.0f, 1.0f, 0.0f);
        rot.setRotateZ(PI*0.5f);
        v.rotate(rot);
        std::cout << v;

        v.set(1.0f, 0.0f, 0.0f);
        rot.setRotateY(PI*0.5f);
        v.rotate(rot);
        std::cout << v;

        v.set(0.0f, 0.0f, 1.0f);
        rot.setRotateX(PI*0.5f);;
        v.rotate(rot);
        std::cout << v;
    }

    std::cout << "===== Matrix34 =====" << std::endl;
    {
        Matrix34 mat;
        mat.identity();
        mat.rotateZ(0.2f*PI);
        mat.rotateY(-0.2f*PI);
        mat.translate(0.145f, 0.31f, 0.22f);

        Matrix34 m0 = mat;
        Matrix34 m1 = mat;

        m0 *= mat;
        m1.mul(m1, mat);

        std::cout << m0;
        std::cout << m1;

        std::cout << "== invert33 ==" << std::endl;
        m0 = mat;
        m1 = mat;

        m1.setTranslate(0.0f, 0.0f, 0.0f);
        m1.invert33();
        m0 *= m1;
        std::cout << m0;

        m0 = mat;
        m1 *= m0;
        std::cout << m1;

        std::cout << "== invert ==" << std::endl;
        m0 = mat;
        m1 = mat;

        m1.invert();
        m0 *= m1;
        std::cout << m0;

        m0 = mat;
        m1 *= m0;
        std::cout << m1;
    }

    std::cout << "===== Matrix44 =====" << std::endl;
    {
        Matrix44 mat;
        mat.identity();
        mat.rotateZ(0.2f*PI);
        mat.translate(0.1f, 0.3f, 0.2f);

        Matrix44 m0 = mat;
        Matrix44 m1 = mat;

        m0 *= mat;

        std::cout << m0;
    }
    return 0;
}
