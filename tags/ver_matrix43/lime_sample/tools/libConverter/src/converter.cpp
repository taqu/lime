#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <sstream>
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

#include <lgraphics/anim/AnimDatabase.h>
#include <lgraphics/anim/AnimControler.h>

#include <lgraphics/io/IOObject.h>
#include <lgraphics/io/IOAnimDatabase.h>

#include "loader/Pmd.h"
#include "IK.h"
#include "IOIK.h"

#include "IOUtil.h"

using namespace lcore;
using namespace lgraphics;

namespace
{
    static const char* LMExt = ".lm";
    static const char* LIKExt = ".lik";
    static const char* LMIKExt = ".lmik";
    static const char* LAExt = ".la";
    class Argument
    {
    public:
        static const int NumArgs = 3;

        Argument()
            :revertImage_(false)
            ,compress_(false)
        {
        }

        ~Argument()
        {
        }

        bool parse(int argc, const char** argv);
        bool error() const;

        void outUsage(std::ostream& os) const;

        bool revert() const{ return revertImage_;}
        bool compress() const{ return compress_;}
        const std::string& getPmd() const{ return filepathPmd_;}
        const std::string& getVmd() const{ return filepathVmd_;}

    private:
        bool revertImage_;
        bool compress_;
        std::string filepathPmd_;
        std::string filepathVmd_;
    };

    bool Argument::parse(int argc, const char** argv)
    {
        for(int i=0; i<argc;){
            int n = i+1;

            if(0 == std::strncmp(argv[i], "-pmd", 4)){
                if(n>=argc){
                    break;
                }
                filepathPmd_ = argv[n];
                i+=2;
                continue;

            }else if(0 == std::strncmp(argv[i], "-vmd", 4)){
                if(n>=argc){
                    break;
                }
                filepathVmd_ = argv[n];
                i+=2;
                continue;

            }else if(0 == std::strncmp(argv[i], "-revert", 6)){
                revertImage_ = true;

            }else if(0 == std::strncmp(argv[i], "-z", 2)){
                compress_ = true;
            }
            ++i;
        }
        return error();
    }

    bool Argument::error() const
    {
        if((filepathPmd_.size()<=0)
            && (filepathVmd_.size()<=0))
        {
            return false;
        }
        return true;
    }

    void Argument::outUsage(std::ostream& os) const
    {
        static const char* usage = "Usage: convert.exe -pmd filepath -vmd filepath [-revert]";
        os << usage << std::endl;
    }


    void chopExt(std::string& dst, const std::string& src)
    {
        std::string::size_type pos = src.rfind('.');
        if(pos == std::string::npos){
            dst.clear();
            return;
        }

        dst.assign(src.c_str(), pos);
    }

    void getName(std::string& dst, const std::string& src)
    {
        std::string::size_type pos = src.rfind('/');

        std::string tmp;
        if(pos != std::string::npos){
            tmp = src.substr(pos+1);
        }else{
            tmp = src;
        }
        chopExt(dst, tmp);
    }

}


void debug_check();


int _tmain(int argc, char* argv[])
{
    lcore::memory::LeakCheck check;

    Argument argument;
    if(false == argument.parse(argc, (const char**)argv)){
        argument.outUsage(std::cerr);
        return 0;
    }


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


    debug_check();

    if(argument.getPmd().size()>0){
        //IK
        pmd::LoaderPmd::IKPack ikPack;
        lcore::ScopedPtr<lgraphics::AnimObject> animObj = pmd::LoaderPmd::load(argument.getPmd().c_str(), &ikPack, argument.revert());
        if(!animObj){
            std::cerr << "\tcannot load " << argument.getPmd().c_str() << std::endl;
        }else{

            //IK準備
            u32 numIKChains = ikPack.size_;
            lcore::ScopedArrayPtr<ltools::IKChain> ikChain_ = LIME_NEW ltools::IKChain[ numIKChains ];
            {
                for(u32 i=0; i<numIKChains; ++i){
                    ikChain_[i].create( *animObj, ikPack.iks_[i] );
                }
            }


            std::string outLMIK;
            chopExt(outLMIK, argument.getPmd());
            outLMIK += LMIKExt;

            
            lcore::osstream ss(1024*1024);
            io::IOObject::write(ss, *animObj);
            //ltools::IOIK::write(ss, ikChain_.get(), numIKChains);
            std::cout << argument.getPmd().c_str() << " size " << ss.tellg() << std::endl;

            util::write(outLMIK.c_str(), ss.c_str(), ss.tellg(), argument.compress());
        }
    }


    if(argument.getVmd().size()>0){
        vmd::LoaderVmd loaderVmd;
        bool ret = loaderVmd.readFile(argument.getVmd().c_str());
        if(false == ret){
            std::cerr << "\tcannot load " << argument.getVmd().c_str() << std::endl;
            return 0;
        }

        lcore::ScopedPtr<AnimDatabase> animDB = loaderVmd.createAnimDB();
        if(animDB == NULL){
            std::cerr << "\tcannot load " << argument.getVmd().c_str() << std::endl;
            return 0;
        }

        std::string outLA;
        chopExt(outLA, argument.getVmd());
        outLA += LAExt;

        lcore::ofstream out(outLA.c_str(), lcore::ios::out|lcore::ios::binary);
        io::IOAnimDatabase::write(out, *animDB);
    }

    lgraphics::Graphics::terminate();
    window.destroy();

    return 0;
}

void debug_check()
{
#if 0
    // モデルのロードチェックしますか

    vmd::LoaderVmd loaderVmd;
    static const char* vmdData = "data/Dance_ver_B/hinsou.vmd";
    bool ret = loaderVmd.readFile(vmdData);
    if(false == ret){
        std::cout << "\tcannot load " << vmdData << std::endl;
        return;
    }
    {
        lcore::ScopedPtr<AnimDatabase> animDB = loaderVmd.createAnimDB();
        LASSERT(animDB != NULL);

        std::string outLA;
        chopExt(outLA, vmdData);
        outLA += LAExt;

        lcore::ofstream out(outLA.c_str(), lcore::ios::out|lcore::ios::binary);
        io::IOAnimDatabase::write(out, *animDB);
    }

    static const char* data[] =
    {
        "data/1052/1052.pmd",
        "data/1052C-Re0710/初音ミク-1052C-Re DIVA風水着EHs1.pmd",
        "data/Lat/Lat式ミクVer2.2_Normalエッジ無し専用.pmd",
        "data/MMD_Hachune_1.321/hachune.pmd",
        "data/MMD_nonowa_1.0/nonowa_model.pmd",
        "data/tewi/てゐ.pmd",
    };

    static const char* out_data[] =
    {
        "data/1052/m1052.pmd",
        "data/1052C-Re0710/m1052c_re_ehs1.pmd",
        "data/Lat/lat.pmd",
        "data/MMD_Hachune_1.321/hachune.pmd",
        "data/MMD_nonowa_1.0/nonowa_model.pmd",
        "data/tewi/twei.pmd",
    };

    static const bool revert[] =
    {
        false,
        true,
        false,
        true,
        true,
        false,
    };

    static const s32 numData = (sizeof(data)/sizeof(const char*));

    //IK
    pmd::LoaderPmd::IKPack ikPack;
    lgraphics::AnimObject *animObj = NULL;
    std::string outLM;
    lcore::ofstream out;
    for(s32 i=0; i<numData; ++i){
        animObj = pmd::LoaderPmd::load(data[i], &ikPack, revert[i]);
        if(animObj == NULL){
            LASSERT(false);
            continue;
        }

        {
            //IK準備
            u32 numIKChains = ikPack.size_;
            lcore::ScopedArrayPtr<ltools::IKChain> ikChain_ = LIME_NEW ltools::IKChain[ numIKChains ];
            {
                for(u32 j=0; j<numIKChains; ++j){
                    ikChain_[j].create( *animObj, ikPack.iks_[j] );
                }

                std::string outLIK;
                chopExt(outLIK, out_data[i]);
                outLIK += "_ik";
                outLIK += LIKExt;

                lcore::ofstream out(outLIK.c_str(), lcore::ios::binary);
                if(false == out.is_open()){
                    std::cout << "\tcannot open file " << outLIK.c_str() << std::endl;
                    continue;
                }

                ltools::IOIK::write(out, ikChain_.get(), numIKChains);
            }
        }

        chopExt(outLM, out_data[i]);
        outLM += LMExt;
        if(out.open(outLM.c_str(), lcore::ios::out|lcore::ios::binary)){
            io::IOObject::write(out, *animObj);
            out.close();
        }


        ikPack.release();
        LIME_DELETE(animObj);
    }


#endif
}

