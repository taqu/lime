#ifndef INC_LRENDER_LIGHT_H__
#define INC_LRENDER_LIGHT_H__
/**
@file Light.h
@author t-sakai
@date 2010/01/05 create
*/
#include "../lrendercore.h"

namespace lrender
{
    class Light
    {
    public:
        static Light* create(LightType type);

        virtual ~Light();

        virtual LightType getType() const =0;
        virtual void calc(f32& intensity, Vector3& color, Vector3& direction, const Vector3& position) =0;

        f32 getIntensity() const;
        void setIntensity(f32 intensity);

        const Vector3& getColor() const;
        void setColor(const Vector3& color);

        const Vector3& getDirection() const;
        void setDirection(const Vector3& direction);
    protected:
        Light();

        f32 intensity_;
        Vector3 color_;
        Vector3 direction_;
    };
}

#endif //INC_LRENDER_LIGHT_H__
