/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 15:32:11 by smun              #+#    #+#             */
/*   Updated: 2022/03/24 16:45:21 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "session.hpp"
#include "context.hpp"

Session::Session(int socketId, Context* context)
    : _socketId(socketId)
    , _context(context)
{
}

Session::~Session()
{
    delete _context;
    Log::V("Deleted session instance. (socketId: %d)", _socketId);
}
