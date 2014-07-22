#include <iostream>
#include <string>

#include "converter.h"

int main(int argc, char** argv)
{
    if(argc < 3){
        std::cout << "FBXConverter <input file> <output file>" << std::endl;
        std::cout << " options:" << std::endl;
        std::cout << "\t-anim <output animation file> \t(-anim animation.anim)" << std::endl;
        return 0;
    }

    std::string inFile;
    std::string outFile;
    std::string outSkeleton;
    std::string outAnimation;

    for(int i=1; i<argc; ++i){
        if(strcmp(argv[i], "-anim") == 0){
            int j = i+1;
            if(j<argc){
                outAnimation = argv[j];
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
    return 0;
}
