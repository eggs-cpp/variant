// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2016
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>
#include <type_traits>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "constexpr.hpp"

EGGS_CXX11_STATIC_CONSTEXPR std::size_t npos = eggs::variant<>::npos;

TEST_CASE("variant<Ts...>::variant(variant<Ts...> const&)", "[variant.cnstr]")
{
    eggs::variant<int, std::string> const v1(42);

    REQUIRE(bool(v1) == true);
    REQUIRE(v1.which() == 0u);
    REQUIRE(*v1.target<int>() == 42);

    eggs::variant<int, std::string> v2(v1);

    CHECK(bool(v2) == true);
    CHECK(v2.which() == v1.which());
    REQUIRE(v1.target<int>() != nullptr);
    CHECK(*v1.target<int>() == 42);
    REQUIRE(v2.target<int>() != nullptr);
    CHECK(*v2.target<int>() == 42);

#if EGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE
    // trivially_copyable
    {
        eggs::variant<int, float> v1(42);

        REQUIRE(bool(v1) == true);
        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        CHECK(std::is_trivially_copyable<decltype(v1)>::value == true);

        eggs::variant<int, float> v2(v1);

        CHECK(bool(v2) == true);
        CHECK(v2.which() == v1.which());
        REQUIRE(v1.target<int>() != nullptr);
        CHECK(*v1.target<int>() == 42);
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);
    }
#endif

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<int, ConstexprTrivial> v1(ConstexprTrivial(42));
        constexpr eggs::variant<int, ConstexprTrivial> v2(v1);
    }
#endif
}

TEST_CASE("variant<>::variant(variant<> const&)", "[variant.cnstr]")
{
    eggs::variant<> const v1;

    REQUIRE(bool(v1) == false);
    REQUIRE(v1.which() == npos);

    eggs::variant<> v2(v1);

    CHECK(bool(v2) == false);
    CHECK(v2.which() == v1.which());

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<> v1;
        constexpr eggs::variant<> v2(v1);
    }
#endif
}
