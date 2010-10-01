//----------------------------------------------------------------------------
/// \file  error.hpp
//----------------------------------------------------------------------------
/// \brief Definition of exception classes.
//----------------------------------------------------------------------------
// Copyright (c) 2010 Serge Aleynikov <saleyn@gmail.com>
// Created: 2010-09-30
//----------------------------------------------------------------------------
/*
***** BEGIN LICENSE BLOCK *****

This file is part of the REPLOG project.

Copyright (C) 2010 Serge Aleynikov <saleyn@gmail.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

***** END LICENSE BLOCK *****
*/
#ifndef _REPLOG_ERROR_HPP_
#define _REPLOG_ERROR_HPP_

#include <exception>
#include <sstream>
#include <stdio.h>

namespace replog {

/**
 * \brief Exception class for I/O related errors.
 */
class io_error : public std::exception {
    std::string m_msg;

    template <class T>
    void init(const char* a_str, T a_arg) {
        std::stringstream s; s << a_str << ' ' << a_arg;
        m_msg = s.str();
    }
    template <class T, class U>
    void init(const char* a_str, T a1, const char* a_delim, U a2, const char* a_suff) {
        std::stringstream s; s << a_str << a1 << a_delim << a2 << a_suff;
        m_msg = s.str();
    }
public:
    io_error(const char* a_str) : m_msg(a_str) {}

    io_error(int a_errno) {
        char buf[128];
        m_msg = strerror_r(a_errno, buf, sizeof(buf));
    }

    io_error(int a_errno, const char* a_prefix) {
        char buf[128], buf2[128];
        int n = snprintf(buf, sizeof(buf), "%s:", a_prefix);
        init(buf, strerror_r(a_errno, buf2, sizeof(buf2)));
    }

    template <class T>
    io_error(const char* a_str, T a_arg) { init(a_str, a_arg); }

    /// Use for reporting errors in the form:
    ///    io_error("Bad data! (expected=", 1, ", got=", 3, ")");
    template <class T, class U>
    io_error(const char* a_str, T a1, const char* a_delim, U a2, const char* a_suff="") {
        init(a_str, a1, a_delim, a2, a_suff);
    }

    virtual ~io_error() throw() {}
    const char* what() const throw() { return m_msg.c_str(); }    
};

typedef io_error replog_error;

class replog_resend: std::exception {
    std::string m_filename;
    off_t       m_offset;
public:
    replog_resend(const char* a_filename, off_t a_offset)
        : m_filename(a_filename), m_offset(a_offset)
    {}
    virtual ~replog_resend() throw() {}

    const std::string&  filename()  const { return m_filename; }
    off_t               offset()    const { return m_offset; }
    const char* what() const throw() {
        std::stringstream s; s << "Resend file " << m_filename << " at " << m_offset;
        return s.str().c_str();
    }
};

} // namespace replog

#endif // _REPLOG_ERROR_HPP_

