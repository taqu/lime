/**
@file IOShaderKey.cpp
@author t-sakai
@date 2010/05/06 create

*/
#include "IOShaderKey.h"

#include "SectionID.h"
#include "../api/ShaderKey.h"

namespace lgraphics
{
namespace io
{

    bool IOShaderKey::read(lcore::istream& is, ShaderKey& key)
    {
        u32 flags[2] = {0,0};
        lcore::io::read(is, flags, sizeof(u32)*2);

        key.setInputFlags(flags[0]);
        key.setFeatureFlags(flags[1]);
        return true;
    }


    bool IOShaderKey::write(lcore::ostream& os, const ShaderKey& key)
    {
        u32 flags[2] = {0,0};
        flags[0] = key.getInputFlags();
        flags[1] = key.getFeatureFlags();

        lcore::io::write(os, flags, sizeof(u32)*2);
        return true;
    }
}
}
