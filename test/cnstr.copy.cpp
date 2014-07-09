// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("variant<Ts...>::variant(variant<Ts...> const&)", "[variant.cnstr]")
{
    eggs::variant<int, std::string> v1(42);

    REQUIRE(bool(v1) == true);
    REQUIRE(v1.which() == 0);
    REQUIRE(*v1.target<int>() == 42);

    eggs::variant<int, std::string> v2(v1);

    REQUIRE(bool(v2) == true);
    REQUIRE(v2.which() == v1.which());
    REQUIRE(*v1.target<int>() == 42);
    REQUIRE(*v2.target<int>() == 42);
}

TEST_CASE("variant<>::variant(variant<> const&)", "[variant.cnstr]")
{
    eggs::variant<> v1;

    REQUIRE(bool(v1) == false);
    REQUIRE(v1.which() == npos);

    eggs::variant<> v2(v1);

    REQUIRE(bool(v2) == false);
    REQUIRE(v2.which() == v1.which());
}
