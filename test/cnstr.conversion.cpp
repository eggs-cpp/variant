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

TEST_CASE("variant<Ts...>::variant<Ts...>(T&&)", "[variant.cnstr]")
{
    eggs::variant<int, std::string> v(42);

    REQUIRE(bool(v) == true);
    REQUIRE(v.which() == 0);
    REQUIRE(v.target_type() == typeid(int));
    REQUIRE(v.target() == v.target<int>());
    REQUIRE(*v.target<int>() == 42);
}
