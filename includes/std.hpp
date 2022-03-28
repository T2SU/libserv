/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   std.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 12:25:54 by smun              #+#    #+#             */
/*   Updated: 2022/03/28 16:42:52 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STD_HPP
#define STD_HPP

#include <cstddef>
#include <iosfwd>

// data buffer
#define TCP_MTU (1500 - 20 - 20)

// line sperator (http/irc)
#define LINE_SEPARATOR "\r\n"

enum { IOEvent_Read = 1 << 0, IOEvent_Write = 1 << 1, IOEvent_Custom = 1 << 2 };
enum { IOFlag_Add = 1 << 0, IOFlag_Remove = 1 << 1, IOFlag_Enable = 1 << 2, IOFlag_Disable = 1 << 3 };

typedef unsigned char	Byte;

#endif
