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

using eggs::variants::in_place;

TEST_CASE("variant<Ts...>::variant(in_place<I>, Args&&...)", "[variant.cnstr]")
{
    eggs::variant<int, std::string> v(in_place<0>, 42);

    CHECK(bool(v) == true);
    CHECK(v.which() == 0u);
    CHECK(v.target_type() == typeid(int));
    REQUIRE(v.target<int>() != nullptr);
    CHECK(*v.target<int>() == 42);
}

TEST_CASE("variant<Ts...>::variant(in_place<I>, std::initializer_list<U>, Args&&...)", "[variant.cnstr]")
{
    eggs::variant<int, std::string> v(in_place<1>, {'4', '2'});

    CHECK(bool(v) == true);
    CHECK(v.which() == 1u);
    CHECK(v.target_type() == typeid(std::string));
    REQUIRE(v.target<std::string>() != nullptr);
    CHECK(*v.target<std::string>() == "42");
}

TEST_CASE("variant<Ts...>::variant(in_place<T>, Args&&...)", "[variant.cnstr]")
{
    eggs::variant<int, std::string> v(in_place<int>, 42);

    CHECK(bool(v) == true);
    CHECK(v.which() == 0u);
    CHECK(v.target_type() == typeid(int));
    REQUIRE(v.target<int>() != nullptr);
    CHECK(*v.target<int>() == 42);
}

TEST_CASE("variant<Ts...>::variant(in_place<T>, std::initializer_list<U>, Args&&...)", "[variant.cnstr]")
{
    eggs::variant<int, std::string> v(in_place<std::string>, {'4', '2'});

    CHECK(bool(v) == true);
    CHECK(v.which() == 1u);
    CHECK(v.target_type() == typeid(std::string));
    REQUIRE(v.target<std::string>() != nullptr);
    CHECK(*v.target<std::string>() == "42");
}
