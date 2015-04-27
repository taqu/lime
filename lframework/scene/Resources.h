#ifndef INC_LSCENE_RESOURCES_H__
#define INC_LSCENE_RESOURCES_H__
/**
@file Resources.h
@author t-sakai
@date 2014/12/08 create
*/
#include "lscene.h"
#include <lcore/HashMap.h>
#include <lcore/allocator/StackAllocator.h>

namespace lscene
{
namespace lfile
{
    class SharedFile;
}

    class Resource;

    /**
    @brief ���\�[�X�L���b�V���B
    
    NumResourceCategory�ɕ��ނ��ĕۑ�
    */
    class Resources : public ResourcesAllocator
    {
    public:
        Resources();
        ~Resources();

        /**
        @brief ����
        @return ���\�[�X�B�Ȃ����NULL
        @param category ... ����
        @param id ... �����L�[
        */
        Resource* find(s32 category, u64 id);

        /**
        @brief �ǉ�
        @return ����
        @param category ... ����
        @param id ... �����L�[
        @param resource ... ���\�[�X
        @param traits ... ���\�[�X�^�C�v
        */
        bool add(s32 category, u64 id, Resource* resource);

        /**
        @brief �폜
        @param category ... ����
        @param resource ... ���\�[�X
        */
        void remove(s32 category, Resource* resource);

        /**
        @brief ���\�[�X�N���A
        @param category ... ����
        */
        void clear(s32 category);

        /**
        @brief ���\�[�X�N���A
        */
        void clearAll();
    private:
        Resources(const Resources&);
        Resources& operator=(const Resources&);

        static const u32 PageSize = 2*1024;

        struct Entry
        {
            u64 id_;
            Resource* resource_;
        };

        typedef lcore::HopscotchHashMap<u64, Entry*> IDToEntryMap;
        typedef lcore::StackAllocator<lscene::SceneAllocator> EntryAllocator;

        /// �G���g���폜
        void destroy(Entry* entry);

        IDToEntryMap idToEntry_[NumResourceCategory];
        EntryAllocator entryAllocator_;
    };
}
#endif //INC_LSCENE_RESOURCES_H__
