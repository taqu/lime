/**
@file Viewer.cpp
@author t-sakai
@date 2017/01/18 create
*/
#include "Viewer.h"
#include <Windows.h>
#include <Commdlg.h>
#include <lframework/gui/gui.h>

#include <lframework/System.h>
#include <lframework/render/Camera.h>
#include <lframework/render/Light.h>
#include <lframework/resource/load_texture.h>
#include <lframework/Application.h>
#include <lframework/input/Keyboard.h>
#include <lframework/input/Mouse.h>
#include <lframework/ecs/ECSManager.h>
#include <lframework/ecs/ComponentGeometric.h>
#include <lframework/ecs/ComponentLight.h>
#include <lframework/ecs/ComponentCamera.h>

#include "converter.h"
#include "ObjConverter.h"

namespace viewer
{
namespace
{
    static const lfw::Char* BlendNames[] =
    {
        "BLEND_ZERO",
        "BLEND_ONE",
        "BLEND_SRC_COLOR",
        "BLEND_INV_SRC_COLOR",
        "BLEND_SRC_ALPHA",
        "BLEND_INV_SRC_ALPHA",
        "BLEND_DEST_ALPHA",
        "BLEND_INV_DEST_ALPHA",
        "BLEND_DEST_COLOR",
        "BLEND_INV_DEST_COLOR",
        "BLEND_SRC_ALPHA_SAT",
        "BLEND_BLEND_FACTOR",
        "BLEND_INV_BLEND_FACTOR",
        "BLEND_SRC1_COLOR",
        "BLEND_INV_SRC1_COLOR",
        "BLEND_SRC1_ALPHA",
        "BLEND_INV_SRC1_ALPHA",
    };

    lfw::s32 toBlendIndex(D3D11_BLEND blend)
    {
        blend = lcore::clamp(blend, D3D11_BLEND_ZERO, D3D11_BLEND_INV_SRC1_ALPHA);
        return static_cast<lfw::s32>(blend-1);
    }

    lfw::u8 fromBlendIndex(lfw::s32 index)
    {
        ++index;
        index = lcore::clamp(index, (lfw::s32)D3D11_BLEND_ZERO, (lfw::s32)D3D11_BLEND_INV_SRC1_ALPHA);
        return static_cast<lfw::u8>(index);
    }

    static const lfw::Char* BlendOpNames[] =
    {
        "BLEND_OP_ADD",
        "BLEND_OP_SUBTRACT",
        "BLEND_OP_REV_SUBTRACT",
        "BLEND_OP_MIN",
        "BLEND_OP_MAX",
    };

    lfw::s32 toBlendOpIndex(D3D11_BLEND_OP blendOp)
    {
        blendOp = lcore::clamp(blendOp, D3D11_BLEND_OP_ADD, D3D11_BLEND_OP_MAX);
        return static_cast<lfw::s32>(blendOp-1);
    }

    lfw::u8 fromBlendOpIndex(lfw::s32 index)
    {
        ++index;
        index = lcore::clamp(index, (lfw::s32)D3D11_BLEND_OP_ADD, (lfw::s32)D3D11_BLEND_OP_MAX);
        return static_cast<lfw::u8>(index);
    }
}

    Viewer::Viewer()
        :model_(NULL)
        ,skeleton_(NULL)
        ,textures_(NULL)
        ,menuOpen_(true)
        ,loadModel_(Format_Unknown)
        ,rotationX_(0.0f)
        ,rotationY_(0.0f)
        ,zoom_(0.0f)
        ,zoomMin_(0.0f)
        ,zoomMax_(0.0f)
        ,zoomRate_(0.0f)

        ,scale_(1.0f)
        ,convertToDDS_(true)
    {
        filepath_[0] = lcore::CharNull;
    }

    Viewer::~Viewer()
    {
        LDELETE_ARRAY(textures_);
    }

    lfw::u32 Viewer::getType() const
    {
        return ECSType_Viewer;
    }

    void Viewer::onCreate()
    {
        lfw::Application& application = lfw::System::getApplication();
        lfw::Entity mainLight = lfw::System::getECSManager().requestCreateGeometric("MainLight");
        light_ = mainLight.addComponent<lfw::ComponentLight>();

        mainLight.getGeometric()->getRotation().lookAt(lmath::Vector4::construct(normalize(lmath::Vector4::construct(0.2f, -1.0f, 0.0f, 0.0f))));

        lfw::Entity mainCamera = lfw::System::getECSManager().requestCreateGeometric("MainCamera");
        camera_ = mainCamera.addComponent<lfw::ComponentCamera>();
        camera_->initialize(60.0f, 0.01f, 100.0f,1024, 768, false);
        mainCamera.getGeometric()->setPosition(lmath::Vector4::construct(0.5f, 2.5f, -2.0f, 0.0f));
    }

    void Viewer::update()
    {
        if(NULL != model_){
            linput::Input& input = lfw::System::getInput();
            //const linput::Keyboard* keyboard = input.getKeyboard();
            const linput::Mouse* mouse = input.getMouse();

            if(mouse->isOn(linput::MouseButton_1)){
                rotationY_ += mouse->getDuration(linput::MouseAxis_X)*0.05f;
                rotationY_ = lcore::clampRotate0(rotationY_, PI2);
                rotationX_ -= mouse->getDuration(linput::MouseAxis_Y)*0.01f;
                rotationX_ = lcore::clamp(rotationX_, -89.0f*DEG_TO_RAD, 89.0f*DEG_TO_RAD);
                lmath::Quaternion rotationY = lmath::Quaternion::rotateY(rotationY_);
                lmath::Quaternion rotationX = lmath::Quaternion::rotateX(rotationX_);
                rotationX *= rotationY;
                lmath::Vector4 axis = lmath::Vector4::construct(0.0f, 0.0f, zoom_);
                axis = rotate(axis, rotationX);

                lmath::Vector4 center = lmath::Vector4::construct(model_->getSphere().position());
                lmath::Vector4 cameraPos = lmath::Vector4::construct(axis + center);
                camera_->getEntity().getGeometric()->setPosition(cameraPos);
                lmath::Quaternion& cameraRotation = camera_->getEntity().getGeometric()->getRotation();
                cameraRotation.lookAt(cameraPos, center);
            }

            if(0!=mouse->getDuration(linput::MouseAxis_Z)){
                zoom_ -= 0.1f*mouse->getDuration(linput::MouseAxis_Z)*zoomRate_;
                zoom_ = lcore::clamp(zoom_, zoomMin_, zoomMax_);
                lmath::Quaternion cameraRotation = camera_->getEntity().getGeometric()->getRotation();
                lmath::Vector4 axis = rotate(cameraRotation, lmath::Vector4::construct(0.0f, 0.0f, zoom_));
                lmath::Vector4 center = lmath::Vector4::construct(model_->getSphere().position());
                lmath::Vector4 cameraPos = lmath::Vector4::construct(-axis + center);

                camera_->getEntity().getGeometric()->setPosition(cameraPos);
            }
        }

        if(!ImGui::Begin("ImGui", &menuOpen_, 0)){
            ImGui::End();
            return;
        }

        if(ImGui::CollapsingHeader("Load Options", ImGuiTreeNodeFlags_DefaultOpen)){
            ImGui::Checkbox("Convert to DDS", &convertToDDS_);
            ImGui::DragFloat("Scale (OBJ only)", &scale_);
        }//ImGui::CollapsingHeader("Options"

        if(ImGui::Button("Open")){
            OPENFILENAME ofn;

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = lfw::System::getWindow().getHandle().hWnd_;
            ofn.lpstrFile = filepath_;
            ofn.nMaxFile = sizeof(filepath_);
            ofn.lpstrFilter = "Model\0*.lm\0All\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if(TRUE == GetOpenFileName(&ofn)){
                lcore::replace(filepath_, '/', '\\');
                loadModel_ = getFormat(filepath_);
            }
        }//if(ImGui::Button("Open"))
        ImGui::SameLine();

        if(Format_Unknown != loadModel_){
            loadModel(loadModel_);
        }

        if(ImGui::Button("Save")){
            if(NULL != model_){
                OPENFILENAME ofn;

                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = lfw::System::getWindow().getHandle().hWnd_;
                ofn.lpstrFile = filepath_;
                ofn.lpstrFile[0] = '\0';
                ofn.nMaxFile = sizeof(filepath_);
                ofn.lpstrFilter = "Model\0*.lm\0All\0*.*\0";
                ofn.nFilterIndex = 1;
                ofn.lpstrFileTitle = NULL;
                ofn.nMaxFileTitle = 0;
                ofn.lpstrInitialDir = NULL;
                ofn.lpstrDefExt = "lm";
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_NONETWORKBUTTON;

                if(TRUE == GetSaveFileName(&ofn)){
                    lcore::replace(filepath_, '/', '\\');
                    modelLoader_.setConvertRefractiveIndex(false);
                    modelLoader_.setReserveGeometryData(true);

                    modelLoader_.save(filepath_, *model_.get(), textures_, skeleton_.get());
                }
            }
        }//if(ImGui::Button("Save"))

        if(ImGui::CollapsingHeader("Materials", ImGuiTreeNodeFlags_DefaultOpen)){
            if(NULL != model_){
                static lcore::Char label[32];
                for(lcore::s32 i=0; i<model_->getNumMaterials(); ++i){
                    lfw::Material& material = model_->getMaterial(i);
                    sprintf_s(label, "%d", i);
                    if(ImGui::TreeNode(label)){
                        ImGui::ColorEdit4("Diffuse", &material.diffuse_.x_);
                        ImGui::ColorEdit3("Specular", &material.specular_.x_);
                        ImGui::SliderFloat("Roughness", &material.specular_.w_, 0.0f, 1.0f);
                        ImGui::ColorEdit3("Ambient", &material.ambient_.x_);
                        ImGui::SliderFloat("Refractive Index", &material.ambient_.w_, 1.0f, 5.0f);
                        ImGui::ColorEdit3("Shadow", &material.shadow_.x_);
                        ImGui::SliderFloat("Metalic", &material.shadow_.w_, 0.0f, 1.0f);

                        lgfx::BlendStateDesc blendStateDesc;
                        if(material.blendState_.getDesc(blendStateDesc)){
                            bool changed = false;
                            bool blendEnable = 0 != blendStateDesc.RenderTarget[0].BlendEnable;
                            lfw::s32 srcBlend = toBlendIndex(blendStateDesc.RenderTarget[0].SrcBlend);
                            lfw::s32 dstBlend = toBlendIndex(blendStateDesc.RenderTarget[0].DestBlend);
                            lfw::s32 blendOp = toBlendOpIndex(blendStateDesc.RenderTarget[0].BlendOp);

                            changed |= ImGui::Checkbox("Blend", &blendEnable);
                            changed |= ImGui::Combo("SrcBlend", &srcBlend, BlendNames, 17);
                            changed |= ImGui::Combo("DstBlend", &dstBlend, BlendNames, 17);
                            changed |= ImGui::Combo("BlendOp", &blendOp, BlendOpNames, 5);
                            if(changed){
                                blendStateDesc.RenderTarget[0].SrcBlend = (D3D11_BLEND)fromBlendIndex(srcBlend);
                                blendStateDesc.RenderTarget[0].DestBlend = (D3D11_BLEND)fromBlendIndex(dstBlend);
                                blendStateDesc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)fromBlendOpIndex(blendOp);
                                material.blendState_ = lgfx::BlendState::create(
                                    FALSE,
                                    blendEnable?TRUE:FALSE,
                                    static_cast<lgfx::BlendType>(blendStateDesc.RenderTarget[0].SrcBlend),
                                    static_cast<lgfx::BlendType>(blendStateDesc.RenderTarget[0].DestBlend),
                                    static_cast<lgfx::BlendOp>(blendStateDesc.RenderTarget[0].BlendOp),
                                    lgfx::Blend_SrcAlpha,
                                    lgfx::Blend_DestAlpha,
                                    lgfx::BlendOp_Add,
                                    lgfx::ColorWrite_All);
                            }
                        }

                        bool flagChanged = false;
                        bool flagCastShadow = material.checkFlag(lfw::MaterialFlag_CastShadow);
                        bool flagReceiveShadow = material.checkFlag(lfw::MaterialFlag_ReceiveShadow);
                        flagChanged |= ImGui::Checkbox("Cast Shadow", &flagCastShadow);
                        ImGui::SameLine();
                        flagChanged |= ImGui::Checkbox("Receive Shadow", &flagReceiveShadow);
                        if(flagChanged){
                            if(flagCastShadow){
                                material.setFlag(lfw::MaterialFlag_CastShadow);
                            } else{
                                material.unsetFlag(lfw::MaterialFlag_CastShadow);
                            }
                            if(flagReceiveShadow){
                                material.setFlag(lfw::MaterialFlag_ReceiveShadow);
                            } else{
                                material.unsetFlag(lfw::MaterialFlag_ReceiveShadow);
                            }
                        }
                        ImGui::TreePop();
                    }
                }
            }
        }
        ImGui::End();
    }

    void Viewer::setModelPath(const lfw::Char* filepath)
    {
        LASSERT(NULL != filepath);
        lcore::s32 len = lcore::strlen_s32(filepath);
        lcore::memcpy(filepath_, filepath, len);
        filepath_[len] = '\0';
        loadModel_ = getFormat(filepath_);
    }

    Format Viewer::getFormat(const lfw::Char* filepath)
    {
        lcore::s32 length = lcore::strlen_s32(filepath);
        const lcore::Char* ext = lcore::getExtension(length, filepath);
        if(0 == lcore::strncmp(ext, "lm", 2)){
            return Format_LM;
        }
        if(0 == lcore::strncmp(ext, "obj", 3)){
            return Format_OBJ;
        }
        if(0 == lcore::strncmp(ext, "fbx", 3)){
            return Format_FBX;
        }
        return Format_Unknown;
    }

    void Viewer::resetCamera()
    {
        rotationX_ = 0.0f;
        rotationY_ = 0.0f;
    }

    void Viewer::loadModel(Format format)
    {
        loadModel_ = Format_Unknown;
        lfw::Model* model = NULL;
        lfw::Skeleton* skeleton = NULL;
        switch(format)
        {
        case Format_LM:
        {
            modelLoader_.setConvertRefractiveIndex(false);
            modelLoader_.setReserveGeometryData(true);
            modelLoader_.loadModel(model, skeleton, 0, filepath_);
            if(NULL != model){
                LDELETE_ARRAY(textures_);
                textures_ = modelLoader_.loadTextures(filepath_);
            }
        }
            break;

        case Format_OBJ:
        {
            lfw::LoaderObj loaderObj;
        }
            break;
        }

        if(NULL != model){
            model_ = model;
            this->setMesh(model_);

            const lmath::Sphere& sphere = model_->getSphere();
            lmath::Vector4 pos = lmath::Vector4::construct(0.0f, 0.0f, sphere.radius()*3.0f, 0.0f);
            camera_->lookAt(pos, lmath::Vector4::construct(sphere.position()));
            zoom_ = 3.0f*sphere.radius();
            zoomMin_ = sphere.radius()*1.5f;
            zoomMax_ = sphere.radius()*6.0f;
            zoomRate_ = 0.125f * sphere.radius();
            resetCamera();
        }

    }
}
