#ifndef INC_LNET_SOCKET_H__
#define INC_LNET_SOCKET_H__
/**
@file Socket.h
@author t-sakai
@date 2011/08/06 create
*/
#include "../lcore.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#include <utility>
#include <iosfwd>

namespace lnet
{
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;

    using lcore::u8;
    using lcore::u16;
    using lcore::u32;

    using lcore::f32;
    using lcore::f64;

    using lcore::Char;

    /// �v���g�R���t�@�~��
    enum Family
    {
        Family_INET  = AF_INET,  /// IPv4
        Family_INET6 = AF_INET6, /// IPv6
    };

    /// �ʐM����
    enum SocketType
    {
        SocketType_STREAM = SOCK_STREAM, /// TCP
        SocketType_DGRAM  = SOCK_DGRAM,  /// UDP
        SocketType_SEQPACKET = SOCK_SEQPACKET,
    };

    /// �v���g�R��
    enum Protocol
    {
        Protocol_IP  = IPPROTO_IP,  /// �ʏ�͂�������w��
        Protocol_TCP = IPPROTO_TCP, /// �������Ă���
        Protocol_UDP = IPPROTO_UDP, /// �������Ă���
    };

    enum Flag
    {
        Flag_Passive = AI_PASSIVE, /// �҂������t���O
    };

    /// shutdown�Ɏg�p
    enum Shut
    {
        Shut_RECEIVE = SD_RECEIVE,
        Shut_SEND    = SD_SEND,
        Shut_BOTH    = SD_BOTH,
    };


    class SocketClient;
    class SocketServer;

    //----------------------------------------------------
    //---
    //--- SocketSystem
    //---
    //----------------------------------------------------
    /**
    @brief �\�P�b�g�V�X�e��

    �\�P�b�g�̎g�p�O��ɁA�������ƏI�����s���B
    */
    class SocketSystem
    {
    public:
        /// ������
        static bool initialize();

        /// �I��
        static void terminate();

        /**
        @brief ���O�̃G���[�R�[�h�擾
        */
        static s32 getError();

        static void printError(std::ostream& os, const Char* message);
    private:
        SocketSystem(){}
        ~SocketSystem(){}

        SocketSystem(const SocketSystem&);
        SocketSystem& operator=(const SocketSystem&);

        static SocketSystem instance_;

        WSADATA wsaData_; /// Windows�\�P�b�g���������
    };

    //----------------------------------------------------
    //---
    //--- AddrInfo
    //---
    //----------------------------------------------------
    /**
    @brief �ڑ����
    */
    class AddrInfo
    {
    public:
        /// �C�e���[�^
        class Iterator
        {
        public:
            Iterator()
                :addrInfo_(NULL)
            {}

            ~Iterator()
            {}

            bool operator==(const Iterator& rhs) const{ return addrInfo_ == rhs.addrInfo_;}
            bool operator!=(const Iterator& rhs) const{ return addrInfo_ != rhs.addrInfo_;}

            const addrinfo* operator->() const{ LASSERT(addrInfo_!=NULL); return addrInfo_;}

            const addrinfo& operator*() const{ LASSERT(addrInfo_!=NULL); return *addrInfo_;}

            void operator++()
            {
                LASSERT(addrInfo_!=NULL);
                addrInfo_ = addrInfo_->ai_next;
            }

        private:
            friend class AddrInfo;

            explicit Iterator(const addrinfo* info)
                :addrInfo_(info)
            {}

            const addrinfo* addrInfo_;
        };


        AddrInfo()
            :addrInfo_(NULL)
        {}

        ~AddrInfo()
        {
            release();
        }

        /**
        @brief �ڑ����擾
        @return ����
        @param node ... IP�܂��̓z�X�g��
        @param service ... �|�[�g
        @param family ... �v���g�R���t�@�~��
        @param type ... �ʐM����
        @param protocol ... �v���g�R��
        @param flag ... �t���O
        */
        bool get(
            const Char* node,
            const Char* service,
            Family family,
            SocketType type,
            Protocol protocol,
            s32 flag);

        /// �L����
        bool valid() const{ return addrInfo_ != NULL;}

        Iterator begin(){ return Iterator(addrInfo_);}
        Iterator begin() const{ return Iterator(addrInfo_);}

        Iterator end(){ return Iterator();}
        Iterator end() const{ return Iterator();}

        /// �擪�擾
        const addrinfo& first() const
        {
            LASSERT(addrInfo_ != NULL);
            return *addrInfo_;
        }
    private:
        AddrInfo(const AddrInfo&);
        AddrInfo& operator=(const AddrInfo&);

        /// ���
        void release();

        addrinfo* addrInfo_; /// ����
    };


    //----------------------------------------------------
    //---
    //--- SocketBase
    //---
    //----------------------------------------------------
    /**
    @brief �\�P�b�g�֐����b�p�[
    */
    class SocketBase
    {
    public:
        virtual ~SocketBase();

        /**
        @brief �\�P�b�g�쐬
        @return ����
        @param family ... �v���g�R���t�@�~��
        @param type ... �ʐM����
        @param protocol ... �v���g�R��
        */
        bool create(s32 family, s32 type, s32 protocol);

        /**
        @brief �\�P�b�g���L����
        @return �L���E����
        */
        bool valid() const{ return socket_ != INVALID_SOCKET;}

        /**
        @brief
        */
        void close();

        /**
        @brief �o�C�g��M
        @return �����Ȃ��M�����o�C�g���A���s�Ȃ�-1
        */
        inline s32 recv(Char* buff, s32 len, s32 flags);

        inline s32 recvfrom(Char* buff, s32 len, s32 flags, SOCKADDR* from, s32* fromlen);

        /**
        @brief �o�C�g���M
        @return �����Ȃ瑗�M�����o�C�g���A���s�Ȃ�-1
        */
        inline s32 send(const Char* buff, s32 len, s32 flags);

        inline s32 sendto(const Char* buff, s32 len, s32 flags, const SOCKADDR* to, s32 tolen);

        /**
        @brief �w������̐ڑ������
        @return ����
        @param ����ڑ�����
        */
        bool shutdown(Shut how);

    protected:
        SocketBase();

        /**
        @param family ... �v���g�R���t�@�~��
        @param type ... �ʐM����
        @param protocol ... �v���g�R��
        @param socket ... �\�P�b�g�n���h��
        */
        SocketBase(s32 family, s32 type, s32 protocol, SOCKET socket);

        /**
        @brief 
        @return ����
        */
        bool connect(const addrinfo& info);

        /**
        @brief 
        @return ����
        */
        bool connect(const SOCKADDR& info);

        /**
        @brief �\�P�b�g�ƃA�h���X�������т���
        @return ����
        */
        bool bind(const addrinfo& info);

        /**
        @brief �\�P�b�g�ƃA�h���X�������т���
        @return ����
        */
        bool bind(const SOCKADDR& info);

        /**
        @brief �ڑ��v���҂���
        @return ����
        @param backlog ... �v���L���[�̃T�C�Y
        */
        bool listen(s32 backlog);

        /**
        @brief �ڑ��v�����ЂƂ��o���A�ڑ��m�����\�P�b�g�쐬
        @return ����
        @param socket ... �o�́B�ڑ��m�������\�P�b�g
        @param addr ... �o�́B�ڑ�����
        @param addrlen ... �ڑ�����T�C�Y
        */
        bool accept(SocketClient& socket, SOCKADDR* addr, s32* addrlen);


        /// �X���b�v
        void swap(SocketBase& rhs);

        s32 family_; /// �v���g�R���t�@�~��
        s32 type_; /// �ʐM����
        s32 protocol_; /// �v���g�R��
        SOCKET socket_; /// �\�P�b�g�n���h��

    private:
        SocketBase(const SocketBase&);
        SocketBase& operator=(const SocketBase&);
    };

    // �o�C�g��M
    inline s32 SocketBase::recv(Char* buff, s32 len, s32 flags)
    {
        LASSERT(socket_ != INVALID_SOCKET);
        LASSERT(buff != NULL);
        LASSERT(len > 0);

        return ::recv(socket_, buff, len, flags);
    }

    // �o�C�g��M
    inline s32 SocketBase::recvfrom(Char* buff, s32 len, s32 flags, SOCKADDR* from, s32* fromlen)
    {
        LASSERT(socket_ != INVALID_SOCKET);
        LASSERT(buff != NULL);
        LASSERT(len > 0);

        return ::recvfrom(socket_, buff, len, flags, from, fromlen);
    }

    // �o�C�g���M
    inline s32 SocketBase::send(const Char* buff, s32 len, s32 flags)
    {
        LASSERT(socket_ != INVALID_SOCKET);
        LASSERT(buff != NULL);
        LASSERT(len > 0);

        return ::send(socket_, buff, len, flags);
    }

    // �o�C�g���M
    inline s32 SocketBase::sendto(const Char* buff, s32 len, s32 flags, const SOCKADDR* to, s32 tolen)
    {
        LASSERT(socket_ != INVALID_SOCKET);
        LASSERT(buff != NULL);
        LASSERT(len > 0);

        return ::sendto(socket_, buff, len, flags, to, tolen);
    }

    //----------------------------------------------------
    //---
    //--- SocketClient
    //---
    //----------------------------------------------------
    /**
    @brief �N���C�A���g�\�P�b�g
    */
    class SocketClient : public SocketBase
    {
    public:
        SocketClient()
        {}

        /**
        @param family ... �v���g�R���t�@�~��
        @param type ... �ʐM����
        @param protocol ... �v���g�R��
        @param socket ... �\�P�b�g�n���h��
        */
        SocketClient(s32 family, s32 type, s32 protocol, SOCKET socket)
            :SocketBase(family, type, protocol, socket)
        {}

        virtual ~SocketClient()
        {}

        /**
        @brief 
        @return ����
        */
        inline bool connect(const addrinfo& info)
        {
            return SocketBase::connect(info);
        }

        /**
        @brief 
        @return ����
        */
        inline bool connect(const SOCKADDR& info)
        {
            return SocketBase::connect(info);
        }

        /// �X���b�v
        inline void swap(SocketClient& rhs)
        {
            SocketBase::swap(rhs);
        }
    };

    //----------------------------------------------------
    //---
    //--- SocketServer
    //---
    //----------------------------------------------------
    /**
    @brief �T�[�o�\�P�b�g
    */
    class SocketServer : public SocketBase
    {
    public:
        SocketServer()
        {}

        /**
        @param family ... �v���g�R���t�@�~��
        @param type ... �ʐM����
        @param protocol ... �v���g�R��
        @param socket ... �\�P�b�g�n���h��
        */
        SocketServer(s32 family, s32 type, s32 protocol, SOCKET socket)
            :SocketBase(family, type, protocol, socket)
        {}

        virtual ~SocketServer()
        {}

        /**
        @brief �\�P�b�g�ƃA�h���X�������т���
        @return ����
        */
        inline bool bind(const addrinfo& info)
        {
            return SocketBase::bind(info);
        }

        /**
        @brief �\�P�b�g�ƃA�h���X�������т���
        @return ����
        */
        inline bool bind(const SOCKADDR& info)
        {
            return SocketBase::bind(info);
        }

        /**
        @brief �ڑ��v���҂���
        @return ����
        @param backlog ... �v���L���[�̃T�C�Y
        */
        inline bool listen(s32 backlog)
        {
            return SocketBase::listen(backlog);
        }

        /**
        @brief �ڑ��v�����ЂƂ��o���A�ڑ��m�����\�P�b�g�쐬
        @return ����
        @param socket ... �o�́B�ڑ��m�������\�P�b�g
        @param addr ... �o�́B�ڑ�����
        @param addrlen ... �ڑ�����T�C�Y
        */
        inline bool accept(SocketClient& socket, SOCKADDR* addr, s32* addrlen)
        {
            return SocketBase::accept(socket, addr, addrlen);
        }

        /// �X���b�v
        inline void swap(SocketServer& rhs)
        {
            return SocketBase::swap(rhs);
        }
    };
}

#endif //INC_LNET_SOCKET_H__
