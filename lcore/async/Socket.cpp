/**
@file Socket.cpp
@author t-sakai
@date 2011/08/06
*/
#include "Socket.h"

#include <memory.h>
#include <iostream>

namespace lnet
{
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
        :family_(0)
        ,type_(0)
        ,protocol_(0)
        ,socket_(INVALID_SOCKET)
    {
    }

    SocketBase::SocketBase(s32 family, s32 type, s32 protocol, SOCKET socket)
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

    bool SocketBase::create(s32 family, s32 type, s32 protocol)
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

    void SocketBase::close()
    {
        if(socket_ != INVALID_SOCKET){
            closesocket(socket_);
            family_ = 0;
            type_ = 0;
            protocol_ = 0;
            socket_ = INVALID_SOCKET;
        }
    }

    bool SocketBase::shutdown(Shut how)
    {
        LASSERT(socket_ != INVALID_SOCKET);
        return (SOCKET_ERROR != ::shutdown(socket_, how));
    }


    bool SocketBase::connect(const addrinfo& info)
    {
        LASSERT(socket_ != INVALID_SOCKET);
        return (SOCKET_ERROR != ::connect(socket_, info.ai_addr, static_cast<int>(info.ai_addrlen)) );
    }

    bool SocketBase::connect(const SOCKADDR& info)
    {
        LASSERT(socket_ != INVALID_SOCKET);
        return (SOCKET_ERROR != ::connect(socket_, &info, static_cast<int>(sizeof(SOCKADDR))) );
    }

    bool SocketBase::bind(const addrinfo& info)
    {
        LASSERT(socket_ != INVALID_SOCKET);
        return (SOCKET_ERROR != ::bind(socket_, info.ai_addr, static_cast<int>(info.ai_addrlen)) );
    }

    bool SocketBase::bind(const SOCKADDR& info)
    {
        LASSERT(socket_ != INVALID_SOCKET);
        return (SOCKET_ERROR != ::bind(socket_, &info, static_cast<int>(sizeof(SOCKADDR))) );
    }

    bool SocketBase::listen(s32 backlog)
    {
        return (SOCKET_ERROR != ::listen(socket_, backlog));
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
