/**
@file ShaderCompiler.cpp
@author t-sakai
@date 2015/01/26 create
*/
#include "ShaderCompiler.h"
#include <lcore/liostream.h>

namespace lgfx
{
    HRESULT __stdcall ShaderInculde::Open(D3D_INCLUDE_TYPE /*type*/, LPCSTR filename, LPCVOID /*parentData*/, LPCVOID* data, UINT* bytes)
    {
        lcore::ifstream file(filename, lcore::ios::binary);
        if(!file.is_open()){
            return E_FAIL;
        }
        u32 size = file.getSize();
        void* d = LMALLOC(size);
        file.read(d, size);
        file.close();
        *data = d;
        *bytes = size;
        return S_OK;
    }

    HRESULT __stdcall ShaderInculde::Close(LPCVOID data)
    {
        void* d = (void*)data;
        LFREE(d);
        return S_OK;
    }
}