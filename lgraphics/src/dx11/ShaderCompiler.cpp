/**
@file ShaderCompiler.cpp
@author t-sakai
@date 2015/01/26 create
*/
#include "ShaderCompiler.h"
#include <lcore/File.h>

namespace lgfx
{
    HRESULT __stdcall ShaderInculde::Open(D3D_INCLUDE_TYPE /*type*/, LPCSTR filename, LPCVOID /*parentData*/, LPCVOID* data, UINT* bytes)
    {
        lcore::File file(filename, lcore::ios::in);
        if(!file.is_open()){
            return E_FAIL;
        }
        s64 size = file.size();
        void* d = LMALLOC(size);
        file.read(size, d);
        file.close();
        *data = d;
        *bytes = static_cast<u32>(size);
        return S_OK;
    }

    HRESULT __stdcall ShaderInculde::Close(LPCVOID data)
    {
        void* d = (void*)data;
        LFREE(d);
        return S_OK;
    }
}
