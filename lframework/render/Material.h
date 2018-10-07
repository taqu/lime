#ifndef INC_LFRAMEWORK_MATERIAL_H_
#define INC_LFRAMEWORK_MATERIAL_H_
/**
@file Material.h
@author t-sakai
@date 2016/11/23 create
*/
#include "../lframework.h"
#include "../resource/load.h"
#include <lmath/Vector4.h>
#include <lgraphics/BlendStateRef.h>

namespace lfw
{
    class Material
    {
    public:
        Material();
        ~Material();

        inline bool checkFlag(MaterialFlag flag) const;
        inline void setFlag(MaterialFlag flag);
        inline void unsetFlag(MaterialFlag flag);
        inline bool isCastShadow() const;
        inline bool isRecieveShadow() const;
        inline bool isTransparent() const;

        inline bool hasTexture(s32 type) const;

        /// ÉXÉèÉbÉv
        void swap(Material& rhs);

        lmath::Vector4 diffuse_; //rgba
        lmath::Vector4 specular_; //rgb roughness
        lmath::Vector4 ambient_; //rgb ambient refractive index of fresnel
        lmath::Vector4 shadow_; //rgb metalic

        u32 flags_;
        lgfx::BlendStateRef blendState_;
        s8 textureIDs_[TextureType_Num];
    private:
        Material(const Material&);
        Material& operator=(const Material&);
    };

    inline bool Material::checkFlag(MaterialFlag flag) const
    {
        return 0 != (flags_ & flag);
    }

    inline void Material::setFlag(MaterialFlag flag)
    {
        flags_ |= flag;
    }

    inline void Material::unsetFlag(MaterialFlag flag)
    {
        flags_ &= ~flag;
    }

    inline bool Material::isCastShadow() const
    {
        return 0 != (flags_ & MaterialFlag_CastShadow);
    }

    inline bool Material::isRecieveShadow() const
    {
        return 0 != (flags_ & MaterialFlag_ReceiveShadow);
    }

    inline bool Material::isTransparent() const
    {
        return 0 != (flags_&MaterialFlag_Transparent);
    }

    inline bool Material::hasTexture(s32 type) const
    {
        return 0<=textureIDs_[type];
    }
}
#endif //INC_LFRAMEWORK_MATERIAL_H_
