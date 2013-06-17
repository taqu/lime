/**
@file Socket.cpp
@author t-sakai
@date 2011/08/06
*/
#include "Socket.h"

#include <memory.h>
#include <iostream>

namespace lcore
{
    //----------------------------------------------------
    //---
    //--- SocketUtil
    //---
    //----------------------------------------------------
    u64 SocketUtil::getIP(Family family, const Char* ipInString)
    {
        LASSERT(NULL != ipInString);

        u64 ip;
        s32 ret = inet_pton(family, ipInString, &(ip));
        return (0<ret)? ip : 0;
    }

    bool SocketUtil::getIPString(Family family, Char* dst, u32 size, u64 ip)
    {
        const Char* ret = inet_ntop(family, &ip, dst, size);
        return (NULL != ret);
    }

    u16 SocketUtil::htons(u16 port)
    {
        return ::htons(port);
    }

    u16 SocketUtil::ntohs(u16 port)
    {
        return ::ntohs(port);
    }

    s32 SocketUtil::select(s32 nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const timeval* timeout)
    {
        return ::select(nfds, readfds, writefds, exceptfds, timeout);
    }

    //----------------------------------------------------
    //---
    //--- SocketSystem
    //---
    //----------------------------------------------------
    // インスタンス
    SocketSystem SocketSystem::instance_;

    // 初期化
    bool SocketSystem::initialize()
    {
        s32 ret = WSAStartup(MAKEWORD(2,2), &instance_.wsaData_);
        return ret == 0;
    }

    // 終了
    void SocketSystem::terminate()
    {
        WSACleanup();
    }

    // エラーコード取得
    s32 SocketSystem::getError()
    {
        return WSAGetLastError();
    }

    void SocketSystem::printError(std::ostream& os, const Char* message)
    {
        if(message != NULL){
            os << message;
        }
        os << "(code " << getError() << ')' << std::endl;
    }

    //----------------------------------------------------
    //---
    //--- FDSet
    //---
    //----------------------------------------------------
    void FDSet::clear(SocketBase& socket)
    {
        FD_CLR(socket.socket_, &set_);
    }

    s32 FDSet::isSet(SocketBase& socket)
    {
        return FD_ISSET(socket.socket_, &set_);
    }

    void FDSet::set(SocketBase& socket)
    {
        FD_SET(socket.socket_, &set_);
    }

    void FDSet::zero()
    {
        FD_ZERO(&set_);
    }

    //----------------------------------------------------
    //---
    //--- AddrInfo
    //---
    //----------------------------------------------------
    bool AddrInfo::get(
            const Char* node,
            const Char* service,
            Family family,
            SocketType type,
            Protocol protocol,
            s32 flag)
    {
        release();

        addrinfo hints;
        memset(&hints, 0, sizeof(addrinfo));
        hints.ai_family = family;
        hints.ai_socktype = type;
        hints.ai_protocol = protocol;
        hints.ai_flags = flag;

        s32 result = getaddrinfo(node, service, &hints, &addrInfo_);
        return (result == 0);
    }

    void AddrInfo::release()
    {
        if(addrInfo_ != NULL){
            freeaddrinfo(addrInfo_);
            addrInfo_ = NULL;
        }
    }


    //----------------------------------------------------
    //---
    //--- SocketBase
    //---
    //----------------------------------------------------
    SocketBase::SocketBase()
        :family_(Family_INET)
        ,type_(SocketType_STREAM)
        ,protocol_(Protocol_TCP)
        ,socket_(INVALID_SOCKET)
    {
    }

    SocketBase::SocketBase(Family family, SocketType type, Protocol protocol, SOCKET socket)
        :family_(family)
        ,type_(type)
        ,protocol_(protocol)
        ,socket_(socket)
    {
    }

    SocketBase::~SocketBase()
    {
        close();
    }

    bool SocketBase::create(Family family, SocketType type, Protocol protocol)
    {
        SOCKET socket = ::socket(family, type, protocol);
        if(socket != INVALID_SOCKET){
            close();
            family_ = family;
            type_ = type;
            protocol_ = protocol;
            socket_ = socket;
        }
        return (socket != INVALID_SOCKET);
    }

    s32 SocketBase::setOption(SocketOption name, const Char* val, s32 length)
    {
        return ::setsockopt(socket_, SOL_SOCKET, name, val, length);
    }

    bool SocketBase::setNonBlock()
    {
        LASSERT(socket_ != INVALID_SOCKET);
#ifdef _WIN32
        u_long val = 1;
        return SOCKET_ERROR != ioctlsocket(socket_, FIONBIO, &val);
#else
        if(fcntl(socket_, F_SETFL, O_NONBLOCK) < 0){
            return false;
        }
        return true;
#endif
    }

    void SocketBase::close()
    {
        if(socket_ != INVALID_SOCKET){
            closesocket(socket_);
            socket_ = INVALID_SOCKET;
        }
    }

    s32 SocketBase::shutdown(Shut how)
    {
        LASSERT(socket_ != INVALID_SOCKET);
        return ::shutdown(socket_, how);
    }

    s32 SocketBase::connect(const addrinfo& info)
    {
        LASSERT(socket_ != INVALID_SOCKET);
        return ::connect(socket_, info.ai_addr, static_cast<int>(info.ai_addrlen));
    }

    s32 SocketBase::connect(const SOCKADDR& info)
    {
        LASSERT(socket_ != INVALID_SOCKET);
        return ::connect(socket_, &info, static_cast<int>(sizeof(SOCKADDR)));
    }

    s32 SocketBase::bind(const SOCKADDR& info)
    {
        LASSERT(socket_ != INVALID_SOCKET);
        return ::bind(socket_, &info, static_cast<int>(sizeof(SOCKADDR)));
    }

    s32 SocketBase::bind(u16 port, u64 acceptIP)
    {
        LASSERT(socket_ != INVALID_SOCKET);
        sockaddr_in info;
        info.sin_family = static_cast<ADDRESS_FAMILY>(family_);
        info.sin_port = htons(port);
        info.sin_addr.S_un.S_addr = static_cast<ULONG>(acceptIP);

        return ::bind(socket_, (const sockaddr*)&info, static_cast<int>(sizeof(sockaddr_in)));
    }

    s32 SocketBase::listen(s32 backlog)
    {
        return ::listen(socket_, backlog);
    }

    bool SocketBase::accept(SocketClient& socket, SOCKADDR* addr, s32* addrlen)
    {
        LASSERT(socket_ != INVALID_SOCKET);

        SOCKET ret = ::accept(socket_, addr, addrlen);

        if(ret != INVALID_SOCKET){
            SocketClient tmp(family_, type_, protocol_, ret);
            socket.swap(tmp);
            return true;
        }
        return false;
    }

    // スワップ
    void SocketBase::swap(SocketBase& rhs)
    {
        std::swap(family_, rhs.family_);
        std::swap(type_, rhs.type_);
        std::swap(protocol_, rhs.protocol_);
        std::swap(socket_, rhs.socket_);
    }
}
