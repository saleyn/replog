//----------------------------------------------------------------------------
/// \file  proto.hpp
//----------------------------------------------------------------------------
/// \brief Definition of messages used by REPLOG protocol.
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
#ifndef _REPLOG_PROTO_HPP_
#define _REPLOG_PROTO_HPP_

#include <string>
#include <string.h>

#include <replog/error.hpp>
#include <replog/endian.hpp>
#include <replog/buffer.hpp>
#include <replog/raw_char.hpp>
#include <replog/hashtable.hpp>

namespace replog {

class msg_base_header {
protected:
    static const uint8_t s_magic_header = 132;

    raw_char<2> m_header_size;
    uint8_t     m_magic;
    char        m_cmd;
    raw_char<4> m_id;           // File identifier.
    raw_char<4> m_name_hash;    // File identifier.

public:
    enum cmd_type {
          GET_SIZE          = 'S'
        , GET_SIZE_RESPONSE = 's'
        , MOVE_FILE         = 'M'
        , DELETE_FILE       = 'D'
        , APPEND            = 'A'
        , RESEND_REQUEST    = 'r'
        , ERROR_RESPONSE    = 'e'
    };
    
    msg_base_header(cmd_type a_cmd, uint16_t a_msg_size, uint32_t a_id,
        uint32_t a_name_hash)
        : m_magic(s_magic_header), m_cmd(a_cmd)
    {
        m_header_size   = a_msg_size;
        m_id            = a_id;
        m_name_hash     = a_name_hash;
    }

    static uint8_t get_magic()      { return s_magic_header; }

    uint16_t header_size()  const   { uint16_t n = (uint16_t)m_header_size; return n; }
    uint8_t  magic()        const   { return m_magic;       }
    cmd_type cmd()          const   { return static_cast<cmd_type>(m_cmd); }
    uint32_t id()           const   { return m_id;          }
    uint32_t name_hash()    const   { return m_name_hash;   }

    static msg_base_header*
    decode_header(char* a_buf, size_t len);
};

class msg_get_size : public msg_base_header {
    msg_get_size(size_t a_msg_size, uint32_t a_id, uint32_t a_name_hash)
        : msg_base_header(GET_SIZE, a_msg_size, a_id, a_name_hash)
    {}
    raw_char<4> m_mode;
    raw_char<4> m_src_fd;
    raw_char<8> m_src_size;
    char        m_name[0];
public:
    mode_t      mode()      const { return m_mode; }
    int         src_fd()    const { return m_src_fd; }
    uint64_t    src_size()  const { return m_src_size; }
    const char* name()      const { return m_name; }

    template <typename Alloc>
    static msg_get_size*
    create(uint32_t a_id, const std::string& a_filename, uint64_t a_src_size,
           int a_src_fd, mode_t a_mode, const Alloc& a = Alloc())
    {
        size_t size = sizeof(msg_get_size) + a_filename.size() + 1;
        msg_get_size* p = reinterpret_cast<msg_get_size*>(Alloc(a).allocate(size));
        uint32_t name_hash = strhash(a_filename);
        new (p) msg_get_size(size, a_id, name_hash);
        p->m_mode     = a_mode;
        p->m_src_fd   = a_src_fd;
        p->m_src_size = a_src_size;
        strcpy(p->m_name, a_filename.c_str());
        return p;
    }
};

// FIXME: Not implemented yet.
class msg_move_file {};
class msg_delete_file {};

class msg_get_size_response : public msg_base_header {
    msg_get_size_response(size_t a_msg_size, uint32_t a_id, uint32_t a_name_hash)
        : msg_base_header(GET_SIZE_RESPONSE, a_msg_size, a_id, a_name_hash)
    {}

    raw_char<4> m_dst_fd;
    raw_char<8> m_dst_size; // Remote size
public:
    int         dst_fd()    const { return m_dst_fd;   }
    uint64_t    dst_size()  const { return m_dst_size; }

    template <typename Alloc>
    static msg_get_size_response*
    create(uint32_t a_id, uint32_t a_name_hash, int a_dst_fd, uint64_t a_dst_size,
           const Alloc& a = Alloc())
    {
        size_t size = sizeof(msg_get_size_response);
        msg_get_size_response* p =
            reinterpret_cast<msg_get_size_response*>(Alloc(a).allocate(size));
        new (p) msg_get_size_response(size, a_id, a_name_hash);
        p->m_dst_fd   = a_dst_fd;
        p->m_dst_size = a_dst_size;
        return p;
    }
};

class msg_append : public msg_base_header {
    msg_append(size_t a_msg_size, uint32_t a_id, uint32_t a_name_hash)
        : msg_base_header(APPEND, a_msg_size, a_id, a_name_hash)
    {}

    raw_char<4> m_dst_fd;
    raw_char<8> m_src_offset; // Source file offset
    raw_char<4> m_chunk_size; // Data chunk size to append
public:
    int      dst_fd()       const { return m_dst_fd; }
    uint64_t src_offset()   const { return m_src_offset; }
    uint32_t chunk_size()   const { return m_chunk_size; }

    template <typename Alloc>
    static msg_append*
    create(uint32_t a_id, uint32_t a_name_hash, int a_dst_fd, uint64_t a_src_offset,
           uint32_t a_chunk_size, const Alloc& a = Alloc())
    {
        size_t size = sizeof(msg_append);
        msg_append* p =
            reinterpret_cast<msg_append*>(Alloc(a).allocate(size));
        new (p) msg_append(size, a_id, a_name_hash);
        p->m_dst_fd     = a_dst_fd;
        p->m_src_offset = a_src_offset;
        p->m_chunk_size = a_chunk_size;
        return p;
    }
};

class msg_resend_request : public msg_base_header {
    msg_resend_request(size_t a_msg_size, uint32_t a_id, uint32_t a_name_hash)
        : msg_base_header(RESEND_REQUEST, a_msg_size, a_id, a_name_hash)
    {}
    raw_char<8> m_dst_size;   // Destination file size
public:
    uint64_t dst_size() const { return m_dst_size; }

    template <typename Alloc>
    static msg_resend_request*
    create(uint32_t a_id, uint32_t a_name_hash, uint64_t a_dst_size,
           const Alloc& a = Alloc())
    {
        size_t size = sizeof(msg_resend_request);
        msg_resend_request* p =
            reinterpret_cast<msg_resend_request*>(Alloc(a).allocate(size));
        new (p) msg_resend_request(size, a_id, a_name_hash);
        p->m_dst_size   = a_dst_size;
        return p;
    }
};

class msg_error_response : public msg_base_header {
    msg_error_response(size_t a_msg_size, uint32_t a_id, uint32_t a_name_hash)
        : msg_base_header(ERROR_RESPONSE, a_msg_size, a_id, a_name_hash)
    {}

    char m_last_cmd;
    char m_error[0];
public:
    cmd_type last_cmd() const { return static_cast<cmd_type>(m_last_cmd); }
    const char* error() const { return m_error; }

    template <typename Alloc>
    static msg_error_response*
    create(uint32_t a_id, uint32_t a_name_hash, 
           cmd_type a_last_cmd, const std::string& a_error, const Alloc& a = Alloc())
    {
        size_t size = sizeof(msg_error_response) + a_error.size() + 1;
        msg_error_response* p =
            reinterpret_cast<msg_error_response*>(Alloc(a).allocate(size));
        new (p) msg_error_response(size, a_id, a_name_hash);
        p->m_last_cmd = static_cast<char>(a_last_cmd);
        strcpy(p->m_error, a_error.c_str());
        return p;
    }
};

} // namespace replog

#endif // _REPLOG_PROTO_HPP_
