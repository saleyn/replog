//----------------------------------------------------------------------------
/// \file  test_raw_char.cpp
//----------------------------------------------------------------------------
/// \brief Test cases for raw_char class.
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
#include <replog/raw_char.hpp>

using namespace replog;

BOOST_AUTO_TEST_CASE( test_raw_char )
{
    {
        raw_char<4> rc("abcd", 4);
        BOOST_REQUIRE_EQUAL(rc.to_string(), "abcd");
        BOOST_REQUIRE_EQUAL(rc.to_string().size(), 4u);
        rc = std::string("ff");
        BOOST_REQUIRE_EQUAL(rc.to_string(), "ff");
        BOOST_REQUIRE_EQUAL(rc.to_string().size(), 2u);
        const uint8_t expect[] = {'f','f',0,0};
        BOOST_REQUIRE_EQUAL(0, memcmp(expect, (char*)rc, sizeof(expect)));
        rc = std::string("");
        BOOST_REQUIRE_EQUAL(rc.to_string(), "");
        BOOST_REQUIRE_EQUAL(rc.to_string().size(), 0u);
    }
    {
        const char s[] = "abcd";
        raw_char<4> rc(s);
        BOOST_REQUIRE_EQUAL(rc.to_string(), "abcd");
        std::stringstream str;
        rc.dump(str);
        BOOST_REQUIRE_EQUAL(str.str(), "abcd");
    }
    {
        raw_char<4> rc(1);
        BOOST_REQUIRE_EQUAL(1, (int)rc);
        const uint8_t expect[] = {0,0,0,1};
        BOOST_REQUIRE_EQUAL(0, memcmp(expect, (char*)rc, sizeof(expect)));
        std::stringstream str;
        rc.dump(str);
        BOOST_REQUIRE_EQUAL(str.str(), "0,0,0,1");
        rc.fill(' ');
        BOOST_REQUIRE_EQUAL(rc.to_string(), "    ");
    }
    {
        raw_char<2> rc((uint16_t)1);
        BOOST_REQUIRE_EQUAL(1, (uint16_t)rc);
        const uint8_t expect[] = {0,1};
        BOOST_REQUIRE_EQUAL(0, memcmp(expect, (char*)rc, sizeof(expect)));
    }
}


