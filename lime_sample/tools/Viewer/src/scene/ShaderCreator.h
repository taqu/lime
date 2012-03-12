#ifndef INC_LSCENE_SHADERCREATOR_H__
#define INC_LSCENE_SHADERCREATOR_H__
/**
@file ShaderCreator.h
@author t-sakai
@date 2010/03/31 create
*/
namespace lscene
{
    class ShaderVSBase;
    class ShaderPSBase;
    class Material;
    class Geometry;
    class ShaderManager;

    class ShaderCreator
    {
    public:
        ShaderCreator()
        {}

        virtual ~ShaderCreator()
        {}

        virtual void create(
            Geometry& geometry,
            Material& material,
            ShaderManager& manager,
            ShaderVSBase*& retVS,
            ShaderPSBase*& retPS) const;
    };
}

#endif //INC_LSCENE_SHADERCREATOR_H__
