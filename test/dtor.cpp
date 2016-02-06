// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2016
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <type_traits>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "dtor.hpp"

struct Y
{
    Y() = default;
    Y(Y const&) {};
    ~Y() = default;
};

TEST_CASE("variant<Ts...>::~variant()", "[variant.dtor]")
{
    {
        {
            eggs::variant<int, Dtor> v;
            v.emplace<1>();

            REQUIRE(Dtor::calls == 0u);
        }
        CHECK(Dtor::calls == 1u);
    }
    Dtor::calls = 0u;

    // trivially_destructible
    {
        eggs::variant<int, Y> v1;
        eggs::variant<int, float> v2;

#if EGGS_CXX11_STD_HAS_IS_TRIVIALLY_DESTRUCTIBLE
        CHECK(std::is_trivially_destructible<decltype(v1)>::value == true);
        CHECK(std::is_trivially_destructible<decltype(v2)>::value == true);
#endif
    }
}
