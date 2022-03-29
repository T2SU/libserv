/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 15:38:55 by smun              #+#    #+#             */
/*   Updated: 2022/03/29 13:07:20 by smun             ###   ########.fr       */
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
    static void Printfp(int level, const char* prefix, const char* format, va_list& va);

public:
    static void V(const char* format, ...);
    static void D(const char* format, ...);
    static void I(const char* format, ...);
    static void W(const char* format, ...);
    static void E(const char* format, ...);
    static void F(const char* format, ...);

    static void Vp(const char* prefix, const char* format, ...);
    static void Dp(const char* prefix, const char* format, ...);
    static void Ip(const char* prefix, const char* format, ...);
    static void Wp(const char* prefix, const char* format, ...);
    static void Ep(const char* prefix, const char* format, ...);
    static void Fp(const char* prefix, const char* format, ...);
};

#endif
