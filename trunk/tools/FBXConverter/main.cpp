#include <iostream>
#include <string>

#include "converter.h"

int main(int argc, char** argv)
{
    if(argc < 3){
        return 0;
    }

    load::Manager manager;

    load::Scene scene;
    {
        load::Importer importer;
        if(!manager.initialize(importer, argv[1])){
            return 0;
        }
        if(!manager.import(scene, importer)){
            return 0;
        }
    }

    load::Converter converter;

    converter.process(scene.scene_);
    converter.out(argv[2]);
    return 0;
}
