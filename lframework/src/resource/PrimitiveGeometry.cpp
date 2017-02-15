/**
@file PrimitiveGeometry.cpp
@author t-sakai
@date 2017/02/12 create
*/
#include "resource/PrimitiveGeometry.h"
#include <lmath/geometry/Sphere.h>
#include <lgraphics/VertexBufferRef.h>
#include <lgraphics/IndexBufferRef.h>
#include <lgraphics/InputLayoutRef.h>
#include "resource/InputLayoutFactory.h"
#include "resource/ResourceModel.h"
#include "resource/ResourceTexture2D.h"
#include "resource/Resources.h"
#include "render/Model.h"

namespace lfw
{
    ResourceModel* PrimitiveGeometry::createPlane(f32 scale)
    {
        Model* model = LNEW Model();
        if(!create(*model, scale)){
            LDELETE(model);
            return NULL;
        }
        return LNEW ResourceModel(model, NULL);
    }

    bool PrimitiveGeometry::create(Model& model, f32 scale)
    {
        if(!model.create(1, 1, 1, 1, 1)){
            return false;
        }
        Node& node = model.getNode(0);
        node.clearTransform();
        node.world0_.identity();
        node.world1_.identity();
        node.numMeshes_ = 1;
        node.meshes_ = &model.getMesh(0);

        Material& material = model.getMaterial(0);
        material.setFlag(MaterialFlag_CastShadow);
        material.setFlag(MaterialFlag_ReceiveShadow);
        material.diffuse_ = lmath::Vector4::one();
        material.specular_ = lmath::Vector4::construct(1.0f, 1.0f, 1.0f, 0.5f);
        material.ambient_ = lmath::Vector4::construct(1.0f, 1.0f, 1.0f, 0.0f);
        material.shadow_ = lmath::Vector4::construct(0.5f, 0.5f, 0.5f, 0.0f);
        material.blendState_ = lgfx::BlendState::create(
            FALSE,
            FALSE,
            lgfx::Blend_SrcAlpha,
            lgfx::Blend_InvSrcAlpha,
            lgfx::BlendOp_Add,
            lgfx::Blend_SrcAlpha,
            lgfx::Blend_DestAlpha,
            lgfx::BlendOp_Add,
            lgfx::ColorWrite_All);

        material.textureIDs_[TextureType_Albedo] = 0;

        Resource::pointer& resource = Resources::getInstance().getEmptyTextureWhite();
        ResourceTexture2D* resTex2D = resource.get_reinterpret_cast<ResourceTexture2D>();

        Texture2D& albedo = model.getTexture(0);
        albedo.texture_ = resTex2D->get();
        albedo.srv_ = resTex2D->getShaderResourceView();
        albedo.sampler_ = resTex2D->getSampler();

        f32 half = scale*0.5f;
        Vertex vertices[4];
        vertices[0].position_[0] = -half; vertices[0].position_[1] =  0.0f; vertices[0].position_[2] = -half;
        vertices[0].normal_[0] = lcore::toBinary16Float(0.0f); vertices[0].normal_[1] = lcore::toBinary16Float(1.0f); vertices[0].normal_[2] = vertices[0].normal_[3] = vertices[0].normal_[0];
        vertices[0].texcoord_[0] = vertices[0].texcoord_[1] = lcore::toBinary16Float(0.0f);

        vertices[1].position_[0] =  half; vertices[1].position_[1] =  0.0f; vertices[1].position_[2] = -half;
        vertices[1].normal_[0] = vertices[0].normal_[0]; vertices[1].normal_[1] = vertices[0].normal_[1]; vertices[1].normal_[2] = vertices[0].normal_[2]; vertices[1].normal_[3] = vertices[0].normal_[3];
        vertices[1].texcoord_[0] = lcore::toBinary16Float(1.0f); vertices[1].texcoord_[1] = lcore::toBinary16Float(0.0f);

        vertices[2].position_[0] = -half; vertices[2].position_[1] =  0.0f; vertices[2].position_[2] =  half;
        vertices[2].normal_[0] = vertices[0].normal_[0]; vertices[2].normal_[1] = vertices[0].normal_[1]; vertices[2].normal_[2] = vertices[0].normal_[2]; vertices[2].normal_[3] = vertices[0].normal_[3];
        vertices[2].texcoord_[0] = lcore::toBinary16Float(0.0f); vertices[2].texcoord_[1] = lcore::toBinary16Float(1.0f);

        vertices[3].position_[0] =  half; vertices[3].position_[1] =  0.0f; vertices[3].position_[2] =  half;
        vertices[3].normal_[0] = vertices[0].normal_[0]; vertices[3].normal_[1] = vertices[0].normal_[1]; vertices[3].normal_[2] = vertices[0].normal_[2]; vertices[3].normal_[3] = vertices[0].normal_[3];
        vertices[3].texcoord_[0] = vertices[3].texcoord_[1] = lcore::toBinary16Float(1.0f);

        u16 indices[6] = {0,2,1, 1,2,3};
        lmath::Sphere sphere;
        sphere.setPosition(0.0f, 0.0f, 0.0f);
        sphere.setRadius(lmath::sqrt(2.0f)*scale);
        createGeometry(model, 4, vertices, 6, indices, sphere);
        return true;
    }

    void PrimitiveGeometry::createGeometry(
        Model& model,
        u32 numVertices,
        Vertex* vertices,
        u32 numIndices,
        u16* indices,
        const lmath::Sphere& sphere)
    {

        Geometry& geometry = model.getGeometry(0);
        lgfx::VertexBufferRef vb = lgfx::VertexBuffer::create(sizeof(Vertex)*numVertices, lgfx::Usage_Default, lgfx::CPUAccessFlag_None, lgfx::ResourceMisc_None, 0, vertices);
        lgfx::IndexBufferRef ib = lgfx::IndexBuffer::create(sizeof(u16)*numIndices, lgfx::Usage_Default, lgfx::CPUAccessFlag_None, lgfx::ResourceMisc_None, 0, indices);

        lgfx::InputLayoutRef inputLayout = Resources::getInstance().getInputLayoutFactory().get(InputLayout_PNU);
        geometry.create(VElem_Position|VElem_Normal|VElem_Texcoord, sizeof(Vertex), numVertices, NULL, numIndices, NULL, inputLayout, vb, ib);

        model.getMesh(0).create(
            lgfx::Primitive_TriangleList,
            0,
            numIndices,
            &model.getGeometry(0),
            &model.getMaterial(0),
            sphere);
        model.setSphere(sphere);
    }
}
