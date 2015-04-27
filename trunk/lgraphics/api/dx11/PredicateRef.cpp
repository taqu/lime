/**
@file PredicateRef.cpp
@author t-sakai
@date 2014/10/10 create
*/
#include "PredicateRef.h"

namespace lgraphics
{
    PredicateRef::PredicateRef(pointer_type query)
        :AsynchronousRef(query)
    {
    }

    PredicateRef Predicate::create(
            QueryType type,
            QueryMiscFlag flag)
    {
        D3D11_QUERY_DESC  desc;
        desc.Query = (D3D11_QUERY)type;
        desc.MiscFlags = flag;

        ID3D11Device* device = Graphics::getDevice().getD3DDevice();
        ID3D11Predicate* predicate = NULL;

        HRESULT hr = device->CreatePredicate(&desc, &predicate);
        if(SUCCEEDED(hr)){
            return PredicateRef(predicate);
        }else{
            return PredicateRef();
        }
    }
}
