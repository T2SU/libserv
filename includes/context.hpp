/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 15:09:45 by smun              #+#    #+#             */
/*   Updated: 2022/03/24 15:19:16 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

class Context
{
private:
    const int   _socketfd;

    Context();
    Context& operator= (const Context&);

public:
    virtual ~Context();
    Context(int fd);
    Context(const Context& ctx);

    int     GetSocket() const;
};

#endif
