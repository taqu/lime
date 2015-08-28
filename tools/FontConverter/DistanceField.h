#ifndef INC_DISTANCEFIELD_H__
#define INC_DISTANCEFIELD_H__
/**
@file DistanceField.h
@author t-sakai
@date 2015/08/06 create
*/

namespace df
{
    void rgb2gray(unsigned char* dst, int width, int height, int numChannels, const unsigned char* src);
    void gaussblur(float* dst, int width, int height, float sigma, const float* src);

    float signedDistance(
        int cx,
        int cy,
        float clamp,
        int width,
        int height,
        int channel,
        int numChannels,
        const unsigned char* data);
}
#endif //INC_DISTANCEFIELD_H__
