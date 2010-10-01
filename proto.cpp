//----------------------------------------------------------------------------
/// \file  proto.hpp
//----------------------------------------------------------------------------
/// \brief Implementation of REPLOG protocol messages.
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
#include <replog/proto.hpp>

namespace replog {

msg_base_header* msg_base_header::decode_header(char* a_buf, size_t len)
{
    if (len < sizeof(msg_base_header))
        throw replog_error("Bad buffer size:", len);
    msg_base_header* p = reinterpret_cast<msg_base_header*>(a_buf);
    if (p->magic() != s_magic_header)
        throw replog_error("Wrong magic number:", p->magic());
    uint16_t n  = p->header_size();
    uint16_t min_sz;
    switch (p->cmd()) {
        case GET_SIZE:          min_sz = sizeof(msg_get_size);          break;
        case GET_SIZE_RESPONSE: min_sz = sizeof(msg_get_size_response); break;
        case MOVE_FILE:         min_sz = sizeof(msg_move_file);         break;
        case DELETE_FILE:       min_sz = sizeof(msg_delete_file);       break;
        case APPEND:            min_sz = sizeof(msg_append);            break;
        case ERROR_RESPONSE:    min_sz = sizeof(msg_error_response);    break;
        case RESEND_REQUEST:    min_sz = sizeof(msg_resend_request);    break;
        default:
            throw replog_error("Unknown command type:", p->cmd());
    }
    if (n < min_sz)
        throw replog_error("Bad header size (got=", n, ", expected=", min_sz, ")");
}

} // namespace replog
