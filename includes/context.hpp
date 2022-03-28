/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 15:09:45 by smun              #+#    #+#             */
/*   Updated: 2022/03/28 17:10:14 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <vector>

class Channel;

class Context
{
private:
    const int       _socketfd;

    Context();
    Context& operator= (const Context&);

protected:
    Channel* const  _attachedChannel;

public:
    virtual ~Context();
    Context(Channel* channel, int fd);
    Context(const Context& ctx);

    int     GetSocket() const;
};

#endif
