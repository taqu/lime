/**
@file Font.cpp
@author t-sakai
@date 2016/12/25 create
*/
#include "resource/Font.h"
#include <lcore/Search.h>
#include "System.h"
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
        ,numGlyphes_(0)
        ,glyphes_(NULL)
    {
        ShaderManager& shaderManager = System::getResources().getShaderManager();
        ps_ = &shaderManager.getPS(ShaderPS_Font);
        psOutline_ = &shaderManager.getPS(ShaderPS_FontOutline);
    }

    Font::~Font()
    {
        LFREE(glyphes_);
    }

    const Font::Glyph* Font::find(u16 id) const
    {
        s32 index = lcore::binarySearchIndex(numGlyphes_, glyphes_, id, comp);
        return (0<=index)? &glyphes_[index] : NULL;
    }
}
