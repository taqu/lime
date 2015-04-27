#ifndef INC_LSCENE_LIGHTENVIRONMENT_H__
#define INC_LSCENE_LIGHTENVIRONMENT_H__
/**
@file LightEnvironment.h
@author t-sakai
@date 2010/11/09 create

*/
#include "DirectionalLight.h"
//#include <lgraphics/api/TextureRef.h>

namespace lscene
{
    class LightEnvironment
    {
    public:
        inline const DirectionalLight& getDirectionalLight() const;
        inline DirectionalLight& getDirectionalLight();

        //inline lgraphics::Texture2DRef& getIrradianceMap();
        //inline void setIrradianceMap(lgraphics::Texture2DRef& irradianceMap);

        //inline lgraphics::Texture2DRef& getSpecularMap();
        //inline void setSpecularMap(lgraphics::Texture2DRef& specularMap);
    private:
        DirectionalLight directional_;
        //lgraphics::Texture2DRef irradianceMap_;
        //lgraphics::Texture2DRef specularMap_;
    };

    inline const DirectionalLight& LightEnvironment::getDirectionalLight() const
    {
        return directional_;
    }

    inline DirectionalLight& LightEnvironment::getDirectionalLight()
    {
        return directional_;
    }

    //inline lgraphics::Texture2DRef& LightEnvironment::getIrradianceMap()
    //{
    //    return irradianceMap_;
    //}

    //inline void LightEnvironment::setIrradianceMap(lgraphics::Texture2DRef& irradianceMap)
    //{
    //    irradianceMap_ = irradianceMap;
    //}

    //inline lgraphics::Texture2DRef& LightEnvironment::getSpecularMap()
    //{
    //    return specularMap_;
    //}

    //inline void LightEnvironment::setSpecularMap(lgraphics::Texture2DRef& specularMap)
    //{
    //    specularMap_ = specularMap;
    //}
}

#endif //INC_LSCENE_LIGHTENVIRONMENT_H__
