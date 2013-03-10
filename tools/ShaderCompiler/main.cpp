#include <iostream>
#include <string>

#include <lcore/liostream.h>
#include <lmath/lmath.h>
#include <lgraphics/lgraphics.h>
#include <lgraphics/api/InitParam.h>
#include <lgraphics/Window.h>
#include <lgraphics/api/InputLayoutRef.h>
#include <lgraphics/api/ShaderRef.h>
#include <lgraphics/api/BlobRef.h>

enum ShaderType
{
    Shader_VS,
    Shader_GS,
    Shader_PS,
    Shader_None,
};

int main(int argc, char** argv)
{
    if(argc < 3){
        return 0;
    }

    ShaderType type = Shader_None;
    bool ps = false;
    int numFiles = 0;
    std::string files[2];
    for(int i=1; i<argc; ++i){
        if(lcore::strncmp(argv[i], "-ps", 3) == 0){
            type = Shader_PS;
        }else if(lcore::strncmp(argv[i], "-gs", 3) == 0){
            type = Shader_GS;
        }else if(lcore::strncmp(argv[i], "-vs", 3) == 0){
            type = Shader_VS;
        }else{
            if(numFiles<2){
                files[numFiles].assign(argv[i]);
                ++numFiles;
            }
        }
    }
    if(type == Shader_None){
        return 0;
    }
    if(numFiles<2){
        return 0;
    }

    static const char Title[] = "compiler";
    lgraphics::Window window;
    //ウィンドウ作成。デバイスのパラメータにウィンドウハンドルを渡す
    lgraphics::Window::InitParam windowParam;
    windowParam.width_ = 800;
    windowParam.height_ = 600;
    windowParam.setTitle(Title, sizeof(Title));
    windowParam.wndProc_ = NULL;
    bool ret = window.initialize(windowParam, true);
    if(false == ret){
        return false;
    }

    lgraphics::InitParam param;
    param.type_ = lgraphics::DriverType_Hardware;
    param.backBufferWidth_ = windowParam.width_;
    param.backBufferHeight_ = windowParam.height_;
    param.refreshRate_ = 60;
    param.windowHandle_ = window.getHandle().hWnd_;
    param.windowed_ = 1;
    param.interval_ = lgraphics::PresentInterval_One;
    param.swapEffect_ = lgraphics::DXGISwapEffect_Discard;

    if(false == lgraphics::Graphics::initialize(param)){
        window.terminate();
        return false;
    }

    window.setShow(false);


    {
        lgraphics::BlobRef blob;
        lgraphics::VertexShaderRef vertexShader;
        lgraphics::GeometryShaderRef geometryShader;
        lgraphics::PixelShaderRef pixelShader;
        switch(type)
        {
        case Shader_VS:
            vertexShader = lgraphics::Shader::createVertexShaderFromFile(files[0].c_str(), 0, NULL, &blob);
            break;

        case Shader_GS:
            geometryShader = lgraphics::Shader::createGeometryShaderFromFile(files[0].c_str(), 0, NULL, &blob);
            break;

        case Shader_PS:
            pixelShader = lgraphics::Shader::createPixelShaderFromFile(files[0].c_str(), 0, NULL, &blob);
            break;

        };

        lgraphics::ShaderCompresser compresser;
        lcore::u8* data = reinterpret_cast<lcore::u8*>(blob.getPointer());
        if(blob.valid() && compresser.compress(blob.getSize(), data)){

            lcore::ofstream out(files[1].c_str(), lcore::ios::binary);
            if(out.is_open()){
                lcore::u32 size = compresser.getSize();
                const lcore::u8* compressed = compresser.getData();

                int count = 0;
                for(lcore::u32 i=0; i<size; ++i){
                    out.print("0x%02XU,", compressed[i]);
                    if(++count>20){
                        count = 0;
                        out.write("\n", 1);
                    }
                }
                out.close();
            }
        }
    }

    lgraphics::Graphics::terminate();
    window.terminate();
    return 0;
}
