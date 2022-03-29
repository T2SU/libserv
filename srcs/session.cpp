/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 15:32:11 by smun              #+#    #+#             */
/*   Updated: 2022/03/29 12:43:37 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "std.hpp"
#include "channel.hpp"
#include "session.hpp"
#include "log.hpp"
#include <algorithm>
#include <sys/socket.h>
#include <cstring>
#include <string>
#include <cerrno>
#include <memory>

Session::Session(Channel* channel, int socketfd, int socketId, const std::string& addr)
    : Context(channel, socketfd)
    , _socketId(socketId)
    , _remoteAddress(addr)
    , _recvBuffer()
    , _sendBuffer()
    , _triggeredEvents(0)
{
    Log::V("Created session instance. (socket-%d/%s)", GetSocket(), GetRemoteAddress().c_str());
}

Session::Session(const Session& s)
    : Context(s)
    , _socketId(s._socketId)
    , _remoteAddress(s._remoteAddress)
    , _recvBuffer(s._recvBuffer)
    , _sendBuffer(s._sendBuffer)
    , _triggeredEvents(0)
{
    Log::V("Copied session instance. (socket-%d/%s)", GetSocket(), GetRemoteAddress().c_str());
}

Session::~Session()
{
    if (_triggeredEvents)
        _attachedChannel->SetEvent(GetSocket(), _triggeredEvents, IOFlag_Remove, NULL);
    _triggeredEvents &= ~_triggeredEvents;
    Log::V("Deleted session instance. (socket-%d/%s)", GetSocket(), GetRemoteAddress().c_str());
}

bool    Session::GetNextLine(ByteBuffer& buffer, std::string& line)
{
    const size_t lineSepLen = std::strlen(LINE_SEPARATOR);
    const ByteBufferIterator begin = buffer.begin();
    const ByteBufferIterator end = buffer.end();
    ByteBufferIterator lineEnd;

    lineEnd = std::search(begin, end, &LINE_SEPARATOR[0], &LINE_SEPARATOR[sizeof(LINE_SEPARATOR) - 1]);
    if (lineEnd != end)
    {
        line.assign(begin, lineEnd);
        buffer.erase(begin, lineEnd + lineSepLen);
        return true;
    }
    return false;
}

void    Session::OnRead()
{
    Byte buffer[TCP_MTU];

    ssize_t bytes = recv(GetSocket(), buffer, sizeof(buffer), 0);
    if (bytes <= 0)
    {
        if (bytes < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            return;
        Log::D("Socket closed from Session::OnRead errno:%d (socket-%d/%s)", errno, GetSocket(), GetRemoteAddress().c_str());
        Close();
        return;
    }
    AppendBuffer(buffer, static_cast<size_t>(bytes));

    std::string line;
    while (GetNextLine(_recvBuffer, line))
        Process(line);
}

void    Session::AppendBuffer(Byte* buffer, size_t bytes)
{
    _recvBuffer.reserve(_recvBuffer.size() + bytes);
    _recvBuffer.insert(_recvBuffer.end(), &buffer[0], &buffer[bytes]);
    Log::V("Appended %llu bytes into buffer. (current: %llu bytes) (socket-%d/%s)", bytes, _recvBuffer.size(), GetSocket(), GetRemoteAddress().c_str());
}

void    Session::OnWrite()
{
    ssize_t bytes = send(GetSocket(), _sendBuffer.data(), _sendBuffer.size(), 0);
    if (bytes < 0)
    {
        if (bytes < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            return;
        Log::D("Socket closed from Session::OnWrite errno:%d (socket-%d/%s)", errno, GetSocket(), GetRemoteAddress().c_str());
        Close();
        return;
    }
    TakeBuffer(static_cast<size_t>(bytes));
    if (_sendBuffer.empty())
    {
        Log::V("Send buffer is empty. disable send io flag (socket-%d/%s)", GetSocket(), GetRemoteAddress().c_str());
        _attachedChannel->SetEvent(GetSocket(), IOEvent_Write, IOFlag_Disable, this);
        _triggeredEvents &= ~IOEvent_Write;
    }
}

void    Session::TakeBuffer(size_t bytes)
{
    const ByteBufferIterator begin  = _sendBuffer.begin();
    const ByteBufferIterator end    = _sendBuffer.begin() + bytes;

    _sendBuffer.erase(begin, end);
    Log::V("Taken %llu bytes from buffer. (socket-%d/%s)", bytes, GetSocket(), GetRemoteAddress().c_str());
}

void    Session::Close()
{
    _attachedChannel->Close(this);
}

void    Session::Process(const std::string& line)
{
    Log::I("[R/%s] %s", _remoteAddress.c_str(), line.c_str());
}

void    Session::Send(const std::string& line)
{
    Log::I("[S/%s] %s", _remoteAddress.c_str(), line.c_str());
    Send(&line[0], line.length());
}

void    Session::Send(const void* buf, size_t len)
{
    const Byte* const bytebuf = reinterpret_cast<const Byte*>(buf);

    _sendBuffer.insert(_sendBuffer.end(), bytebuf, bytebuf + len);
    if (!(_triggeredEvents & IOEvent_Write))
    {
        _triggeredEvents |= IOEvent_Write;
        _attachedChannel->SetEvent(GetSocket(), IOEvent_Write, IOFlag_Enable, this);
    }
}

const std::string&  Session::GetRemoteAddress() const
{
    return _remoteAddress;
}

int Session::GetSocketId() const
{
    return _socketId;
}
