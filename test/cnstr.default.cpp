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
#include "constexpr.hpp"

EGGS_CXX11_STATIC_CONSTEXPR std::size_t npos = eggs::variant<>::npos;

TEST_CASE("variant<Ts...>::variant()", "[variant.cnstr]")
{
    eggs::variant<int, std::string> v;

    CHECK(bool(v) == false);
    CHECK(v.which() == npos);
    CHECK(v.target() == nullptr);
    CHECK(v.target_type() == typeid(void));

#if EGGS_CXX11_HAS_CONSTEXPR
    SECTION("constexpr")
    {
        constexpr eggs::variant<Constexpr> v;
        constexpr bool vb = bool(v);
        constexpr std::size_t vw = v.which();
        constexpr void const* vt = v.target();
        constexpr std::type_info const& vtt = v.target_type();
    }
#endif
}

TEST_CASE("variant<>::variant()", "[variant.cnstr]")
{
    eggs::variant<> v;

    CHECK(bool(v) == false);
    CHECK(v.which() == npos);
    CHECK(v.target() == nullptr);
    CHECK(v.target_type() == typeid(void));

#if EGGS_CXX11_HAS_CONSTEXPR
    SECTION("constexpr")
    {
        constexpr eggs::variant<> v;
        constexpr bool vb = bool(v);
        constexpr std::size_t vw = v.which();
        constexpr void const* vt = v.target();
        constexpr std::type_info const& vtt = v.target_type();
    }
#endif
}
