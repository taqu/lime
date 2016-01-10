#include <iostream>
#include <string>

#include <lcore/liostream.h>
#include <lmath/lmath.h>
#include <lgraphics/lgraphics.h>
#include <lgraphics/api/InitParam.h>
#include <lgraphics/Window.h>
#include <lgraphics/api/InputLayoutRef.h>
#include <lgraphics/api/ShaderRef.h>
#include <lgraphics/api/dx11/ShaderCompiler.h>
#include <lgraphics/api/BlobRef.h>

typedef lgraphics::ShaderCompiler<0> ConcreteShaderCompiler;

enum ShaderType
{
    Shader_VS,
    Shader_GS,
    Shader_PS,
    Shader_CS,
    Shader_DS,
    Shader_HS,
    Shader_None,
};

enum Profile
{
    Profile_4_0 =0,
    Profile_5_0,
};

const lcore::Char* ProfilePSName[] =
{
    "ps_4_0",
    "ps_5_0",
};

const lcore::Char* ProfileVSName[] =
{
    "vs_4_0",
    "vs_5_0",
};

const lcore::Char* ProfileGSName[] =
{
    "gs_4_0",
    "gs_5_0",
};

const lcore::Char* ProfileCSName[] =
{
    "cs_4_0",
    "cs_5_0",
};

const lcore::Char* ProfileDSName[] =
{
    "",
    "ds_5_0",
};

const lcore::Char* ProfileHSName[] =
{
    "",
    "hs_5_0",
};

int main(int argc, char** argv)
{
    if(argc < 3){
        return 0;
    }

    Profile profile = Profile_4_0;

    ShaderType type = Shader_None;
    bool ps = false;
    bool force = false;
    int numFiles = 0;
    std::string files[2];
    for(int i=1; i<argc; ++i){
        if(lcore::strncmp(argv[i], "-ps", 3) == 0){
            type = Shader_PS;
        }else if(lcore::strncmp(argv[i], "-gs", 3) == 0){
            type = Shader_GS;
        }else if(lcore::strncmp(argv[i], "-vs", 3) == 0){
            type = Shader_VS;
        }else if(lcore::strncmp(argv[i], "-cs", 3) == 0){
            type = Shader_CS;
        } else if(lcore::strncmp(argv[i], "-ds", 3) == 0){
            type = Shader_DS;
        } else if(lcore::strncmp(argv[i], "-hs", 3) == 0){
            type = Shader_HS;
        }else if(lcore::strncmp(argv[i], "-p4", 3) == 0){
            profile = Profile_4_0;
        }else if(lcore::strncmp(argv[i], "-p5", 3) == 0){
            profile = Profile_5_0;
        } else if(lcore::strncmp(argv[i], "-force", 6) == 0){
            force = true;
        }else{
            if(numFiles<2){
                files[numFiles].assign(argv[i]);
                ++numFiles;
            }
        }
    }
    if(type == Shader_None){
        return -1;
    }
    if(numFiles<2){
        return -1;
    }

    if(false == force){
        int num = 0;
        FILETIME writeTime[2];
        for(int i=0; i<2; ++i){
            HANDLE file = CreateFile(files[i].c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if(NULL == file){
                continue;
            }

            if(GetFileTime(file, NULL, NULL, &writeTime[i])){
                ++num;
            }
            CloseHandle(file);
        }

        if(2<=num){
            if(writeTime[0].dwHighDateTime<writeTime[1].dwHighDateTime){
                return 0;
            } else if(writeTime[0].dwHighDateTime == writeTime[1].dwHighDateTime){
                if(writeTime[0].dwLowDateTime <= writeTime[1].dwLowDateTime){
                    return 0;
                }
            }
        }
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
        lgraphics::BlobRef error;
        lgraphics::VertexShaderRef vertexShader;
        lgraphics::GeometryShaderRef geometryShader;
        lgraphics::PixelShaderRef pixelShader;

        lcore::ifstream in(files[0].c_str(), lcore::ios::binary);
        if(!in.is_open()){
            std::cerr << "can't open file: " << files[0] << std::endl;
            lgraphics::Graphics::terminate();
            window.terminate();
            return -1;
        }

        lcore::u32 size = in.getSize(0);

        lcore::ScopedArrayPtr<lcore::Char> buff(LIME_NEW lcore::Char[size]);
        in.read(buff.get(), size);
        in.close();

        lgraphics::ShaderInculde* shaderIncude = LIME_NEW lgraphics::ShaderInculde;
        switch(type)
        {
        case Shader_VS:
            blob = ConcreteShaderCompiler::createVertexShaderBlobFromMemory(
                buff.get(),
                size,
                ProfileVSName[profile],
                0, NULL,
                shaderIncude,
                &error);
            break;

        case Shader_GS:
            blob = ConcreteShaderCompiler::createGeometryShaderBlobFromMemory(
                buff.get(),
                size,
                ProfileGSName[profile],
                0, NULL,
                shaderIncude,
                &error);
            break;

        case Shader_PS:
            blob = ConcreteShaderCompiler::createPixelShaderBlobFromMemory(
                buff.get(),
                size,
                ProfilePSName[profile],
                0, NULL,
                shaderIncude,
                &error);
            break;

        case Shader_CS:
            blob = ConcreteShaderCompiler::createComputeShaderBlobFromMemory(
                buff.get(),
                size,
                ProfileCSName[profile],
                0, NULL,
                shaderIncude,
                &error);
            break;

        case Shader_DS:
            if(Profile_5_0<=profile){
                blob = ConcreteShaderCompiler::createComputeShaderBlobFromMemory(
                    buff.get(),
                    size,
                    ProfileDSName[profile],
                    0, NULL,
                    shaderIncude,
                    &error);
            }
            break;

        case Shader_HS:
            if(Profile_5_0<=profile){
                blob = ConcreteShaderCompiler::createComputeShaderBlobFromMemory(
                    buff.get(),
                    size,
                    ProfileHSName[profile],
                    0, NULL,
                    shaderIncude,
                    &error);
            }
            break;

        };

        if(!blob.valid()){
            std::cerr << "error: ";
            if(error.valid()){
                const lcore::Char* msg = (const lcore::Char*)error.getPointer();
                std::cerr << msg;
            }
            std::cerr << std::endl;
            lgraphics::Graphics::terminate();
            window.terminate();
            return -1;

        }else{
            if(error.valid()){
                const lcore::Char* msg = (const lcore::Char*)error.getPointer();
                std::cerr << msg;
            }
        }

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

        LIME_DELETE(shaderIncude);
    }

    lgraphics::Graphics::terminate();
    window.terminate();
    return 0;
}
