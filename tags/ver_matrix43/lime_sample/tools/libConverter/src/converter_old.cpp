#include "stdafx.h"
#include <fstream>
#include <windows.h>
#include <lcore/LeakCheck.h>
#include <lmath/lmath.h>

#include <lgraphics/lgraphics.h>
#include <lgraphics/api/InitParam.h>
#include <lgraphics/Window.h>

#include "loader/LoaderPmd.h"
#include "loader/LoaderVmd.h"
#include <lgraphics/scene/Scene.h>
#include <lgraphics/scene/AnimObject.h>
#include <lgraphics/scene/ShaderManager.h>
#include <lgraphics/scene/ShaderCreator.h>
#include "CustomShader.h"

#include <lgraphics/anim/AnimDatabase.h>
#include <lgraphics/anim/AnimControler.h>

#include <lgraphics/io/IOObject.h>
#include <lgraphics/io/IOAnimDatabase.h>

#include "loader/Pmd.h"
#include "IK.h"
#include "IOIK.h"

using namespace lcore;
using namespace lgraphics;

namespace
{
    class CustomShaderCreator : public ShaderCreator
    {
    public:
        CustomShaderCreator()
        {
            VertexShaderRef vs = Shader::createVertexShaderFromFile("shader/Skinning.vs");
            PixelShaderRef ps = Shader::createPixelShaderFromFile("shader/ModifiedPhong.ps");

            ShaderManager* manager = ShaderManager::getInstance();
            test::SimpleShaderVS *shaderVS = LIME_NEW test::SimpleShaderVS(vs);
            shaderVS->initialize();

            test::SimpleShaderPS *shaderPS = LIME_NEW test::SimpleShaderPS(ps);
            shaderPS->initialize();

            manager->addVS("simpleVS", shaderVS);
            manager->addPS("simplePS", shaderPS);
        }

        virtual ShaderVSBase* createVS(Geometry& geometry, const ShaderManager& manager) const
        {
            return manager.findVS("simpleVS");
        }

        virtual ShaderPSBase* createPS(Material& material, const ShaderManager& manager) const
        {
            return manager.findPS("simplePS");
        }
    };
}

int _tmain(int argc, TCHAR* argv[])
{
    lcore::memory::LeakCheck check;

    lgraphics::Window window;
    {
        lgraphics::Window::InitParam param;
        param.width_ = 600;
        param.height_ = 400;
        param.setTitle("converter", 9);
        if(false == window.initialize(param, true)){
            return -1;
        }

        window.setShow(false);
    }

    {
        lgraphics::InitParam param;
        param.backBufferWidth_ = 600;
        param.backBufferHeight_ = 400;
        param.isClearColor_ = true;
        param.isClearDepth_ = true;
        param.clearColor_ = 0xFF0000FFU;
        param.clearDepth_ = 1.0f;
        param.interval_ = lgraphics::PresentInterval_One;
        param.windowHandle_ = window.getHandle().hWnd_;


        if(false == lgraphics::Graphics::initialize(param)){
            return -1;
        }
    }

    lgraphics::GraphicsDeviceRef &device = lgraphics::Graphics::getDevice();
    device.setViewport(0, 0, 600, 400);

    lgraphics::AnimObject *animObj = NULL;
    u32 numIKChains = 0;
    ltools::IKChain *ikChain_ = NULL;

    //IK
    pmd::LoaderPmd::IKPack ikPack;
    const char* filepath = "data/1052/1052.pmd";
    {
        

        animObj = pmd::LoaderPmd::load(filepath, &ikPack);

        //IK準備
        numIKChains = ikPack.size_;
        ikChain_ = LIME_NEW ltools::IKChain[ numIKChains ];
    }

    animObj->initializeShader();
    {
        lcore::ofstream out("data/m1052.lm", lcore::ios::out|lcore::ios::binary);
        io::IOObject::write(out, *animObj);
        out.close();

        LIME_DELETE(animObj);
    }
    {
        animObj = LIME_NEW lgraphics::AnimObject;
        lcore::ifstream is("data/m1052.lm", std::ios::binary);

        is.seekg(0, lcore::ios::end);
        u32 size = is.tellg();
        is.seekg(0, lcore::ios::beg);
        char *buffer = LIME_NEW char[size];
        is.read(buffer, size);
        is.close();

        lcore::isstream iss(buffer, size);
        io::IOObject::read(iss, *animObj);
        animObj->initializeShader();
        LIME_DELETE_ARRAY(buffer);

        //IK準備
        for(u32 i=0; i<numIKChains; ++i){
            ikChain_[i].create( *animObj, ikPack.iks_[i] );
        }
        lcore::ofstream out("data/m1052.lik", lcore::ios::binary);
        ltools::IOIK::write(out, ikChain_, numIKChains);
    }


    AnimDatabase *animDB = NULL;
    {
        const char* filepath = "data/azunyan214.vmd";
        vmd::LoaderVmd loaderVmd;
        bool ret = loaderVmd.readFile(filepath);
        LASSERT(ret);
        
        animDB = loaderVmd.createAnimDB(*animObj);

        lcore::ofstream out("data/azunyan214.la", lcore::ios::out|lcore::ios::binary);
        io::IOAnimDatabase::write(out, *animDB);
        LIME_DELETE(animDB);
    }

    {
        animDB = LIME_NEW AnimDatabase;

        lcore::ifstream is("data/azunyan214.la", lcore::ios::in|lcore::ios::binary);
        is.seekg(0, lcore::ios::end);
        u32 size = is.tellg();
        is.seekg(0, lcore::ios::beg);
        char *buffer = new char[size];
        is.read(buffer, size);
        is.close();

        lcore::isstream iss(buffer, size);
        io::IOAnimDatabase::read(iss, *animDB);
        LIME_DELETE_ARRAY(buffer);
    }

    lgraphics::AnimControler *animControler = LIME_NEW lgraphics::AnimControler(*animObj);

    lgraphics::Scene scene;

    f32 counter_ = 0.0f;

    bool loop = true;
    while(loop)
    {
        if(false == window.peekMessage()){
            break;
        }
        lmath::Matrix44 matView;
        matView.identity();
        matView.lookAt(
            lmath::Vector3(0.0f, 0.0f, -50.0f),
            //lmath::Vector3(0.0f, 0.0f, -20.0f),
            lmath::Vector3(0.0f, 0.0f, 0.0f),
            lmath::Vector3(0.0f, 1.0f, 0.0f));

        lmath::Matrix44 proj;
        proj.identity();
        proj.perspectiveFov(PI * 60.0f/180.0f, 600.0f/400.0f, 1.0f, 200.0f);

        scene.setViewMatrix(matView);
        scene.setProjMatrix(proj);
        scene.updateMatrix();

        //アニメーション更新
        animControler->update(*animObj, *animDB, counter_);

        for(u32 i=0; i<numIKChains; ++i){
            //ikChain_[i].calc( *animObj );
        }

        counter_ += 0.5f;
        if(counter_ >= 200.0f){
            counter_ = 0.0f;
        }

        device.present(NULL, NULL, NULL);
        device.clear();
        device.beginScene();

        lmath::Matrix43 mat;
        mat.identity();
        //mat.rotateY(counter_*(0.5f/PI));
        mat.translate(0.0f, -0.5f, 0.0f);
        animObj->setWorldMatrix(mat);
        animObj->draw(scene);

        device.endScene();
    }

    LIME_DELETE_ARRAY(ikChain_);
    LIME_DELETE(animControler);
    LIME_DELETE(animDB);
    LIME_DELETE(animObj);

    ShaderManager::terminate();
    lgraphics::Graphics::terminate();
    window.destroy();

	return 0;
}

