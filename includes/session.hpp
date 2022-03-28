/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   session.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 15:26:59 by smun              #+#    #+#             */
/*   Updated: 2022/03/28 20:40:30 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SESSION_HPP
#define SESSION_HPP

#include "std.hpp"
#include "context.hpp"
#include <vector>
#include <string>

class Session : public Context
{
private:
    typedef std::vector<Byte>             ByteBuffer;
    typedef std::vector<Byte>::iterator   ByteBufferIterator;

    const int           _socketId;
    const std::string   _remoteAddress;
    ByteBuffer          _recvBuffer;
    ByteBuffer          _sendBuffer;

    bool                _sendTriggered;

    Session();
    Session(const Session&);
    Session& operator= (const Session&);

    void    AppendBuffer(Byte* buffer, size_t bytes);
    void    TakeBuffer(size_t bytes);

    void    Process(const std::string& line);
    void    Send(const std::string& line);
    void    Send(const void* buf, size_t len);

    static bool    GetNextLine(ByteBuffer& buffer, std::string& line);

public:
    Session(Channel* channel, int socketfd, int socketId, const std::string& addr);

    virtual ~Session();

    void    OnRead();
    void    OnWrite();

    void    TriggerClose();

    int     GetSocketId() const;
    const std::string&  GetRemoteAddress() const;
};

#endif
