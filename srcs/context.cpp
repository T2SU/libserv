/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 15:18:07 by smun              #+#    #+#             */
/*   Updated: 2022/03/28 17:10:30 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "std.hpp"
#include "context.hpp"
#include "channel.hpp"
#include "log.hpp"
#include <unistd.h>

Context::Context(Channel* channel, int fd)
    : _socketfd(fd)
    , _attachedChannel(channel)
{
}

Context::Context(const Context& ctx)
    : _socketfd(ctx._socketfd)
    , _attachedChannel(ctx._attachedChannel)
{
}

Context::~Context()
{
    close(_socketfd);
    Log::V("Deleted context instance. (fd: %d)", _socketfd);
}

int Context::GetSocket() const
{
    return _socketfd;
}
