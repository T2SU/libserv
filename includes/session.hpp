/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   session.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 15:26:59 by smun              #+#    #+#             */
/*   Updated: 2022/03/24 15:33:35 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SESSION_HPP
#define SESSION_HPP

class Context;

class Session
{
private:
    const int   _socketId;
    Context*    _context;

    Session();
    Session(const Session&);
    Session& operator= (const Session&);

public:
    Session(int socketId, Context* context);
    virtual ~Session();

};

#endif
