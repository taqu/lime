#ifndef INC_LRESOURCE_RESOURCEDB_H__
#define INC_LRESOURCE_RESOURCEDB_H__
/**
@file ResourceDB.h
@author t-sakai
@date 2011/01/09 create
*/
#include <lcore/lcore.h>
#include <lcore/HashMap.h>
#include <lgraphics/api/TextureRef.h>

namespace lgraphics
{
    TextureRef;
}

namespace lresource
{
    using lcore::Char;
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;
    using lcore::u8;
    using lcore::u16;
    using lcore::u32;
    using lcore::f32;
    using lcore::f64;

    class ResourceDB
    {
    public:
        static const u32 DefaultSize = 32;

        typedef lcore::HashMapCharArray<lgraphics::TextureRef> NameToTextureMap;

        ResourceDB();
        ~ResourceDB();


        bool addTexture(lgraphics::TextureRef& texture);
        void eraseTexture(lgraphics::TextureRef& texture);

        lgraphics::TextureRef* findTexture(const lgraphics::NameString& name)
        {
            return findTexture(name.c_str(), name.size());
        }

        lgraphics::TextureRef* findTexture(const Char* name, u32 length);
    private:
        NameToTextureMap nameToTexture_;
    };
}
#endif //INC_LRESOURCE_RESOURCEDB_H__
