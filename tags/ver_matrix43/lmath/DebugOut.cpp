/**
@file DebugOut.cpp
@author t-sakai
@date 2010/01/13
*/
#include <iostream>
#include "DebugOut.h"

#include "Vector3.h"

namespace lmath
{
    std::ostream& operator<<(std::ostream& os, const Vector3& v)
    {
        os << v._x << ", " << v._y << ", " << v._z;
        return os;
    }
}