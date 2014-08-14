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

using eggs::variants::nullvariant;

EGGS_CXX11_STATIC_CONSTEXPR std::size_t npos = eggs::variant<>::npos;

TEST_CASE("variant<Ts...>::variant(T&&)", "[variant.cnstr]")
{
    eggs::variant<int, std::string> v(42);

    CHECK(bool(v) == true);
    CHECK(v.which() == 0u);
    CHECK(v.target_type() == typeid(int));
    CHECK(v.target() == v.target<int>());
    REQUIRE(v.target<int>() != nullptr);
    CHECK(*v.target<int>() == 42);
}

TEST_CASE("variant<Ts...>::variant(nullvariant_t)", "[variant.cnstr]")
{
    eggs::variant<int, std::string> v(nullvariant);

    CHECK(bool(v) == false);
    CHECK(v.which() == npos);
    CHECK(v.target() == nullptr);
    CHECK(v.target_type() == typeid(void));

    SECTION("initializer-list")
    {
        eggs::variant<int, std::string> v = {};

        CHECK(bool(v) == false);
        CHECK(v.which() == npos);
        CHECK(v.target() == nullptr);
        CHECK(v.target_type() == typeid(void));
    }
}

TEST_CASE("variant<>::variant(nullvariant_t)", "[variant.cnstr]")
{
    eggs::variant<> v(nullvariant);

    CHECK(bool(v) == false);
    CHECK(v.which() == npos);
    CHECK(v.target() == nullptr);
    CHECK(v.target_type() == typeid(void));
}
