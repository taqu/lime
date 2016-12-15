/**
@file KDTree.cpp
@author t-sakai
@date 2016/5/29 create
*/
#include "accelerator/KDTree.h"

namespace lrender
{
    void Photon::estimateIrradiance(
        Vector3& irradiance,
        const Vector3& position,
        const Vector3& normal,
        s32 num,
        s32 minNum,
        const f32* sqrDistances,
        const Photon** photons)
    {
        irradiance.zero();
        if(num<minNum){
            return;
        }

        Vector3 dir;
        for(s32 i=0; i<num; ++i){
            f32 theta = lmath::dequantizeRadian16(photons[i]->theta_);
            f32 phi = lmath::dequantizeRadian16(photons[i]->phi_);
            lmath::sphericalToCartesian(dir, theta, phi);
            if(dot(normal, dir)<0.0f){
                irradiance += photons[i]->power_;
            }
        }
        f32 inv = (1.0f/PI)/(sqrDistances[num-1]);
        irradiance *= inv;
    }
}
