#ifndef INC_LFRAMEWORK_COMPONENTMANAGER_H_
#define INC_LFRAMEWORK_COMPONENTMANAGER_H_
/**
@file ComponentManager.h
@author t-sakai
@date 2016/05/18 create
*/
#include "lecs.h"
#include "Entity.h"

namespace lfw
{
    class ComponentID;

    template<class Derived>
    struct ComponentDataCommon
    {
        inline u32 type() const{ return typeFlags_>>8;}
        inline u32 flags() const{ return typeFlags_&0xFFU;}
        inline void setType(u32 type)
        {
            typeFlags_ = (type<<8) | (typeFlags_&0xFFU);
        }
        inline bool checkFlag(u8 flag) const
        {
            return 0 != (typeFlags_&flag);
        }
        inline void setFlag(u8 flag)
        {
            typeFlags_ |= flag;
        }
        inline void resetFlag(u8 flag)
        {
            typeFlags_ &= ~(static_cast<u32>(flag));
        }
        inline void clearFlags()
        {
            typeFlags_ &= ~(0xFFU);
        }
        inline void clear(u32 type)
        {
            typeFlags_ = type<<8;
        }

        Entity entity_;
        u32 typeFlags_;
    };

    class ComponentManager
    {
    public:
        static const u8 CommonFlag_Start = 0x01U<<0;

        virtual ~ComponentManager()
        {}

        virtual u8 category() const =0;

        virtual void destroy(ID id) =0;

        virtual Behavior* getBehavior(ID id) =0;

        virtual void initialize() =0;
        virtual void terminate() =0;

        virtual void updateComponent(ID id) =0;

    protected:
        ComponentManager(const ComponentManager&);
        ComponentManager& operator=(const ComponentManager&);

        ComponentManager()
        {}
    };
}
#endif //INC_LFRAMEWORK_COMPONENTMANAGER_H_
