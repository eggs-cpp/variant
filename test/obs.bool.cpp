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

TEST_CASE("variant<Ts...>::operator bool()", "[variant.obs]")
{
    // non-empty
    {
        eggs::variant<int, std::string> const v(42);

        CHECK(bool(v) == true);

        CHECK(noexcept(bool(v)) == true);

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v(42);
            constexpr bool vb = bool(v);
        }
#endif
    }

    // empty
    {
        eggs::variant<int, std::string> const v;

        CHECK(bool(v) == false);

        CHECK(noexcept(bool(v)) == true);

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v;
            constexpr bool vb = bool(v);
        }
#endif
    }
}

TEST_CASE("variant<>::operator bool()", "[variant.obs]")
{
    eggs::variant<> const v;

    CHECK(bool(v) == false);

    CHECK(noexcept(bool(v)) == true);

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<> v;
        constexpr bool vb = bool(v);
    }
#endif
}
