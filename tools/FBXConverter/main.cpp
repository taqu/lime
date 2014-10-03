#include <iostream>
#include <string>

#include "converter.h"
#include "ObjConverter.h"

namespace
{
    static const int Flag_Fbx = 0;
    static const int Flag_Obj = 1;
}

int main(int argc, char** argv)
{
    if(argc < 3){
        std::cout << "FBXConverter <input file (.fbx, .obj)> <output file>" << std::endl;
        std::cout << " options:" << std::endl;
        std::cout << "\t-anim <output animation file> \t(-anim animation.anim) (only for FBX)" << std::endl;
        std::cout << "\t-scale <scale> (only for OBJ)" << std::endl;
        return 0;
    }

    std::string inFile;
    std::string outFile;
    std::string outSkeleton;
    std::string outAnimation;
    float scale = 1.0f;
    for(int i=1; i<argc; ++i){
        if(strcmp(argv[i], "-anim") == 0){
            int j = i+1;
            if(j<argc){
                outAnimation = argv[j];
                ++i;
            }

        }else if(strcmp(argv[i], "-scale") == 0){
            int j = i+1;
            if(j<argc){
                scale = atof(argv[j]);
                ++i;
            }

        }else if(inFile.size()<=0){
            inFile = argv[i];
        }else{
            outFile = argv[i];
        }
    }

    if(inFile.size()<=0){
        return 0;
    }

    int flag = Flag_Fbx;
    std::string::size_type pos = inFile.rfind('.');
    if(pos < inFile.size()){
        if(inFile.compare(pos, 4, ".obj") == 0){
            flag = Flag_Obj;
        }
    }

    if(Flag_Obj == flag){
        load::LoaderObj loaderObj;
        if(loaderObj.load(inFile.c_str())){
            if(0 < outFile.size()){
                loaderObj.save(outFile.c_str(), scale);
            }
        }

    }else{
        //Fbx
        load::Manager manager;

        load::Scene scene;
        {
            load::Importer importer;
            if(!manager.initialize(importer, inFile.c_str())){
                return 0;
            }
            if(!manager.import(scene, importer)){
                return 0;
            }
        }

        load::Converter converter;

        converter.process(scene.scene_);
        converter.out(outFile.c_str());
        if(0<outAnimation.size()){
            converter.outAnimation(outAnimation.c_str());
        }

        load::Converter::print(scene.scene_);
    }
    return 0;
}
