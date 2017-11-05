// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "constexpr.hpp"

TEST_CASE("variant<Ts...>::variant()", "[variant.cnstr]")
{
    eggs::variant<int, std::string> v;

    CHECK(bool(v) == false);
    CHECK(v.which() == eggs::variant_npos);
    CHECK(v.target() == nullptr);

#if EGGS_CXX98_HAS_RTTI
    CHECK(v.target_type() == typeid(void));
#endif

    CHECK(noexcept(eggs::variant<int, std::string>()) == true);

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<Constexpr> v;
    }
#endif
}

TEST_CASE("variant<>::variant()", "[variant.cnstr]")
{
    eggs::variant<> v;

    CHECK(bool(v) == false);
    CHECK(v.which() == eggs::variant_npos);
    CHECK(v.target() == nullptr);

#if EGGS_CXX98_HAS_RTTI
    CHECK(v.target_type() == typeid(void));
#endif

    CHECK(noexcept(eggs::variant<>()) == true);

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<> v;
    }
#endif
}
