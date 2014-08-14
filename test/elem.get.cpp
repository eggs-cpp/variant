// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("get<I>(variant<Ts...>&)", "[variant.elem]")
{
    eggs::variant<int, std::string> v(42);

    REQUIRE(v.which() == 0u);
    REQUIRE(*v.target<int>() == 42);

    int& ref = eggs::variants::get<0>(v);

    CHECK(ref == 42);

    SECTION("throws")
    {
        CHECK_THROWS_AS(
            eggs::variants::get<1>(v)
          , eggs::variants::bad_variant_access);
    }
}

TEST_CASE("get<I>(variant<Ts...> const&)", "[variant.elem]")
{
    eggs::variant<int, std::string> const v(42);

    REQUIRE(v.which() == 0u);
    REQUIRE(*v.target<int>() == 42);

    int const& ref = eggs::variants::get<0>(v);

    CHECK(ref == 42);

    SECTION("throws")
    {
        CHECK_THROWS_AS(
            eggs::variants::get<1>(v)
          , eggs::variants::bad_variant_access);
    }
}

TEST_CASE("get<I>(variant<Ts...>&&)", "[variant.elem]")
{
    eggs::variant<int, std::string> v(42);

    REQUIRE(v.which() == 0u);
    REQUIRE(*v.target<int>() == 42);

    int&& ref = eggs::variants::get<0>(std::move(v));

    CHECK(ref == 42);
}

TEST_CASE("get<T>(variant<Ts...>&)", "[variant.elem]")
{
    eggs::variant<int, std::string> v(42);

    int& ref = eggs::variants::get<int>(v);

    CHECK(ref == 42);

    SECTION("throws")
    {
        CHECK_THROWS_AS(
            eggs::variants::get<std::string>(v)
          , eggs::variants::bad_variant_access);
    }
}

TEST_CASE("get<T>(variant<Ts...> const&)", "[variant.elem]")
{
    eggs::variant<int, std::string> const v(42);

    int const& ref = eggs::variants::get<int>(v);

    CHECK(ref == 42);

    SECTION("throws")
    {
        CHECK_THROWS_AS(
            eggs::variants::get<std::string>(v)
          , eggs::variants::bad_variant_access);
    }
}

TEST_CASE("get<T>(variant<Ts...>&&)", "[variant.elem]")
{
    eggs::variant<int, std::string> v(42);

    int&& ref = eggs::variants::get<int>(std::move(v));

    CHECK(ref == 42);
}
