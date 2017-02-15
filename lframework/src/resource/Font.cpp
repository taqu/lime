/**
@file Font.cpp
@author t-sakai
@date 2016/12/25 create
*/
#include "resource/Font.h"
#include <lcore/Search.h>
#include "resource/Resources.h"

namespace lfw
{
    Font::Font()
        :fontSize_(0)
        ,lineHeight_(0)
        ,width_(0)
        ,height_(0)
        ,invWidth_(0)
        ,invHeight_(0)
        ,numChars_(0)
        ,chars_(NULL)
    {
        ShaderManager& shaderManager = Resources::getInstance().getShaderManager();
        ps_ = &shaderManager.getPS(ShaderPS_Font);
        psOutline_ = &shaderManager.getPS(ShaderPS_FontOutline);
    }

    Font::~Font()
    {
        LFREE(chars_);
    }

    const Font::FontChar* Font::find(u16 id) const
    {
        s32 index = lcore::binarySearchIndex(numChars_, chars_, id, comp);
        return (0<=index)? &chars_[index] : NULL;
    }
}
