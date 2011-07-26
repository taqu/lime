/**
@file RigidBodyData.cpp
@author t-sakai
@date 2011/07/08
*/
#include "RigidBodyData.h"

namespace pmd
{
    lcore::istream& operator>>(lcore::istream& is, RigidBody& rhs)
    {
        lcore::io::read(is, rhs.name_, NameSize);
        lcore::io::read(is, rhs.boneIndex_);
        lcore::io::read(is, rhs.collideGroup_);
        lcore::io::read(is, rhs.collideMask_);
        lcore::io::read(is, rhs.shapeType_);

        lcore::io::read(is, &(rhs.size_), sizeof(lmath::Vector3));
        lcore::io::read(is, &(rhs.position_), sizeof(lmath::Vector3));
        lcore::io::read(is, &(rhs.rotation_), sizeof(lmath::Vector3));

        lcore::io::read(is, rhs.mass_);
        lcore::io::read(is, rhs.linearDamping_);
        lcore::io::read(is, rhs.angularDamping_);
        lcore::io::read(is, rhs.restitution_);
        lcore::io::read(is, rhs.friction_);
        lcore::io::read(is, rhs.type_);

        return is;
    }


    lcore::istream& operator>>(lcore::istream& is, Constraint& rhs)
    {
        lcore::io::read(is, rhs.name_, NameSize);
        lcore::io::read(is, rhs.rigidBodyIndex_[0]);
        lcore::io::read(is, rhs.rigidBodyIndex_[1]);

        lcore::io::read(is, &(rhs.position_), sizeof(lmath::Vector3));
        lcore::io::read(is, &(rhs.rotation_), sizeof(lmath::Vector3));

        lcore::io::read(is, &(rhs.limitLowerTrans_), sizeof(lmath::Vector3));
        lcore::io::read(is, &(rhs.limitUpperTrans_), sizeof(lmath::Vector3));

        lcore::io::read(is, &(rhs.limitLowerRot_), sizeof(lmath::Vector3));
        lcore::io::read(is, &(rhs.limitUpperRot_), sizeof(lmath::Vector3));

        lcore::io::read(is, rhs.stiffness_, sizeof(f32)*6);

        return is;
    }
}
