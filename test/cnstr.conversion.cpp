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

using eggs::variants::nullvariant;

EGGS_CXX11_STATIC_CONSTEXPR std::size_t npos = eggs::variant<>::npos;

TEST_CASE("variant<Ts...>::variant(T&&)", "[variant.cnstr]")
{
    eggs::variant<int, std::string> v(42);

    CHECK(bool(v) == true);
    CHECK(v.which() == 0u);
    CHECK(v.target_type() == typeid(int));
    REQUIRE(v.target<int>() != nullptr);
    CHECK(*v.target<int>() == 42);

#if EGGS_CXX11_HAS_CONSTEXPR
    SECTION("constexpr")
    {
        constexpr eggs::variant<int, Constexpr> v(Constexpr(42));
        constexpr bool vb = bool(v);
        constexpr std::size_t vw = v.which();
        constexpr bool vttb = v.target<Constexpr>()->x == 42;
        constexpr std::type_info const& vtt = v.target_type();

#  if EGGS_CXX14_HAS_CONSTEXPR
        struct test { static constexpr int call()
        {
            eggs::variant<int, Constexpr> v(Constexpr(42));
            v.target<Constexpr>()->x = 43;
            return 0;
        }};
        constexpr int c = test::call();
#  endif
    }
#endif
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

#if EGGS_CXX11_HAS_CONSTEXPR
    SECTION("constexpr")
    {
        constexpr eggs::variant<int, Constexpr> v(nullvariant);
        constexpr bool vb = bool(v);
        constexpr std::size_t vw = v.which();
        constexpr void const* vt = v.target();
        constexpr std::type_info const& vtt = v.target_type();
    }
#endif
}

TEST_CASE("variant<>::variant(nullvariant_t)", "[variant.cnstr]")
{
    eggs::variant<> v(nullvariant);

    CHECK(bool(v) == false);
    CHECK(v.which() == npos);
    CHECK(v.target() == nullptr);
    CHECK(v.target_type() == typeid(void));

#if EGGS_CXX11_HAS_CONSTEXPR
    SECTION("constexpr")
    {
        constexpr eggs::variant<int, Constexpr> v(nullvariant);
        constexpr bool vb = bool(v);
        constexpr std::size_t vw = v.which();
        constexpr void const* vt = v.target();
        constexpr std::type_info const& vtt = v.target_type();
    }
#endif
}
