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

    /// プロトコルファミリ
    enum Family
    {
        Family_INET  = AF_INET,  /// IPv4
        Family_INET6 = AF_INET6, /// IPv6
    };

    /// 通信方式
    enum SocketType
    {
        SocketType_STREAM = SOCK_STREAM, /// TCP
        SocketType_DGRAM  = SOCK_DGRAM,  /// UDP
        SocketType_SEQPACKET = SOCK_SEQPACKET,
    };

    /// プロトコル
    enum Protocol
    {
        Protocol_IP  = IPPROTO_IP,  /// 通常はこちらを指定
        Protocol_TCP = IPPROTO_TCP, /// 明示しても可
        Protocol_UDP = IPPROTO_UDP, /// 明示しても可
    };

    enum Flag
    {
        Flag_Passive = AI_PASSIVE, /// 待ちうけフラグ
    };

    /// shutdownに使用
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
    @brief ソケットシステム

    ソケットの使用前後に、初期化と終了を行う。
    */
    class SocketSystem
    {
    public:
        /// 初期化
        static bool initialize();

        /// 終了
        static void terminate();

        /**
        @brief 直前のエラーコード取得
        */
        static s32 getError();

        static void printError(std::ostream& os, const Char* message);
    private:
        SocketSystem(){}
        ~SocketSystem(){}

        SocketSystem(const SocketSystem&);
        SocketSystem& operator=(const SocketSystem&);

        static SocketSystem instance_;

        WSADATA wsaData_; /// Windowsソケット初期化情報
    };

    //----------------------------------------------------
    //---
    //--- AddrInfo
    //---
    //----------------------------------------------------
    /**
    @brief 接続情報
    */
    class AddrInfo
    {
    public:
        /// イテレータ
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
        @brief 接続情報取得
        @return 成否
        @param node ... IPまたはホスト名
        @param service ... ポート
        @param family ... プロトコルファミリ
        @param type ... 通信方式
        @param protocol ... プロトコル
        @param flag ... フラグ
        */
        bool get(
            const Char* node,
            const Char* service,
            Family family,
            SocketType type,
            Protocol protocol,
            s32 flag);

        /// 有効か
        bool valid() const{ return addrInfo_ != NULL;}

        Iterator begin(){ return Iterator(addrInfo_);}
        Iterator begin() const{ return Iterator(addrInfo_);}

        Iterator end(){ return Iterator();}
        Iterator end() const{ return Iterator();}

        /// 先頭取得
        const addrinfo& first() const
        {
            LASSERT(addrInfo_ != NULL);
            return *addrInfo_;
        }
    private:
        AddrInfo(const AddrInfo&);
        AddrInfo& operator=(const AddrInfo&);

        /// 解放
        void release();

        addrinfo* addrInfo_; /// 実態
    };


    //----------------------------------------------------
    //---
    //--- SocketBase
    //---
    //----------------------------------------------------
    /**
    @brief ソケット関数ラッパー
    */
    class SocketBase
    {
    public:
        virtual ~SocketBase();

        /**
        @brief ソケット作成
        @return 成否
        @param family ... プロトコルファミリ
        @param type ... 通信方式
        @param protocol ... プロトコル
        */
        bool create(s32 family, s32 type, s32 protocol);

        /**
        @brief ソケットが有効か
        @return 有効・無効
        */
        bool valid() const{ return socket_ != INVALID_SOCKET;}

        /**
        @brief
        */
        void close();

        /**
        @brief バイト受信
        @return 成功なら受信したバイト数、失敗なら-1
        */
        inline s32 recv(Char* buff, s32 len, s32 flags);

        inline s32 recvfrom(Char* buff, s32 len, s32 flags, SOCKADDR* from, s32* fromlen);

        /**
        @brief バイト送信
        @return 成功なら送信したバイト数、失敗なら-1
        */
        inline s32 send(const Char* buff, s32 len, s32 flags);

        inline s32 sendto(const Char* buff, s32 len, s32 flags, const SOCKADDR* to, s32 tolen);

        /**
        @brief 指定方向の接続を閉じる
        @return 成否
        @param 閉じる接続方向
        */
        bool shutdown(Shut how);

    protected:
        SocketBase();

        /**
        @param family ... プロトコルファミリ
        @param type ... 通信方式
        @param protocol ... プロトコル
        @param socket ... ソケットハンドル
        */
        SocketBase(s32 family, s32 type, s32 protocol, SOCKET socket);

        /**
        @brief 
        @return 成否
        */
        bool connect(const addrinfo& info);

        /**
        @brief 
        @return 成否
        */
        bool connect(const SOCKADDR& info);

        /**
        @brief ソケットとアドレス情報を結びつける
        @return 成否
        */
        bool bind(const addrinfo& info);

        /**
        @brief ソケットとアドレス情報を結びつける
        @return 成否
        */
        bool bind(const SOCKADDR& info);

        /**
        @brief 接続要求待ち受け
        @return 成否
        @param backlog ... 要求キューのサイズ
        */
        bool listen(s32 backlog);

        /**
        @brief 接続要求をひとつ取り出し、接続確立しソケット作成
        @return 成否
        @param socket ... 出力。接続確立したソケット
        @param addr ... 出力。接続先情報
        @param addrlen ... 接続先情報サイズ
        */
        bool accept(SocketClient& socket, SOCKADDR* addr, s32* addrlen);


        /// スワップ
        void swap(SocketBase& rhs);

        s32 family_; /// プロトコルファミリ
        s32 type_; /// 通信方式
        s32 protocol_; /// プロトコル
        SOCKET socket_; /// ソケットハンドル

    private:
        SocketBase(const SocketBase&);
        SocketBase& operator=(const SocketBase&);
    };

    // バイト受信
    inline s32 SocketBase::recv(Char* buff, s32 len, s32 flags)
    {
        LASSERT(socket_ != INVALID_SOCKET);
        LASSERT(buff != NULL);
        LASSERT(len > 0);

        return ::recv(socket_, buff, len, flags);
    }

    // バイト受信
    inline s32 SocketBase::recvfrom(Char* buff, s32 len, s32 flags, SOCKADDR* from, s32* fromlen)
    {
        LASSERT(socket_ != INVALID_SOCKET);
        LASSERT(buff != NULL);
        LASSERT(len > 0);

        return ::recvfrom(socket_, buff, len, flags, from, fromlen);
    }

    // バイト送信
    inline s32 SocketBase::send(const Char* buff, s32 len, s32 flags)
    {
        LASSERT(socket_ != INVALID_SOCKET);
        LASSERT(buff != NULL);
        LASSERT(len > 0);

        return ::send(socket_, buff, len, flags);
    }

    // バイト送信
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
    @brief クライアントソケット
    */
    class SocketClient : public SocketBase
    {
    public:
        SocketClient()
        {}

        /**
        @param family ... プロトコルファミリ
        @param type ... 通信方式
        @param protocol ... プロトコル
        @param socket ... ソケットハンドル
        */
        SocketClient(s32 family, s32 type, s32 protocol, SOCKET socket)
            :SocketBase(family, type, protocol, socket)
        {}

        virtual ~SocketClient()
        {}

        /**
        @brief 
        @return 成否
        */
        inline bool connect(const addrinfo& info)
        {
            return SocketBase::connect(info);
        }

        /**
        @brief 
        @return 成否
        */
        inline bool connect(const SOCKADDR& info)
        {
            return SocketBase::connect(info);
        }

        /// スワップ
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
    @brief サーバソケット
    */
    class SocketServer : public SocketBase
    {
    public:
        SocketServer()
        {}

        /**
        @param family ... プロトコルファミリ
        @param type ... 通信方式
        @param protocol ... プロトコル
        @param socket ... ソケットハンドル
        */
        SocketServer(s32 family, s32 type, s32 protocol, SOCKET socket)
            :SocketBase(family, type, protocol, socket)
        {}

        virtual ~SocketServer()
        {}

        /**
        @brief ソケットとアドレス情報を結びつける
        @return 成否
        */
        inline bool bind(const addrinfo& info)
        {
            return SocketBase::bind(info);
        }

        /**
        @brief ソケットとアドレス情報を結びつける
        @return 成否
        */
        inline bool bind(const SOCKADDR& info)
        {
            return SocketBase::bind(info);
        }

        /**
        @brief 接続要求待ち受け
        @return 成否
        @param backlog ... 要求キューのサイズ
        */
        inline bool listen(s32 backlog)
        {
            return SocketBase::listen(backlog);
        }

        /**
        @brief 接続要求をひとつ取り出し、接続確立しソケット作成
        @return 成否
        @param socket ... 出力。接続確立したソケット
        @param addr ... 出力。接続先情報
        @param addrlen ... 接続先情報サイズ
        */
        inline bool accept(SocketClient& socket, SOCKADDR* addr, s32* addrlen)
        {
            return SocketBase::accept(socket, addr, addrlen);
        }

        /// スワップ
        inline void swap(SocketServer& rhs)
        {
            return SocketBase::swap(rhs);
        }
    };
}

#endif //INC_LNET_SOCKET_H__
