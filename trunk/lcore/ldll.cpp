/**
@file ldll.cpp
@author t-sakai
@date 2010/12/03 create

*/
#if defined(_WIN32)
#include <tchar.h>

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include <windows.h>

#else
#include <stdlib.h>
#include <dlfcn.h>
#endif

#include "ldll.h"

namespace lcore
{
    DLL::DLL()
        :module_(NULL)
    {
    }

    DLL::DLL(const char* path)
        :module_(NULL)
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

    LHMODULE openDLL(const char* path)
    {
        LASSERT(path != NULL);
        return (LHMODULE)LoadLibrary(path);
    }

    void closeDLL(LHMODULE module)
    {
        if(module){
            FreeLibrary((HMODULE)module);
            module = NULL;
        }
    }

    void* getProcAddressDLL(LHMODULE module, const char* name)
    {
        LASSERT(module != NULL);
        LASSERT(name != NULL);
        return (void*)GetProcAddress((HMODULE)module, name);
    }

#else

    HMODULE openDLL(const char* path)
    {
        LASSERT(path != NULL);
        return dlopen(path, RTLD_NOW);
    }

    void closeDLL(LHMODULE module)
    {
        if(module){
            dlclose(module);
        }
    }

    void* getProcAddressDLL(LHMODULE module, const char* name)
    {
        LASSERT(module != NULL);
        LASSERT(name != NULL);
        return (void*)dlsym(module, name);
    }
#endif
}
