/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 14:49:17 by smun              #+#    #+#             */
/*   Updated: 2022/03/29 12:28:14 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "std.hpp"
#include "channel.hpp"
#include "session.hpp"
#include "context.hpp"
#include "log.hpp"
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <string>
#include <sys/event.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <cerrno>

Channel::Channel(int port)
    : _listenPort(port)
    , _eventfd(-1)
    , _listenContext(NULL)
    , _killed(false)
    , _sessions()
{
}

Channel::~Channel()
{
    delete _listenContext;
    close(_eventfd);
    Log::V("Deleted channel instance");
}

void    Channel::Init()
{
    _eventfd = kqueue();
    if (_eventfd < 0)
        throw std::runtime_error("failed kqueue()");
    Log::V("Created kqueue fd (%d)", _eventfd);
}

void    Channel::BindAndListen()
{
    // 1. 소켓 생성
    int listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenfd < 0)
        throw std::runtime_error("failed socket() for binding port");
    int enable = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        throw std::runtime_error("failed setsockopt(SO_REUSEADDR, 1) for binding port");
    Log::V("Created listen sockket fd. (%d)", listenfd);

    // 2. 포트 바인딩을 위해 sockaddr 구조체 설정.
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = PF_INET;
    addr.sin_port = htons(_listenPort);
    addr.sin_addr.s_addr = INADDR_ANY;
    Log::V("Defined sockaddr for binding port. (Port %d)", _listenPort);

    // 3. 포트 바인딩
    sockaddr* paddr = reinterpret_cast<sockaddr*>(&addr);
    int bindres = bind(listenfd, paddr, sizeof(*paddr));
    if (bindres < 0)
        throw std::runtime_error("failed bind()");
    Log::D("Bound port for listen. (Port %d)", _listenPort);

    // 4. 연결을 수락하기 시작. (backlog: 20)
    int listenres = ::listen(listenfd, 20);
    if (listenres < 0)
        throw std::runtime_error("failed listen()");
    _listenContext = new Context(this, listenfd);
    Log::I("Listening on port %d.", _listenPort);

    // 5. 연결을 수락하기 위해, kqueue에 read 이벤트 등록.
    SetEvent(listenfd, IOEvent_Read, IOFlag_Add | IOFlag_Enable, _listenContext);
}

void    Channel::SetEvent(int fd, int events, int flags, Context* context)
{
    kevent64_s  ev[2];
    int         eventNum = 0;
    int         eventFlags = 0;

    // 내가 만든 IOFlag 플래그를 실제 kqueue에서 사용되는 플래그로 변환.
    if (flags & IOFlag_Add)
        eventFlags |= EV_ADD;
    if (flags & IOFlag_Remove)
        eventFlags |= EV_DELETE;
    if (flags & IOFlag_Enable)
        eventFlags |= EV_ENABLE;
    if (flags & IOFlag_Disable)
        eventFlags |= EV_DISABLE;

    // kqueue64를 위해 이벤트 등록 또는 삭제 구조체를 설정.
    if (events & IOEvent_Read)
         EV_SET64(&ev[eventNum++], fd, EVFILT_READ, eventFlags, 0, 0, reinterpret_cast<uint64_t>(context), 0, 0);
    if (events & IOEvent_Write)
         EV_SET64(&ev[eventNum++], fd, EVFILT_WRITE, eventFlags, 0, 0, reinterpret_cast<uint64_t>(context), 0, 0);

    // 실제 kqueue에 이벤트를 등록 또는 삭제 요청.
    int evregist = kevent64(_eventfd, ev, eventNum, NULL, 0, 0, NULL);
    if (evregist < 0)
    {
        int err = errno; (void)err;
        throw std::runtime_error("kevent64() failed");
    }
}

void    Channel::Accept()
{
    static int socketIdCounter = 0;
    sockaddr_in remoteaddr;
    socklen_t remoteaddr_size = sizeof(remoteaddr);

    // Accept 큐에 쌓여 있는, 클라이언트 접속 요청 하나를 accept 함수를 통해 가져옴.
    int clientfd = accept(_listenContext->GetSocket(), reinterpret_cast<sockaddr*>(&remoteaddr), &remoteaddr_size);
    if (clientfd < 0)
    {
        Log::E("accept() failed.");
        return;
    }

    try
    {
        // 새로운 세션 인스턴스 생성.
        int socketId = ++socketIdCounter;
        const std::string& addr = inet_ntoa(remoteaddr.sin_addr);
        SharedPtr<Session> session = SharedPtr<Session>(new Session(this, clientfd, socketId, addr));
        _sessions[socketId] = session;
        Log::I("Incoming a connection from %s", addr.c_str());
        SetNonBlock(clientfd);
        SetEvent(clientfd, IOEvent_Read, IOFlag_Add | IOFlag_Enable, session.Load());
    }
    catch (std::exception& ex)
    {
        // 세션 인스턴스 생성 및 등록에 실패할 경우, 에러 출력.
        Log::F("Failed to create new client session. (%s)", ex.what());
    }
}

void    Channel::Read(Session* session)
{
    if (session == NULL) throw std::runtime_error("Channel::Read not called as parameter session.");
    Log::V("Channel::Read socket-%d", session->GetSocket());
    session->OnRead();
}

void    Channel::Write(Session* session)
{
    if (session == NULL) throw std::runtime_error("Channel::Write not called as parameter session.");
    Log::V("Channel::Write socket-%d", session->GetSocket());
    session->OnWrite();
}

void    Channel::Close(Session* session)
{
    if (session == NULL) throw std::runtime_error("Channel::Close not called as parameter session.");
    Log::V("Channel::Close socket-%d", session->GetSocket());
    _sessions.erase(session->GetSocketId());
}

void    Channel::Run()
{
    // 한 번에 최대로 처리할 수 있는 이벤트 개수.
    const int MaxEvents = 20;
    kevent64_s events[MaxEvents];

    // 서버가 살아있을 동안 계속 반복.
    while (_killed == false)
    {
        // kqueue에서, 쌓여 있을 IO가능(ready) 이벤트를 모두 가져옴.
        int numbers = kevent64(_eventfd, NULL, 0, events, MaxEvents, 0, NULL);
        for (int i = 0; i < numbers; i++)
        {
            kevent64_s& event = events[i];
            int filter = event.filter;
            Context* context = reinterpret_cast<Context*>(event.udata);

            // 처리 시작.
            try
            {
                // 연결 수락
                if (filter == EVFILT_READ && _listenContext == context)
                    Accept();

                // 데이터 읽기
                else if (filter == EVFILT_READ)
                    Read(dynamic_cast<Session*>(context));

                // 데이터 쓰기
                else if (filter == EVFILT_WRITE)
                    Write(dynamic_cast<Session*>(context));
            }

            // 예외 처리
            catch (const std::exception& ex)
            {
                Log::F("Socket event error was occurred: [%s]", ex.what());
            }
        }
    }
}

void    Channel::SetNonBlock(int socketfd)
{
    int setfcntlres = fcntl(socketfd, F_SETFL, O_NONBLOCK);
    if (setfcntlres < 0)
        throw std::runtime_error("failed fcntl() for set nonblocking flag");
}
