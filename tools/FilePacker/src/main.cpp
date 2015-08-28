#include <cassert>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include <iostream>
#include <lcore/liostream.h>

#include <lframework/pack/lpacktree.h>
#include <lframework/pack/PackWriter.h>

namespace lpacktree
{
    bool isFile(const WIN32_FIND_DATA& data)
    {
        static const DWORD checks[] =
        {
            FILE_ATTRIBUTE_HIDDEN,
            FILE_ATTRIBUTE_SYSTEM,
            FILE_ATTRIBUTE_DIRECTORY,
            FILE_ATTRIBUTE_ENCRYPTED,
            FILE_ATTRIBUTE_TEMPORARY,
            FILE_ATTRIBUTE_SPARSE_FILE,
        };
        static const s32 Num = sizeof(checks)/sizeof(DWORD);
        for(int i=0; i<Num; ++i){
            DWORD check = checks[i] & data.dwFileAttributes;
            if(check != 0){
                return false;
            }
        }
        return true;
    }

    bool isDirectory(const WIN32_FIND_DATA& data)
    {
        static const DWORD checks[] =
        {
            FILE_ATTRIBUTE_HIDDEN,
            FILE_ATTRIBUTE_SYSTEM,
            FILE_ATTRIBUTE_ENCRYPTED,
            FILE_ATTRIBUTE_TEMPORARY,
            FILE_ATTRIBUTE_SPARSE_FILE,
        };
        static const s32 Num = sizeof(checks)/sizeof(DWORD);
        for(int i=0; i<Num; ++i){
            DWORD check = checks[i] & data.dwFileAttributes;
            if(check != 0){
                return false;
            }
        }
        
        return 0 != (FILE_ATTRIBUTE_DIRECTORY & data.dwFileAttributes);
    }

    void traverse(lpacktree::PackWriter& writer, const std::string& path, const char* name)
    {
        std::string inpath = path;
        inpath += "*";

        lpacktree::FileEntry* directory = NULL;
        if(NULL != name){
            writer.push_back(name, 0, lpacktree::Type_Directory, 0, NULL);
            directory = &writer.get(writer.size()-1);
        }

        WIN32_FIND_DATA findData;
        HANDLE handle = FindFirstFile(inpath.c_str(), &findData);
        if(INVALID_HANDLE_VALUE == handle){
            return;
        }

        std::string filepath;
        lpacktree::u32 size = 0;
        lcore::ifstream in;
        lpacktree::s32 numFiles = 0;
        lpacktree::s8* buffer = NULL;
        for(;;){
            if(lpacktree::isFile(findData)){
                std::cerr << findData.cFileName << std::endl;
                filepath = path;
                filepath.append(findData.cFileName);

                in.open(filepath.c_str(), lcore::ios::binary);
                if(in.is_open()){
                    in.seekg(0, std::ios::end);
                    size = in.tellg();
                    in.seekg(0, std::ios::beg);
                    buffer = new lpacktree::s8[size];
                    in.read(buffer, size);
                    in.close();
                    
                    writer.push_back(findData.cFileName, size, lpacktree::Type_File, 0, buffer);
                    ++numFiles;
                    delete[] buffer;
                }
                
            }
            if(FALSE == FindNextFile(handle, &findData)){
                break;
            }
        }
        FindClose(handle);

        handle = FindFirstFile(inpath.c_str(), &findData);
        if(INVALID_HANDLE_VALUE == handle){
            return;
        }

        for(;;){
            if(lpacktree::isDirectory(findData) && '.' != findData.cFileName[0]){
                std::cerr << findData.cFileName << std::endl;
                filepath = path;
                filepath.append(findData.cFileName);
                filepath.append("\\");
                traverse(writer, filepath, findData.cFileName);
                ++numFiles;
                
            }
            if(FALSE == FindNextFile(handle, &findData)){
                break;
            }
        }
        FindClose(handle);

        if(NULL != directory){
            directory->numChildren_ = numFiles;
        }
    }
}

int main(int argc, char** argv)
{
    //static char* debug[] = {"cmd", "./src/", "out.pak", "-list", "list.txt"};
    //argc = 5;
    //argv = debug;

    if(argc<3){
        std::cerr << argv[0] << " directory name" << std::endl;
        return 0;
    }

    int inIndex = -1;
    int outIndex = -1;
    int listIndex = -1;
    for(int i=1; i<argc; ++i){
        if(strcmp(argv[i], "-list") == 0){
            int j = i+1;
            if(j<argc){
                listIndex = j;
                ++i;
            }
        }else if(inIndex<0){
            inIndex = i;
        }else{
            outIndex = i;
        }
    }
    if(inIndex<0 || outIndex<0){
        std::cerr << "invalid args" << std::endl;
        return 0;
    }

    {
        std::string name = argv[inIndex];
        if(name[0] == '.'){
            name = "";
        }

        std::string path = argv[inIndex];
        lpacktree::u32 len = path.size();
        if('\\' != path[len-1]
        && '/' != path[len-1])
        {
            path += '\\';
        }

        lpacktree::PackWriter writer;
        lpacktree::traverse(writer, path, name.c_str());

        writer.write(argv[outIndex]);

        if(listIndex>=0){
            writer.writeList(argv[listIndex]);
        }
    }

    return 0;
}
