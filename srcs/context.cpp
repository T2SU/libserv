/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 15:18:07 by smun              #+#    #+#             */
/*   Updated: 2022/03/24 16:43:46 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "std.hpp"
#include "context.hpp"
#include "log.hpp"
#include <unistd.h>

Context::~Context()
{
    close(_socketfd);
    Log::V("Deleted context instance. (fd: %d)", _socketfd);
}

Context::Context(int fd)
    : _socketfd(fd)
{
}

Context::Context(const Context& ctx)
    : _socketfd(ctx._socketfd)
{
}

int Context::GetSocket() const
{
    return _socketfd;
}
