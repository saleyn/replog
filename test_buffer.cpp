//----------------------------------------------------------------------------
/// \file  test_buffer.cpp
//----------------------------------------------------------------------------
/// \brief Test cases for classes in the buffer.hpp file.
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

#include <boost/test/unit_test.hpp>
#include <replog/buffer.hpp>

using namespace replog;

BOOST_AUTO_TEST_CASE( test_basic_io_buffer )
{
    basic_io_buffer<40> buf;
    BOOST_REQUIRE_EQUAL(40,    buf.max_size());
    BOOST_REQUIRE_EQUAL(0,     buf.size());
    BOOST_REQUIRE_EQUAL(40,    buf.available());
    BOOST_REQUIRE_EQUAL(false, buf.allocated());

    buf.reallocate(30);
    BOOST_REQUIRE_EQUAL(40,    buf.max_size());
    BOOST_REQUIRE_EQUAL(0,     buf.size());
    BOOST_REQUIRE_EQUAL(40,    buf.available());
    BOOST_REQUIRE_EQUAL(false, buf.allocated());

    strncpy(buf.wr_ptr(), "1234567890", 10);
    BOOST_REQUIRE_EQUAL(40,    buf.max_size());
    BOOST_REQUIRE_EQUAL(0,     buf.size());
    BOOST_REQUIRE_EQUAL(40,    buf.available());
    buf.commit(10);
    BOOST_REQUIRE_EQUAL(40,    buf.max_size());
    BOOST_REQUIRE_EQUAL(10,    buf.size());
    BOOST_REQUIRE_EQUAL(30,    buf.available());

    const char* p = buf.read(10);
    BOOST_REQUIRE_EQUAL(0,     memcmp("1234567890", p, 10));

    BOOST_REQUIRE_EQUAL(40,    buf.max_size());
    BOOST_REQUIRE_EQUAL(0,     buf.size());
    BOOST_REQUIRE_EQUAL(30,    buf.available());

    p = buf.write("xx", 2);
    BOOST_REQUIRE_EQUAL(2,     buf.size());
    BOOST_REQUIRE_EQUAL(28,    buf.available());
    BOOST_REQUIRE_EQUAL(28,    buf.end() - p);

    buf.crunch();
    BOOST_REQUIRE_EQUAL(40,    buf.max_size());
    BOOST_REQUIRE_EQUAL(2,     buf.size());
    BOOST_REQUIRE_EQUAL(38,    buf.available());

    buf.read(2);
    BOOST_REQUIRE_EQUAL(0,     buf.size());
    BOOST_REQUIRE_EQUAL(38,    buf.available());

    buf.crunch();
    BOOST_REQUIRE_EQUAL(0,     buf.size());
    BOOST_REQUIRE_EQUAL(40,    buf.available());
}


