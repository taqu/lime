#ifndef INC_LCORE_THREAD_H__
#define INC_LCORE_THREAD_H__
/**
@file Thread.h
@author t-sakai
@date 2011/08/06 create
*/
#include "../lcore.h"
#include "SyncObject.h"

namespace lcore
{
    /**
    @brief �v���Z�b�T���擾
    */
    u32 getNumberOfProcessors();

    //----------------------------------------------------
    //---
    //--- Thread
    //---
    //----------------------------------------------------
    class Thread
    {
    public:
        enum WaitStatus
        {
            Wait_Abandoned = WAIT_ABANDONED, /// ���L���Ă����I�u�W�F�N�g��������Ȃ��ŏI������
            Wait_Success = WAIT_OBJECT_0 , /// �V�O�i����ԂɂȂ���
            Wait_Timeout = WAIT_TIMEOUT , /// �^�C���A�E�g����
            Wait_Failed = WAIT_FAILED , /// �֐����s
        };

        static const u32 Infinite = 0xFFFFFFFFU; /// �^�C���A�E�g���ԂɎw�肷��ƁA�V�O�i����ԂɂȂ�܂őҋ@

        Thread();
        virtual ~Thread();

        /**
        @return ����
        @brief �X���b�h��ҋ@��Ԃō쐬
        */
        bool create();

        /// �X���b�hID�擾
        u32 getID() const
        {
            return id_;
        }

        /// �J�n
        void start();

        /// �ĊJ
        void resume();

        /// �ꎞ��~
        void suspend();

        /// �I��
        void stop();

        /**
        @brief �I����҂�
        @return ����
        @param timeout ... �^�C���A�E�g����
        */
        WaitStatus join(u32 timeout=Infinite);

    protected:
        /// �X���b�h�I�����ׂ���
        bool canRun();

        /// �����{��
        virtual void run() =0;

    private:
        void release();

        static u32 __stdcall proc(void* args);

        Thread(const Thread&);
        Thread& operator=(const Thread&);

        HANDLE handle_;
        u32 id_;
        CriticalSection cs_;
        bool canRun_;
        bool suspend_;
    };
}

#endif //INC_LCORE_THREAD_H__

