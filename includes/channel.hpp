/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 14:43:33 by smun              #+#    #+#             */
/*   Updated: 2022/03/28 20:29:49 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include "shared_ptr.hpp"

class Context;
class Session;

class Channel
{
private:
    typedef     std::map<int, SharedPtr<Session> >             SessionMap;
    typedef     std::map<int, SharedPtr<Session> >::iterator   SessionMapIterator;

    const int   _listenPort;

    int         _eventfd;
    Context*    _listenContext;
    bool        _killed;

    SessionMap  _sessions;

    Channel();
    Channel(const Channel&);
    Channel&    operator=(const Channel&);

    void    Accept();
    void    Read(Session* session);
    void    Write(Session* session);
    void    Close(Session* session);

    void    ReleaseSocket(int fd);

public:
    Channel(int port);
    virtual ~Channel();

    void    Init();
    void    BindAndListen();

    void    Run();
    void    SetEvent(int fd, int events, int flags, Context* context);

    static void SetNonBlock(int socketfd);
};

#endif
