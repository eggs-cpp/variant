// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>
#include <typeinfo>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "constexpr.hpp"

#if EGGS_CXX98_HAS_RTTI
TEST_CASE("variant<Ts...>::target_type()", "[variant.obs]")
{
    // non-empty
    {
        eggs::variant<int, std::string> const v(42);

        CHECK(v.target_type() == typeid(int));

        CHECK((noexcept(v.target_type()) == true));

#if EGGS_CXX11_HAS_CONSTEXPR_RTTI
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v(42);
            constexpr std::type_info const& vtt = v.target_type();
        }
#endif
    }

    // empty
    {
        eggs::variant<int, std::string> const v;

        CHECK(v.target_type() == typeid(void));

        CHECK((noexcept(v.target_type()) == true));

#if EGGS_CXX11_HAS_CONSTEXPR_RTTI
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v;
            constexpr std::type_info const& vtt = v.target_type();
        }
#endif
    }
}

TEST_CASE("variant<>::target_type()", "[variant.obs]")
{
    eggs::variant<> const v;

    CHECK(v.target_type() == typeid(void));

    CHECK((noexcept(v.target_type()) == true));

#if EGGS_CXX11_HAS_CONSTEXPR_RTTI
    // constexpr
    {
        constexpr eggs::variant<> v;
        constexpr std::type_info const& vtt = v.target_type();
    }
#endif
}
#endif
