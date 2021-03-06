/**
@file QueryRef.cpp
@author t-sakai
@date 2014/10/10 create
*/
#include "QueryRef.h"
#include "../../Graphics.h"

namespace lgfx
{
    QueryRef::QueryRef(pointer_type query)
        :AsynchronousRef(query)
    {
    }

    QueryRef& QueryRef::operator=(const QueryRef& rhs)
    {
        QueryRef(rhs).swap(*this);
        return *this;
    }

    QueryRef& QueryRef::operator=(QueryRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            asynchronous_ =  rhs.asynchronous_;
            rhs.asynchronous_ = NULL;
        }
        return *this;
    }

    QueryRef Query::create(
            QueryType type,
            QueryMiscFlag flag)
    {
        D3D11_QUERY_DESC  desc;
        desc.Query = (D3D11_QUERY)type;
        desc.MiscFlags = flag;

        ID3D11Device* device = getDevice().getD3DDevice();
        ID3D11Query* query = NULL;

        HRESULT hr = device->CreateQuery(&desc, &query);
        if(SUCCEEDED(hr)){
            return QueryRef(query);
        }else{
            return QueryRef();
        }
    }
}
