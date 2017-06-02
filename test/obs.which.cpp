// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "constexpr.hpp"

EGGS_CXX11_STATIC_CONSTEXPR std::size_t npos = eggs::variant<>::npos;

TEST_CASE("variant<Ts...>::which()", "[variant.obs]")
{
    // non-empty
    {
        eggs::variant<int, std::string> const v(42);

        CHECK(v.which() == 0);

#if EGGS_CXX11_HAS_NOEXCEPT
        CHECK((noexcept(v.which()) == true));
#endif

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v(42);
            constexpr std::size_t vw = v.which();
        }
#endif
    }

    // empty
    {
        eggs::variant<int, std::string> const v;

        CHECK(v.which() == npos);

#if EGGS_CXX11_HAS_NOEXCEPT
        CHECK((noexcept(v.which()) == true));
#endif

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v;
            constexpr std::size_t vw = v.which();
        }
#endif
    }
}

TEST_CASE("variant<>::which()", "[variant.obs]")
{
    eggs::variant<> const v;

    CHECK(v.which() == npos);

#if EGGS_CXX11_HAS_NOEXCEPT
    CHECK((noexcept(v.which()) == true));
#endif

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<> v;
        constexpr std::size_t vw = v.which();
    }
#endif
}
