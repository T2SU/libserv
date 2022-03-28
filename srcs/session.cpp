/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 15:32:11 by smun              #+#    #+#             */
/*   Updated: 2022/03/28 21:26:55 by smun             ###   ########.fr       */
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
    , _sendTriggered(false)
{
    Log::V("Created session instance. (socket-%d/%s)", GetSocket(), GetRemoteAddress().c_str());
}

Session::Session(const Session& s)
    : Context(s)
    , _socketId(s._socketId)
    , _remoteAddress(s._remoteAddress)
    , _recvBuffer(s._recvBuffer)
    , _sendBuffer(s._sendBuffer)
    , _sendTriggered(s._sendTriggered)
{
    Log::V("Copied session instance. (socket-%d/%s)", GetSocket(), GetRemoteAddress().c_str());
}

Session::~Session()
{
    _attachedChannel->SetEvent(GetSocket(), IOEvent_Read | IOEvent_Write, IOFlag_Remove, NULL);
    Log::V("Deleted session instance. (socket-%d/%s)", GetSocket(), GetRemoteAddress().c_str());
}

bool    Session::GetNextLine(ByteBuffer& buffer, std::string& line)
{
    const ByteBufferIterator begin = buffer.begin();
    const ByteBufferIterator end = buffer.end();
    ByteBufferIterator lineEnd;

    lineEnd = std::search(begin, end, &LINE_SEPARATOR[0], &LINE_SEPARATOR[sizeof(LINE_SEPARATOR) - 1]);
    if (lineEnd != end)
    {
        line.assign(begin, lineEnd);
        buffer.erase(begin, lineEnd);
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
    std::memcpy(&_recvBuffer[_recvBuffer.size()], buffer, bytes);
    Log::V("Appended %llu bytes into buffer. (socket-%d/%s)", bytes, GetSocket(), GetRemoteAddress().c_str());
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
        _sendTriggered = false;
    }
}

void    Session::TakeBuffer(size_t bytes)
{
    const ByteBufferIterator begin = _sendBuffer.begin();
    ByteBufferIterator end = _sendBuffer.begin();

    std::advance(end, bytes);
    _sendBuffer.erase(begin, end);
    Log::V("Taken %llu bytes from buffer. (socket-%d/%s)", bytes, GetSocket(), GetRemoteAddress().c_str());
}

void    Session::Close()
{
    int events = IOEvent_Read;
    if (_sendTriggered)
        events |= IOEvent_Write;
    _attachedChannel->SetEvent(GetSocket(), events, IOFlag_Remove, this);
}

void    Session::Process(const std::string& line)
{
    Log::I("[R/%s] %s", _remoteAddress, line);
}

void    Session::Send(const std::string& line)
{
    Log::I("[S/%s] %s", _remoteAddress, line);
    Send(&line[0], line.length());
}

void    Session::Send(const void* buf, size_t len)
{
    const Byte* const bytebuf = reinterpret_cast<const Byte*>(buf);

    _sendBuffer.insert(_sendBuffer.end(), bytebuf, bytebuf + len);
    if (!_sendTriggered)
    {
        _sendTriggered = true;
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
