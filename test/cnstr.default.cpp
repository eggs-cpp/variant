// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2016
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

EGGS_CXX11_STATIC_CONSTEXPR std::size_t npos = eggs::variant<>::npos;

TEST_CASE("variant<Ts...>::variant()", "[variant.cnstr]")
{
    eggs::variant<int, std::string> v;

    CHECK(bool(v) == false);
    CHECK(v.which() == npos);
    CHECK(v.target() == nullptr);

#if EGGS_CXX98_HAS_RTTI
    CHECK(v.target_type() == typeid(void));
#endif

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<Constexpr> v;
        constexpr bool vb = bool(v);
        constexpr std::size_t vw = v.which();
        constexpr void const* vt = v.target();

#  if EGGS_CXX98_HAS_RTTI
        constexpr std::type_info const& vtt = v.target_type();
#  endif
    }
#endif
}

TEST_CASE("variant<>::variant()", "[variant.cnstr]")
{
    eggs::variant<> v;

    CHECK(bool(v) == false);
    CHECK(v.which() == npos);
    CHECK(v.target() == nullptr);

#if EGGS_CXX98_HAS_RTTI
    CHECK(v.target_type() == typeid(void));
#endif

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<> v;
        constexpr bool vb = bool(v);
        constexpr std::size_t vw = v.which();
        constexpr void const* vt = v.target();

#  if EGGS_CXX98_HAS_RTTI
        constexpr std::type_info const& vtt = v.target_type();
#  endif
    }
#endif
}
