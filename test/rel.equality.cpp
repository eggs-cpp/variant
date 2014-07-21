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

TEST_CASE("operator==(variant<Ts...> const&, variant<Ts...> const&)", "[variant.rel]")
{
    SECTION("same members")
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(v1.which() == 0);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> v2(42);

        REQUIRE(v2.which() == v1.which());
        REQUIRE(*v2.target<int>() == 42);

        REQUIRE(v1 == v2);
    }

    SECTION("empty member")
    {
        eggs::variant<int, std::string> v1;

        REQUIRE(v1.which() == npos);

        eggs::variant<int, std::string> v2(42);

        REQUIRE(v2.which() == 0);
        REQUIRE(*v2.target<int>() == 42);

        REQUIRE(v1 != v2);
    }

    SECTION("different members")
    {
        eggs::variant<int, std::string> v1(std::string{""});

        REQUIRE(v1.which() == 1);
        REQUIRE(*v1.target<std::string>() == "");

        eggs::variant<int, std::string> v2(42);

        REQUIRE(v2.which() == 0);
        REQUIRE(*v2.target<int>() == 42);

        REQUIRE(v1 != v2);
    }
}

TEST_CASE("operator==(variant<Ts...> const&, T const&)", "[variant.rel]")
{
    SECTION("same members")
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(v1.which() == 0);
        REQUIRE(*v1.target<int>() == 42);

        REQUIRE(v1 == 42);
    }

    SECTION("empty member")
    {
        eggs::variant<int, std::string> v1;

        REQUIRE(v1.which() == npos);

        REQUIRE(v1 != 42);
    }

    SECTION("different members")
    {
        eggs::variant<int, std::string> v1(std::string{""});

        REQUIRE(v1.which() == 1);
        REQUIRE(*v1.target<std::string>() == "");

        REQUIRE(v1 != 42);
    }
}

TEST_CASE("operator==(T const&, variant<Ts...> const&)", "[variant.rel]")
{
    SECTION("same members")
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(v1.which() == 0);
        REQUIRE(*v1.target<int>() == 42);

        REQUIRE(42 == v1);
    }

    SECTION("empty member")
    {
        eggs::variant<int, std::string> v1;

        REQUIRE(v1.which() == npos);

        REQUIRE(42 != v1);
    }

    SECTION("different members")
    {
        eggs::variant<int, std::string> v1(std::string{""});

        REQUIRE(v1.which() == 1);
        REQUIRE(*v1.target<std::string>() == "");

        REQUIRE(42 != v1);
    }
}

TEST_CASE("operator==(variant<> const&, variant<> const&)", "[variant.rel]")
{
    eggs::variant<> v1;

    REQUIRE(v1.which() == npos);

    eggs::variant<> v2;

    REQUIRE(v2.which() == npos);

    REQUIRE(v1 == v2);
}
