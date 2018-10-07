/**
@file PrimitiveGeometry.cpp
@author t-sakai
@date 2017/02/12 create
*/
#include "resource/PrimitiveGeometry.h"
#include "System.h"
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

    void PrimitiveGeometry::createLightSphere(Geometry& geometry, s32 resolution)
    {
        u16 hres = 4;
        u16 vres = 2;

        for(s32 i=0; i<resolution; ++i){
            hres <<= 1;
            vres = vres + (1<<i);
        }
        vres = (vres<<1) - 1;
        s32 half_vres = vres>>1;
        f32 inv_half_vres = 1.0f/half_vres;
        f32 inv_hres = 1.0f/hres;

        f32 radius = 1.0f;
        {
            f32 theta = 0.5f*PI_2*inv_half_vres;
            while(1.0e-6f<theta){
                radius *= 1.0f/(lmath::cosf(theta));
                theta *= 0.5f;
            }
            radius *= 1.01f;
        }

        s32 numVertices = hres*(vres-2) + 2;
        s32 numIndices = hres*6 + (vres-3)*(6*hres);

        //vertices
        //-----------------
        VertexP* vertices = LNEW VertexP[numVertices];
        f32 x = 0.0f;
        f32 y = radius;
        f32 z = 0.0f;
        vertices[0].position_[0] = x;
        vertices[0].position_[1] = y;
        vertices[0].position_[2] = z;


        vertices[numVertices-1].position_[0] = x;
        vertices[numVertices-1].position_[1] = -y;
        vertices[numVertices-1].position_[2] = z;

        s32 vindex = 1;
        for(s32 v=half_vres-1; (1-half_vres)<=v; --v){
            f32 theta = PI_2*v*inv_half_vres;
            y = radius*lmath::sinf(theta);

            f32 r = radius*lmath::cosf(theta);
            for(s32 h=0; h<hres; ++h){
                f32 phi = PI2*h*inv_hres;
                x = r*lmath::cosf(phi);
                z = r*lmath::sinf(phi);
                vertices[vindex].position_[0] = x;
                vertices[vindex].position_[1] = y;
                vertices[vindex].position_[2] = z;
                ++vindex;
            }
        }

        //indices
        //-----------------
        u16* indices = LNEW u16[numIndices];
        s32 index=0;
        u16 offset = 1;
        for(u16 i=1; i<hres; ++i){
            indices[index++] = 0;
            indices[index++] = offset+i;
            indices[index++] = offset+i-1;
        }
        indices[index++] = 0;
        indices[index++] = 1;
        indices[index++] = hres;


        for(s32 i=0; i<(vres-3); ++i){
            u16 top = offset;
            offset += hres;
            u16 bottom = offset;
            for(u16 j=1; j<hres; ++j){
                indices[index+0] = top;
                indices[index+1] = bottom+1;
                indices[index+2] = bottom;

                indices[index+3] = indices[index+0];
                indices[index+4] = top+1;
                indices[index+5] = indices[index+1];

                index += 6;
                ++top;
                ++bottom;
            }
            indices[index+0] = top;
            indices[index+1] = offset;
            indices[index+2] = bottom;

            indices[index+3] = indices[index+0];
            indices[index+4] = offset-hres;
            indices[index+5] = indices[index+1];
            index += 6;
        }

        for(u16 i=1; i<hres; ++i){
            indices[index++] = offset+i-1;
            indices[index++] = offset+i;
            indices[index++] = offset+hres;
        }
        indices[index++] = offset+hres-1;
        indices[index++] = offset;
        indices[index++] = offset+hres;

        lgfx::InputLayoutRef inputLayout = System::getResources().getInputLayoutFactory().get(InputLayout_P);
        lgfx::VertexBufferRef vb = lgfx::VertexBuffer::create(sizeof(VertexP)*numVertices, lgfx::Usage_Default, lgfx::CPUAccessFlag_None, lgfx::ResourceMisc_None, 0, vertices);
        lgfx::IndexBufferRef ib = lgfx::IndexBuffer::create(sizeof(u16)*numIndices, lgfx::Usage_Default, lgfx::CPUAccessFlag_None, lgfx::ResourceMisc_None, 0, indices);

        geometry.create(VElem_Position, sizeof(VertexP), numVertices, NULL, numIndices, NULL, inputLayout, vb, ib);

        LDELETE_ARRAY(indices);
        LDELETE_ARRAY(vertices);
    }

    void PrimitiveGeometry::createLightCapsule(Geometry& /*geometry*/, s32 /*resolution*/)
    {
    }

    void PrimitiveGeometry::createLightCone(Geometry& /*geometry*/, s32 /*resolution*/)
    {
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

        Resource::pointer& resource = System::getResources().getEmptyTextureWhite();
        ResourceTexture2D* resTex2D = resource.get_reinterpret_cast<ResourceTexture2D>();

        Texture2D& albedo = model.getTexture(0);
        albedo.texture_ = resTex2D->get();
        albedo.srv_ = resTex2D->getShaderResourceView();
        albedo.sampler_ = resTex2D->getSampler();

        f32 half = scale*0.5f;
        Vertex vertices[4];
        vertices[0].position_[0] = -half; vertices[0].position_[1] =  0.0f; vertices[0].position_[2] = -half;
        vertices[0].normal_[0] = lcore::toFloat16(0.0f); vertices[0].normal_[1] = lcore::toFloat16(1.0f); vertices[0].normal_[2] = vertices[0].normal_[3] = vertices[0].normal_[0];
        vertices[0].texcoord_[0] = vertices[0].texcoord_[1] = lcore::toFloat16(0.0f);

        vertices[1].position_[0] =  half; vertices[1].position_[1] =  0.0f; vertices[1].position_[2] = -half;
        vertices[1].normal_[0] = vertices[0].normal_[0]; vertices[1].normal_[1] = vertices[0].normal_[1]; vertices[1].normal_[2] = vertices[0].normal_[2]; vertices[1].normal_[3] = vertices[0].normal_[3];
        vertices[1].texcoord_[0] = lcore::toFloat16(1.0f); vertices[1].texcoord_[1] = lcore::toFloat16(0.0f);

        vertices[2].position_[0] = -half; vertices[2].position_[1] =  0.0f; vertices[2].position_[2] =  half;
        vertices[2].normal_[0] = vertices[0].normal_[0]; vertices[2].normal_[1] = vertices[0].normal_[1]; vertices[2].normal_[2] = vertices[0].normal_[2]; vertices[2].normal_[3] = vertices[0].normal_[3];
        vertices[2].texcoord_[0] = lcore::toFloat16(0.0f); vertices[2].texcoord_[1] = lcore::toFloat16(1.0f);

        vertices[3].position_[0] =  half; vertices[3].position_[1] =  0.0f; vertices[3].position_[2] =  half;
        vertices[3].normal_[0] = vertices[0].normal_[0]; vertices[3].normal_[1] = vertices[0].normal_[1]; vertices[3].normal_[2] = vertices[0].normal_[2]; vertices[3].normal_[3] = vertices[0].normal_[3];
        vertices[3].texcoord_[0] = vertices[3].texcoord_[1] = lcore::toFloat16(1.0f);

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

        lgfx::InputLayoutRef inputLayout = System::getResources().getInputLayoutFactory().get(InputLayout_PNU);
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
