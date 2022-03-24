/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 14:43:33 by smun              #+#    #+#             */
/*   Updated: 2022/03/24 20:50:09 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>

class Context;

class Channel
{
private:
    const int   _listenPort;

    int         _eventfd;
    Context*    _listenContext;
    bool        _killed;

    Channel();
    Channel(const Channel&);
    Channel&    operator=(const Channel&);

    void    Init();
    void    BindAndListen();

    void    Accept();
    void    Read(Context* context);
    void    Write(Context* context);


public:
    Channel(int port);
    virtual ~Channel();

    void    Run();
    void    SetEvent(int fd, int events, int flags, Context* context);

    static void SetNonBlock(int socketfd);
};

#endif
