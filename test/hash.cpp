// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2016
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <functional>
#include <string>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("std::hash<variant<Ts...>>", "[variant.hash]")
{
    eggs::variant<int, std::string> const v(42);

    REQUIRE(v.which() == 0u);
    REQUIRE(*v.target<int>() == 42);

    std::hash<eggs::variant<int, std::string>> variant_hasher;
    std::hash<int> int_hasher;

    CHECK(variant_hasher(v) == int_hasher(42));
}
