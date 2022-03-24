/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 15:38:55 by smun              #+#    #+#             */
/*   Updated: 2022/03/24 20:57:46 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOG_HPP
#define LOG_HPP

#include <iosfwd>
#include <string> // for convenience
#include <cstdarg>

class Log
{
private:
    enum
    {
        LogLevel_Verbose,
        LogLevel_Debug,
        LogLevel_Info,
        LogLevel_Warning,
        LogLevel_Error,
        LogLevel_Fatal
    };

    Log();
    ~Log();
    Log(const Log&);
    Log& operator=(const Log&);

    static const char* GetPrefix(int level);
    static void Printf(int level, const char* format, va_list& va);

public:
    static void V(const char* format, ...);
    static void D(const char* format, ...);
    static void I(const char* format, ...);
    static void W(const char* format, ...);
    static void E(const char* format, ...);
    static void F(const char* format, ...);
};

#endif
