/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 14:49:17 by smun              #+#    #+#             */
/*   Updated: 2022/03/24 21:01:53 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "std.hpp"
#include "channel.hpp"
#include "context.hpp"
#include "log.hpp"
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <string>
#include <sys/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

Channel::Channel(int port)
    : _listenPort(port)
    , _eventfd(-1)
    , _listenContext(NULL)
    , _killed(false)
{
}

Channel::~Channel()
{
    delete _listenContext;
    close(_eventfd);
    Log::V("Deleted Channel instance");
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
    _listenContext = new Context(listenfd);
    Log::I("Listening on port %d.", _listenPort);

    // 5. 연결을 수락하기 위해, kqueue에 read 이벤트 등록.
    SetEvent(listenfd, IOEvent_Read, IOFlag_Add | IOFlag_Enable, _listenContext);
}

void    Channel::SetEvent(int fd, int events, int flags, Context* context)
{
    kevent64_s  ev[3];
    int         eventNum = 0;
    int         eventFlags = 0;

    if (flags & IOFlag_Add)
        eventFlags |= EV_ADD;
    if (flags & IOFlag_Remove)
        eventFlags |= EV_DELETE;
    if (flags & IOFlag_Enable)
        eventFlags |= EV_ENABLE;
    if (flags & IOFlag_Disable)
        eventFlags |= EV_DISABLE;

    if (events & IOEvent_Read)
         EV_SET64(&ev[eventNum++], fd, EVFILT_READ, eventFlags, 0, 0, reinterpret_cast<uint64_t>(context), 0, 0);
    if (events & IOEvent_Write)
         EV_SET64(&ev[eventNum++], fd, EVFILT_WRITE, eventFlags, 0, 0, reinterpret_cast<uint64_t>(context), 0, 0);
    if (events & IOEvent_Custom)
         EV_SET64(&ev[eventNum++], fd, EVFILT_USER, eventFlags, 0, 0, reinterpret_cast<uint64_t>(context), 0, 0);

    int evregist = kevent64(_eventfd, ev, eventNum, NULL, 0, 0, NULL);
    if (evregist < 0)
        throw std::runtime_error("kevent64() failed");
}

void    Channel::Accept()
{
    sockaddr_in remoteaddr;
    socklen_t remoteaddr_size = sizeof(remoteaddr);
    int clientfd = accept(_listenContext->GetSocket(), reinterpret_cast<sockaddr*>(&remoteaddr), &remoteaddr_size);
    if (clientfd < 0)
    {
        Log::E("accept() failed.");
        return;
    }

    Context* ctx = NULL;
    try
    {
        ctx = new Context(clientfd);
        Log::I("Incoming a connection from %s", "?.?.?.?");
        SetNonBlock(clientfd);
        SetEvent(clientfd, IOEvent_Read, IOFlag_Add | IOFlag_Enable, ctx);
    }
    catch (std::exception& ex)
    {
        delete ctx;
        Log::F("Failed to create new client context. (%s)", ex.what());
    }
}

void    Channel::Read(Context* context)
{

}

void    Channel::Write(Context* context)
{

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

            // 연결 수락
            if (filter == EVFILT_READ && _listenContext == context)
                Accept();

            // 데이터 읽기
            else if (filter == EVFILT_READ)
                Read(context);

            // 데이터 쓰기
            else if (filter == EVFILT_WRITE)
                Write(context);
        }
    }
}

void    Channel::SetNonBlock(int socketfd)
{
    int setfcntlres = fcntl(socketfd, F_SETFL, O_NONBLOCK);
    if (setfcntlres < 0)
        throw std::runtime_error("failed fcntl() for set nonblocking flag");
}
