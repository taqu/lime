/**
@file ldll.cpp
@author t-sakai
@date 2010/12/03 create

*/
#if defined(_WIN32)
#include <tchar.h>
#else
#include <stdlib.h>
#include <dlfcn.h>
#endif

#include "lcore.h"
#include "ldll.h"

namespace lcore
{
    DLL::DLL()
        :module_(NULL)
    {
    }

    DLL::DLL(const char* path)
    {
        open(path);
    }

    DLL::~DLL()
    {
        close();
    }

    bool DLL::is_open() const
    {
        return (module_ != NULL);
    }

    bool DLL::open(const char* path)
    {
        LASSERT(path != NULL);
        LASSERT(module_ == NULL);

        module_ = openDLL(path);
        return (module_ != NULL);
    }

    void DLL::close()
    {
        closeDLL(module_);
        module_ = NULL;
    }

    void* DLL::getProcAddress(const char* name)
    {
        LASSERT(module_ != NULL);
        LASSERT(name != NULL);
        return getProcAddressDLL(module_, name);
    }

#if defined(_WIN32)

    HMODULE openDLL(const char* path)
    {
        LASSERT(path != NULL);
        return LoadLibrary(path);
    }

    void closeDLL(HMODULE module)
    {
        if(module){
            FreeLibrary(module);
            module = NULL;
        }
    }

    void* getProcAddressDLL(HMODULE module, const char* name)
    {
        LASSERT(module != NULL);
        LASSERT(name != NULL);
        return (void*)GetProcAddress(module, name);
    }

#else

    HMODULE openDLL(const char* path)
    {
        LASSERT(path != NULL);
        return dlopen(path, RTLD_NOW);
    }

    void closeDLL(HMODULE module)
    {
        if(module){
            dlclose(module);
        }
    }

    void* getProcAddressDLL(HMODULE module, const char* name)
    {
        LASSERT(module != NULL);
        LASSERT(name != NULL);
        return (void*)dlsym(module, name);
    }
#endif
}
