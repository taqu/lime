#ifndef INC_SHADERMANAGER_H__
#define INC_SHADERMANAGER_H__
/**
@file ShaderManager.h
@author t-sakai
@date 2009/08/16 create
*/
#include <lframework/scene/lscene.h>
#include <lcore/HashMap.h>
#include <lcore/Noncopyable.h>
#include <lgraphics/api/ShaderKey.h>

namespace lgraphics
{
    struct Descriptor;
}

namespace lscene
{
    class ShaderBase;
    class ShaderVSBase;
    class ShaderPSBase;
    class ShaderCreator;
    class Material;
    class Geometry;

    class ProgramKey
    {
    public:
        ProgramKey()
            :vs_(NULL)
            ,ps_(NULL)
        {}

        ProgramKey(const lgraphics::Descriptor* vs, const lgraphics::Descriptor* ps)
            :vs_(vs)
            ,ps_(ps)
        {}

        const lgraphics::Descriptor* vs_;
        const lgraphics::Descriptor* ps_;

        bool operator==(const ProgramKey& rhs) const
        {
            return (vs_ == rhs.vs_) && (ps_ == rhs.ps_);
        }

        bool operator!=(const ProgramKey& rhs) const
        {
            return (vs_ != rhs.vs_) || (ps_ != rhs.ps_);
        }
    };

    class ProgramValue
    {
    public:
        ProgramValue()
            :vs_(NULL)
            ,ps_(NULL)
        {}

        ProgramValue(ShaderVSBase* vs, ShaderPSBase* ps)
            :vs_(vs)
            ,ps_(ps)
        {}

        ShaderVSBase* vs_;
        ShaderPSBase* ps_;

        bool operator==(const ProgramValue& rhs) const
        {
            return (vs_ == rhs.vs_) && (ps_ == rhs.ps_);
        }

        bool operator!=(const ProgramValue& rhs) const
        {
            return (vs_ != rhs.vs_) || (ps_ != rhs.ps_);
        }
    };

    class ShaderManager : private lcore::NonCopyable<ShaderManager>
    {
        ShaderManager(
            u32 size,
            lgraphics::ShaderKey& emptyKey
#if defined(LIME_GLES2)
            ,ProgramKey& emptyProgramKey
#endif
            );

        ~ShaderManager();
    public:
        static const u32 DefaultSize = 64;

        static const u32 NameLength = 32;
        typedef lcore::String<NameLength> NameString;

        static ShaderManager* getInstance()
        {
            LASSERT(instance_ != NULL);
            return instance_;
        }

        static void initialize(u32 size);
        static void terminate();
#if defined(LIME_GLES2)
        bool addVS(const lgraphics::ShaderKey& key, lgraphics::Descriptor* shader);
        bool addPS(const lgraphics::ShaderKey& key, lgraphics::Descriptor* shader);

        lgraphics::Descriptor* findVS(const lgraphics::ShaderKey& key);
        lgraphics::Descriptor* findPS(const lgraphics::ShaderKey& key);

        bool checkVS(const lgraphics::ShaderKey& key) const;
        bool checkPS(const lgraphics::ShaderKey& key) const;

        bool addProgram(ShaderVSBase* vs, ShaderPSBase* ps);

        bool findProgram(const lgraphics::Descriptor* vsDesc, const lgraphics::Descriptor* psDesc, ShaderVSBase*& vs, ShaderPSBase*& ps);

        bool checkProgram(const lgraphics::Descriptor* vs, const lgraphics::Descriptor* ps) const;

#else

        bool addVS(ShaderVSBase* shader);
        bool addPS(ShaderPSBase* shader);

        ShaderVSBase* findVS(const lgraphics::ShaderKey& key);
        ShaderPSBase* findPS(const lgraphics::ShaderKey& key);

        bool checkVS(const lgraphics::ShaderKey& key) const;
        bool checkPS(const lgraphics::ShaderKey& key) const;
#endif


        void setCreator(ShaderCreator* creator);
        const ShaderCreator& getCreator() const
        {
            LASSERT(creator_ !=NULL);
            return *creator_;
        }

        void create(Geometry& geometry,
            Material& material,
            ShaderVSBase*& vs,
            ShaderPSBase*& ps);


    private:
        static ShaderManager *instance_;
#if defined(LIME_GLES2)
        typedef lcore::HashMap<lgraphics::ShaderKey, lgraphics::Descriptor*> ShaderMap;

        ShaderMap vsMap_;
        ShaderMap psMap_;

        typedef lcore::HashMap<ProgramKey, ProgramValue> ProgramMap;
        ProgramMap programMap_;

#else
        typedef lcore::HashMap<lgraphics::ShaderKey, ShaderBase*> ShaderMap;

        ShaderMap vsMap_;
        ShaderMap psMap_;
#endif
        ShaderCreator *creator_;
    };
}

#endif //INC_SHADERMANAGER_H__
