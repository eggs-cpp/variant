// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "dtor.hpp"

TEST_CASE("variant<Ts...>::~variant()", "[variant.dtor]")
{
    {
        {
            eggs::variant<int, Dtor> v;
            v.emplace<Dtor>();

            REQUIRE(Dtor::called == false);
        }
        REQUIRE(Dtor::called == true);
    }
    Dtor::called = false;
}
