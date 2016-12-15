#include <iostream>
#include <string>

#include <lcore/liostream.h>
#include <lmath/lmath.h>
#include <lgraphics/Graphics.h>
#include <lgraphics/InitParam.h>
#include <lgraphics/Window.h>
#include <lgraphics/InputLayoutRef.h>
#include <lgraphics/ShaderRef.h>
#include <lgraphics/dx11/ShaderCompiler.h>
#include <lgraphics/BlobRef.h>

typedef lgfx::ShaderCompiler<0> ConcreteShaderCompiler;

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
    bool force = false;
    bool compress = false;
    bool text = false;
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
        }else if(lcore::strncmp(argv[i], "-ds", 3) == 0){
            type = Shader_DS;
        }else if(lcore::strncmp(argv[i], "-hs", 3) == 0){
            type = Shader_HS;
        }else if(lcore::strncmp(argv[i], "-p4", 3) == 0){
            profile = Profile_4_0;
        }else if(lcore::strncmp(argv[i], "-p5", 3) == 0){
            profile = Profile_5_0;
        }else if(lcore::strncmp(argv[i], "-force", 6) == 0){
            force = true;
        }else if(lcore::strncmp(argv[i], "-compress", 9) == 0){
            compress = true;
        }else if(lcore::strncmp(argv[i], "-text", 5) == 0){
            text = true;
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

    //static const char Title[] = "Shader Compiler";
    //lgfx::Window window;
    ////ウィンドウ作成。デバイスのパラメータにウィンドウハンドルを渡す
    //lgfx::Window::InitParam windowParam;
    //windowParam.width_ = 800;
    //windowParam.height_ = 600;
    //windowParam.setTitle(Title, sizeof(Title));
    //windowParam.wndProc_ = NULL;
    //bool ret = window.initialize(windowParam, true);
    //if(false == ret){
    //    return false;
    //}

    //lgfx::InitParam initParam;
    //initParam.type_ = lgfx::DriverType_Hardware;
    //initParam.backBufferWidth_ = windowParam.width_;
    //initParam.backBufferHeight_ = windowParam.height_;
    //initParam.refreshRate_ = 60;
    //initParam.windowHandle_ = window.getHandle().hWnd_;
    //initParam.windowed_ = 1;
    //initParam.interval_ = lgfx::PresentInterval_One;
    //initParam.swapEffect_ = lgfx::DXGISwapEffect_Discard;

    //if(false == lgfx::initializeGraphics(initParam)){
    //    window.terminate();
    //    return false;
    //}

    //window.setShow(false);


    {
        lgfx::BlobRef blob;
        lgfx::BlobRef error;

        lcore::ifstream in(files[0].c_str(), lcore::ios::binary);
        if(!in.is_open()){
            std::cerr << "can't open file: " << files[0] << std::endl;
            //lgfx::terminateGraphics();
            //window.terminate();
            return -1;
        }

        lcore::u32 size = in.getSize();

        lcore::ScopedArrayPtr<lcore::Char> buff(LNEW lcore::Char[size]);
        in.read(buff.get(), size);
        in.close();

        lcore::ScopedPtr<lgfx::ShaderInculde> shaderIncude(LNEW lgfx::ShaderInculde);
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

        if(error.valid()){
            const lcore::Char* msg = (const lcore::Char*)error.getPointer();
            std::cerr << msg << std::endl;
        }

        if(!blob.valid()){
            std::cerr << "error: ";
            if(error.valid()){
                const lcore::Char* msg = (const lcore::Char*)error.getPointer();
                std::cerr << msg;
            }
            std::cerr << std::endl;
            //lgfx::terminateGraphics();
            //window.terminate();
            return -1;

        }

        lcore::s32 outBuffSize = static_cast<lcore::s32>(sizeof(lgfx::BlobPack) + blob.getSize()*4);
        lcore::ScopedArrayPtr<lcore::u8> outBuff(LNEW lcore::u8[outBuffSize]);

        lcore::s32 result = lgfx::packBlob(outBuffSize, outBuff, blob, compress);
        if(result<=0){
            std::cerr << "fail to pack compiled shader" << std::endl;
            //lgfx::terminateGraphics();
            //window.terminate();
            return -1;
        }

        lcore::ofstream out(files[1].c_str(), lcore::ios::binary);
        if(!out.is_open()){
            std::cerr << "can't open file: " << files[1] << std::endl;
            //lgfx::terminateGraphics();
            //window.terminate();
            return -1;
        }
        if(text){
            int count = 0;
            for(lcore::s32 i=0; i<result; ++i){
                out.print("0x%02XU,", outBuff[i]);
                if(++count>20){
                    count = 0;
                    out.write("\n", 1);
                }
            }
        } else{
            lcore::io::write(out, outBuff.get(), result);
        }
        out.close();
    }

    //lgfx::terminateGraphics();
    //window.terminate();
    return 0;
}
