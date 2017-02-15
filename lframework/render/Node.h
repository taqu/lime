#ifndef INC_LFRAMEWORK_NODE_H__
#define INC_LFRAMEWORK_NODE_H__
/**
@file Node.h
@author t-sakai
@date 2016/11/23 create
*/
#include "../lframework.h"
#include <lmath/Vector3.h>
#include <lmath/Vector4.h>
#include <lmath/Matrix44.h>
#include <lmath/Quaternion.h>

namespace lfw
{
    class Mesh;

    //--------------------------------------------------
    class Node
    {
    public:
        Node();
        ~Node();

        void clearTransform();

        /// スワップ
        void swap(Node& rhs);

        /**
        @note meshes_もコピーするので後で正しくセットすること
        */
        void copyTo(Node& dst);

        void calcEuler(const lmath::Vector3& angle);
        void calcEuler(f32 x, f32 y, f32 z);

        void calcLocalMatrix(lmath::Matrix44& local);

        lmath::Vector4 translation_;
        lmath::Quaternion rotation_;
        lmath::Vector3 scale_;
        lmath::Matrix44 world0_;
        lmath::Matrix44 world1_;

        u8 type_;
        u8 rotationOrder_;
        u8 meshStartIndex_;
        u8 numMeshes_;

        u8 index_;
        u8 parentIndex_;
        u8 childStartIndex_;
        u8 numChildren_;

        Mesh* meshes_;

    private:
        Node(const Node&);
        Node& operator=(const Node&);
    };

    inline void Node::calcEuler(const lmath::Vector3& angle)
    {
        calcEuler(angle.x_, angle.y_, angle.z_);
    }
}
#endif //INC_LFRAMEWORK_NODE_H__
