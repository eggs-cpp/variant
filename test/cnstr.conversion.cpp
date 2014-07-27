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

using eggs::variants::nullvariant;

constexpr std::size_t npos = eggs::variant<>::npos;

TEST_CASE("variant<Ts...>::variant(T&&)", "[variant.cnstr]")
{
    eggs::variant<int, std::string> v(42);

    REQUIRE(bool(v) == true);
    REQUIRE(v.which() == 0);
    REQUIRE(v.target_type() == typeid(int));
    REQUIRE(v.target() == v.target<int>());
    REQUIRE(*v.target<int>() == 42);
}

TEST_CASE("variant<Ts...>::variant(nullvariant_t)", "[variant.cnstr]")
{
    eggs::variant<int, std::string> v(nullvariant);

    REQUIRE(bool(v) == false);
    REQUIRE(v.which() == npos);
    REQUIRE(v.target() == nullptr);
    REQUIRE(v.target_type() == typeid(void));

    SECTION("initializer-list")
    {
        eggs::variant<int, std::string> v = {};

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == npos);
        REQUIRE(v.target() == nullptr);
        REQUIRE(v.target_type() == typeid(void));
    }
}

TEST_CASE("variant<>::variant(nullvariant_t)", "[variant.cnstr]")
{
    eggs::variant<> v(nullvariant);

    REQUIRE(bool(v) == false);
    REQUIRE(v.which() == npos);
    REQUIRE(v.target() == nullptr);
    REQUIRE(v.target_type() == typeid(void));
}
