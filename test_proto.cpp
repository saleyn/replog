//----------------------------------------------------------------------------
/// \file  test_proto.cpp
//----------------------------------------------------------------------------
/// \brief Test cases for protocol messages.
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

#define BOOST_TEST_MODULE test_replog

#include <boost/test/unit_test.hpp>
#include <boost/smart_ptr.hpp>
#include <replog/proto.hpp>

using namespace replog;

BOOST_AUTO_TEST_CASE( test_msg_base_header )
{
    msg_base_header msg(msg_base_header::GET_SIZE, 12345u, 1u, 123456789u);
    BOOST_REQUIRE_EQUAL(msg.magic(),        msg_base_header::get_magic());
    BOOST_REQUIRE_EQUAL(msg.cmd(),          msg_base_header::GET_SIZE);
    BOOST_REQUIRE_EQUAL(msg.header_size(),  (uint16_t)12345);
    BOOST_REQUIRE_EQUAL(msg.id(),           (uint32_t)1);
    BOOST_REQUIRE_EQUAL(msg.name_hash(),    (uint32_t)123456789);
    const uint8_t expect[] = {48,57,132,83,0,0,0,1,7,91,205,21};
    BOOST_REQUIRE_EQUAL(sizeof(expect), sizeof(msg));
    BOOST_REQUIRE_EQUAL(0, memcmp(expect, &msg, sizeof(msg)));
}

BOOST_AUTO_TEST_CASE( test_msg_get_size )
{
    typedef std::allocator<char> alloc_t;
    alloc_t a;

    const char filename[] = "test.log";

    boost::scoped_ptr<msg_get_size> msg(
        msg_get_size::create(1, filename, 1234567890ull, 2, 660, a));

    BOOST_REQUIRE_EQUAL(msg->magic(),        msg_base_header::get_magic());
    BOOST_REQUIRE_EQUAL(msg->cmd(),          msg_base_header::GET_SIZE);
    BOOST_REQUIRE_EQUAL(msg->header_size(),  (uint16_t)sizeof(msg_get_size)+sizeof(filename));
    BOOST_REQUIRE_EQUAL(msg->id(),           (uint32_t)1);
    BOOST_REQUIRE_EQUAL(msg->name_hash(),    strhash(filename));
    BOOST_REQUIRE_EQUAL(msg->mode(),         (mode_t)660);
    BOOST_REQUIRE_EQUAL(msg->src_fd(),       2);
    BOOST_REQUIRE_EQUAL(msg->src_size(),     1234567890ull);
    BOOST_REQUIRE_EQUAL(msg->name(),         filename);

    const uint8_t expect[] = {
        0  ,37 ,132,83 ,0  ,0  ,0  ,1,
        240,21 ,24 ,244,0  ,0  ,2  ,148,
        0  ,0  ,0  ,2  ,0  ,0  ,0  ,0,
        73 ,150,2  ,210,116,101,115,116,
        46 ,108,111,103,0
    };
    BOOST_REQUIRE_EQUAL(sizeof(expect), msg->header_size());
    BOOST_REQUIRE_EQUAL(0, memcmp(expect, &*msg, msg->header_size()));
}

BOOST_AUTO_TEST_CASE( test_msg_get_size_response )
{
    typedef std::allocator<char> alloc_t;
    alloc_t a;

    boost::scoped_ptr<msg_get_size_response> msg(
        msg_get_size_response::create(1, 123456789u, 2, 1234567890ull, a));

    BOOST_REQUIRE_EQUAL(msg->magic(),       msg_base_header::get_magic());
    BOOST_REQUIRE_EQUAL(msg->cmd(),         msg_base_header::GET_SIZE_RESPONSE);
    BOOST_REQUIRE_EQUAL(msg->header_size(), (uint16_t)sizeof(msg_get_size_response));
    BOOST_REQUIRE_EQUAL(msg->id(),          (uint32_t)1);
    BOOST_REQUIRE_EQUAL(msg->name_hash(),   123456789u);
    BOOST_REQUIRE_EQUAL(msg->dst_fd(),      2);
    BOOST_REQUIRE_EQUAL(msg->dst_size(),    1234567890ull);
    const uint8_t expect[] = {
        0,  24, 132,115,0  ,0  ,0  ,1,
        7,  91, 205,21 ,0  ,0  ,0  ,2,
        0,  0 , 0  ,0  ,73 ,150,2  ,210
    };
    BOOST_REQUIRE_EQUAL(sizeof(expect), msg->header_size());
    BOOST_REQUIRE_EQUAL(0, memcmp(expect, &*msg, msg->header_size()));
}

BOOST_AUTO_TEST_CASE( test_msg_append )
{
    typedef std::allocator<char> alloc_t;
    alloc_t a;

    boost::scoped_ptr<msg_append> msg(
        msg_append::create(1, 123456789u, 2, 1234567890ull, 1234u, a));

    BOOST_REQUIRE_EQUAL(msg->magic(),       msg_base_header::get_magic());
    BOOST_REQUIRE_EQUAL(msg->cmd(),         msg_base_header::APPEND);
    BOOST_REQUIRE_EQUAL(msg->header_size(), (uint16_t)sizeof(msg_append));
    BOOST_REQUIRE_EQUAL(msg->id(),          (uint32_t)1);
    BOOST_REQUIRE_EQUAL(msg->name_hash(),   123456789u);
    BOOST_REQUIRE_EQUAL(msg->dst_fd(),      2);
    BOOST_REQUIRE_EQUAL(msg->src_offset(),  1234567890ull);
    BOOST_REQUIRE_EQUAL(msg->chunk_size(),  1234u);
    const uint8_t expect[] = {
        0,  28 ,132,65 ,0  ,0  ,0  ,1,
        7,  91 ,205,21 ,0  ,0  ,0  ,2,
        0,  0  ,0  ,0  ,73 ,150,2  ,210,
        0,  0  ,4  ,210
    };
    BOOST_REQUIRE_EQUAL(sizeof(expect), msg->header_size());
    BOOST_REQUIRE_EQUAL(0, memcmp(expect, &*msg, msg->header_size()));
}

BOOST_AUTO_TEST_CASE( test_msg_resend_request )
{
    typedef std::allocator<char> alloc_t;
    alloc_t a;

    boost::scoped_ptr<msg_resend_request> msg(
        msg_resend_request::create(1, 123456789u, 1234567890ull, a));

    BOOST_REQUIRE_EQUAL(msg->magic(),       msg_base_header::get_magic());
    BOOST_REQUIRE_EQUAL(msg->cmd(),         msg_base_header::RESEND_REQUEST);
    BOOST_REQUIRE_EQUAL(msg->header_size(), (uint16_t)sizeof(msg_resend_request));
    BOOST_REQUIRE_EQUAL(msg->id(),          (uint32_t)1);
    BOOST_REQUIRE_EQUAL(msg->name_hash(),   123456789u);
    BOOST_REQUIRE_EQUAL(msg->dst_size(),    1234567890ull);
    const uint8_t expect[] = {
        0  ,20 ,132,114,0  ,0  ,0  ,1,
        7  ,91 ,205,21 ,0  ,0  ,0  ,0,
        73 ,150,2  ,210
    };
    BOOST_REQUIRE_EQUAL(sizeof(expect), msg->header_size());
    BOOST_REQUIRE_EQUAL(0, memcmp(expect, &*msg, msg->header_size()));
}

BOOST_AUTO_TEST_CASE( test_msg_error_response )
{
    typedef std::allocator<char> alloc_t;
    alloc_t a;

    const char error[] = "error";

    boost::scoped_ptr<msg_error_response> msg(
        msg_error_response::create(1, 123456789u, msg_base_header::GET_SIZE, error, a));

    BOOST_REQUIRE_EQUAL(msg->magic(),       msg_base_header::get_magic());
    BOOST_REQUIRE_EQUAL(msg->cmd(),         msg_base_header::ERROR_RESPONSE);
    BOOST_REQUIRE_EQUAL(msg->header_size(), (uint16_t)sizeof(msg_error_response)+sizeof(error));
    BOOST_REQUIRE_EQUAL(msg->id(),          (uint32_t)1);
    BOOST_REQUIRE_EQUAL(msg->name_hash(),   123456789u);
    BOOST_REQUIRE_EQUAL(msg->last_cmd(),    msg_base_header::GET_SIZE);
    BOOST_REQUIRE_EQUAL(msg->error(),       error);

    const uint8_t expect[] = {
        0  ,19 ,132,101,0  ,0  ,0  ,1,
        7  ,91 ,205,21 ,83 ,101,114,114,
        111,114,0
    };
    BOOST_REQUIRE_EQUAL(sizeof(expect), msg->header_size());
    BOOST_REQUIRE_EQUAL(0, memcmp(expect, &*msg, msg->header_size()));
}
