//----------------------------------------------------------------------------
/// \file  buffer.hpp
//----------------------------------------------------------------------------
/// \brief This file contains implementation of a buffer class used for 
/// storing data for I/O operations.
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
#ifndef _REPLOG_BUFFER_HPP_
#define _REPLOG_BUFFER_HPP_

#include <boost/static_assert.hpp>
#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>
#include <algorithm>
#include <string.h>
#include <replog/error.hpp>

namespace replog {

/**
 * \brief Basic buffer providing stack space for data up to N bytes and
 *        heap space when reallocation of space over N bytes is needed.
 * A typical use of the buffer is for I/O operations that require tracking
 * of produced and consumed space. 
 */
template <int N>
class basic_io_buffer: boost::noncopyable {
    char  m_data[N];
    char* m_begin;
    char* m_end;
    char* m_rd_ptr;
    char* m_wr_ptr;
public:
    basic_io_buffer()
        : m_begin(m_data), m_end(m_data+N)
        , m_rd_ptr(m_data), m_wr_ptr(m_data)
    {}

    ~basic_io_buffer() { reset(); }

    /// Reset internal storage to initial state.
    void reset() {
        if (m_begin != m_data)
            delete [] m_begin;
        m_rd_ptr = m_wr_ptr = m_begin = m_data;
        m_end    = m_data + N;
    }
        
    /// Ensure there's enough space in the buffer to hold \a n bytes.
    void reallocate(size_t n) {
        if (n < max_size())
            return;
        size_t rd_offset = m_rd_ptr - m_begin;
        size_t wr_offset = m_wr_ptr - m_begin;
        char*  old_begin = m_begin;
        m_begin = new char[n];
        m_end   = m_begin + n;
        if (wr_offset > 0)
            memcpy(m_begin, old_begin, wr_offset);
        m_rd_ptr = m_begin + rd_offset;
        m_wr_ptr = m_begin + wr_offset;
        if (old_begin != m_data)
            delete [] old_begin;
    }

    size_t      max_size() const { return m_end    - m_begin;  }
    size_t      size()     const { return m_wr_ptr - m_rd_ptr; }
    size_t      available()const { return m_end    - m_wr_ptr; }

    const char* rd_ptr()   const { return m_rd_ptr; }
    const char* wr_ptr()   const { return m_wr_ptr; }
    const char* end()      const { return m_end;    }

    char*       rd_ptr()         { return m_rd_ptr; }
    char*       wr_ptr()         { return m_wr_ptr; }

    bool        allocated() const { return m_begin != m_data; }

    template <typename T>
    T* cast() throw(io_error) {
        return reinterpret_cast<T*>(read(sizeof(T)));
    }

    /// Read \a n bytes from the buffer.
    char* read(int n) throw(io_error) {
        if (size() < n)
            throw io_error("Buffer space not ready! (need=", n, ", have=", size(), ")");
        char* p = m_rd_ptr;
        m_rd_ptr += n;
        BOOST_ASSERT(m_rd_ptr <= m_wr_ptr);
        return p;
    }

    /// Adjust buffer write pointer by \a n bytes.
    void commit(int n)   { m_wr_ptr += n; BOOST_ASSERT(m_wr_ptr <= m_end); }

    /// Move any unread data to the beginning of the buffer.
    void crunch() {
        size_t sz = size();
        if (m_rd_ptr > m_begin) {
            if (sz > 0)
                memmove(m_begin, m_rd_ptr, sz);
            m_rd_ptr = m_begin;
            m_wr_ptr = m_begin + sz;
        }
    }

    /// Write \a n bytes to a buffer from a given source \a a_src.
    /// @return pointer to the next possible buffer write location.
    char* write(const char* a_src, size_t n) {
        BOOST_ASSERT(m_wr_ptr + n < m_end);
        memcpy(m_wr_ptr, a_src, n);
        m_wr_ptr += n;
        return m_wr_ptr;
    }
};

/**
 * \brief A buffer providing space for input and output I/O operations.
 */
template <int InBufSize, int OutBufSize = InBufSize>
struct io_buffer {
    basic_io_buffer<InBufSize>  in;
    basic_io_buffer<OutBufSize> out;
};

} // namespace replog

#endif // _REPLOG_BUFFER_HPP_

