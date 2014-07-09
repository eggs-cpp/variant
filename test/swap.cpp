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

constexpr std::size_t npos = eggs::variant<>::npos;

TEST_CASE("variant<Ts...>::swap(variant<Ts...>&)", "[variant.swap]")
{
    SECTION("empty source")
    {
        eggs::variant<int, std::string> v1;

        REQUIRE(v1.which() == npos);

        eggs::variant<int, std::string> v2(42);

        REQUIRE(v2.which() == 0);
        REQUIRE(*v2.target<int>() == 42);

        v2.swap(v1);

        REQUIRE(v1.which() == 0);
        REQUIRE(*v1.target<int>() == 42);

        REQUIRE(v2.which() == npos);
    }

    SECTION("empty target")
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(v1.which() == 0);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> v2;

        REQUIRE(v2.which() == npos);

        v2.swap(v1);

        REQUIRE(v1.which() == npos);

        REQUIRE(v2.which() == 0);
        REQUIRE(*v2.target<int>() == 42);
    }

    SECTION("same target")
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(v1.which() == 0);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> v2(43);

        REQUIRE(v2.which() == v1.which());
        REQUIRE(*v2.target<int>() == 43);

        v2.swap(v1);

        REQUIRE(v1.which() == v1.which());
        REQUIRE(*v1.target<int>() == 43);

        REQUIRE(v2.which() == 0);
        REQUIRE(*v2.target<int>() == 42);
    }

    SECTION("different target")
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(v1.which() == 0);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> v2(std::string{""});

        REQUIRE(v2.which() == 1);
        REQUIRE(*v2.target<std::string>() == "");

        v2.swap(v1);

        REQUIRE(v1.which() == 1);
        REQUIRE(*v1.target<std::string>() == "");

        REQUIRE(v2.which() == 0);
        REQUIRE(*v2.target<int>() == 42);
    }
}

TEST_CASE("variant<>::swap(variant<>&)", "[variant.swap]")
{
    eggs::variant<> v1;

    REQUIRE(v1.which() == npos);

    eggs::variant<> v2;

    REQUIRE(v2.which() == npos);

    v2.swap(v1);

    REQUIRE(v1.which() == npos);

    REQUIRE(v2.which() == npos);
}
