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

TEST_CASE("operator<(variant<Ts...> const&, variant<Ts...> const&)", "[variant.rel]")
{
    SECTION("same members")
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> const v2(43);

        REQUIRE(v2.which() == v1.which());
        REQUIRE(*v2.target<int>() == 43);

        REQUIRE(v1 < v2);
    }

    SECTION("empty member")
    {
        eggs::variant<int, std::string> const v1;

        REQUIRE(v1.which() == npos);

        eggs::variant<int, std::string> const v2(42);

        REQUIRE(v2.which() == 0);
        REQUIRE(*v2.target<int>() == 42);

        REQUIRE(v1 < v2);
    }

    SECTION("different members")
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> const v2(std::string{""});

        REQUIRE(v2.which() == 1);
        REQUIRE(*v2.target<std::string>() == "");

        REQUIRE(v1 < v2);
    }
}

TEST_CASE("operator<(variant<Ts...> const&, T const&)", "[variant.rel]")
{
    SECTION("same members")
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0);
        REQUIRE(*v1.target<int>() == 42);

        REQUIRE(v1 < 43);
    }

    SECTION("empty member")
    {
        eggs::variant<int, std::string> const v1;

        REQUIRE(v1.which() == npos);

        REQUIRE(v1 < 42);
    }

    SECTION("different members")
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0);
        REQUIRE(*v1.target<int>() == 42);

        REQUIRE(v1 < std::string{""});
    }
}

TEST_CASE("operator<(T const&, variant<Ts...> const&)", "[variant.rel]")
{
    SECTION("same members")
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0);
        REQUIRE(*v1.target<int>() == 42);

        REQUIRE(41 < v1);
    }

    SECTION("empty member")
    {
        eggs::variant<int, std::string> const v1;

        REQUIRE(v1.which() == npos);

        REQUIRE((42 < v1) == false);
    }

    SECTION("different members")
    {
        eggs::variant<int, std::string> const v1(std::string{""});

        REQUIRE(v1.which() == 1);
        REQUIRE(*v1.target<std::string>() == "");

        REQUIRE(42 < v1);
    }
}

TEST_CASE("operator<(variant<Ts...> const&, nullvariant_t)", "[variant.rel]")
{
    SECTION("empty member")
    {
        eggs::variant<int, std::string> const v1;

        REQUIRE(v1.which() == npos);

        REQUIRE((v1 < nullvariant) == false);
        REQUIRE(v1 <= nullvariant);
        REQUIRE((v1 > nullvariant) == false);
        REQUIRE(v1 >= nullvariant);
    }

    SECTION("non-empty members")
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0);
        REQUIRE(*v1.target<int>() == 42);

        REQUIRE((v1 < nullvariant) == false);
        REQUIRE((v1 <= nullvariant) == false);
        REQUIRE(v1 > nullvariant);
        REQUIRE(v1 >= nullvariant);
    }
}

TEST_CASE("operator<(nullvariant_t, variant<Ts...> const&)", "[variant.rel]")
{
    SECTION("empty member")
    {
        eggs::variant<int, std::string> const v1;

        REQUIRE(v1.which() == npos);

        REQUIRE((nullvariant < v1) == false);
        REQUIRE(nullvariant <= v1);
        REQUIRE((nullvariant > v1) == false);
        REQUIRE(nullvariant >= v1);
    }

    SECTION("non-empty members")
    {
        eggs::variant<int, std::string> const v1(42);

        REQUIRE(v1.which() == 0);
        REQUIRE(*v1.target<int>() == 42);

        REQUIRE(nullvariant < v1);
        REQUIRE(nullvariant <= v1);
        REQUIRE((nullvariant > v1) == false);
        REQUIRE((nullvariant >= v1) == false);
    }
}

TEST_CASE("operator<(variant<> const&, variant<> const&)", "[variant.rel]")
{
    eggs::variant<> const v1;

    REQUIRE(v1.which() == npos);

    eggs::variant<> const v2;

    REQUIRE(v2.which() == npos);

    REQUIRE((v1 < v2) == false);
    REQUIRE((v2 < v1) == false);
}
