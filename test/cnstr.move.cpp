// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>
#include <type_traits>

#include <eggs/variant/detail/config/prefix.hpp>
#include <eggs/variant/detail/utility.hpp>

using eggs::variants::detail::move;

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "constexpr.hpp"

struct MovableOnly
{
    std::string x;

    MovableOnly() : x() {}
    MovableOnly(std::string const& s) : x(s) {}
    MovableOnly(MovableOnly&& rhs) : x(::move(rhs.x)) {};
    MovableOnly& operator=(MovableOnly&& rhs) { x = ::move(rhs.x); return *this; };
};

#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
template <bool NoThrow>
struct NoThrowMoveConstructible
{
    NoThrowMoveConstructible() {}
    NoThrowMoveConstructible(NoThrowMoveConstructible&&) noexcept(NoThrow) {}
};
#endif

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
struct NonCopyConstructible
{
    NonCopyConstructible() {}
    NonCopyConstructible(NonCopyConstructible const&) = delete;
    NonCopyConstructible& operator=(NonCopyConstructible const&) { return *this; }; // not trivially copyable
};

struct NonCopyConstructibleTrivial
{
    NonCopyConstructibleTrivial() {}
    NonCopyConstructibleTrivial(NonCopyConstructibleTrivial const&) = delete;
    NonCopyConstructibleTrivial& operator=(NonCopyConstructibleTrivial const&) = default;
};

#  if !EGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE || !EGGS_CXX11_STD_HAS_IS_TRIVIALLY_DESTRUCTIBLE
namespace eggs { namespace variants { namespace detail
{
    template <> struct is_trivially_copyable<NonCopyConstructibleTrivial> : std::true_type {};
    template <> struct is_trivially_destructible<NonCopyConstructibleTrivial> : std::true_type {};
}}}
#  endif
#endif

TEST_CASE("variant<Ts...>::variant(variant<Ts...>&&)", "[variant.cnstr]")
{
    eggs::variant<int, MovableOnly> v1(42);

    REQUIRE(bool(v1) == true);
    REQUIRE(v1.which() == 0u);
    REQUIRE(*v1.target<int>() == 42);

    eggs::variant<int, MovableOnly> v2(::move(v1));

    CHECK(bool(v1) == true);
    CHECK(bool(v2) == true);
    CHECK(v2.which() == v1.which());
    REQUIRE(v1.target<int>() != nullptr);
    CHECK(*v1.target<int>() == 42);

    // list-initialization
    {
        eggs::variant<int, std::string> v = {};

        CHECK(bool(v) == false);
        CHECK(v.which() == eggs::variant_npos);
        CHECK(v.target() == nullptr);

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(void));
#endif
    }

    // trivially_copyable
    {
        eggs::variant<int, float> v1(42);

        REQUIRE(bool(v1) == true);
        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

#if EGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE
        CHECK(std::is_trivially_copyable<decltype(v1)>::value == true);
#endif

        eggs::variant<int, float> v2(::move(v1));

        CHECK(bool(v1) == true);
        CHECK(bool(v2) == true);
        CHECK(v2.which() == v1.which());
        REQUIRE(v1.target<int>() != nullptr);
        CHECK(*v1.target<int>() == 42);
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);
    }

#if EGGS_CXX14_HAS_CONSTEXPR
    // constexpr
    {
        struct test { static constexpr int call()
        {
            eggs::variant<int, ConstexprTrivial> v1(ConstexprTrivial(42));
            eggs::variant<int, ConstexprTrivial> v2(::move(v1));
            return 0;
        }};
        constexpr int c = test::call();
    }
#endif

#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
    // noexcept
    {
        REQUIRE(
            std::is_move_constructible<
                eggs::variant<int, NoThrowMoveConstructible<true>>
            >::value);
        CHECK(
            std::is_nothrow_move_constructible<
                eggs::variant<int, NoThrowMoveConstructible<true>>
            >::value);

        REQUIRE(
            std::is_move_constructible<
                eggs::variant<int, NoThrowMoveConstructible<false>>
            >::value);
        CHECK(
            !std::is_nothrow_move_constructible<
                eggs::variant<int, NoThrowMoveConstructible<false>>
            >::value);
    }
#endif

    // sfinae
    {
#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
        CHECK(
            !std::is_move_constructible<
                eggs::variant<NonCopyConstructible>
            >::value);
        CHECK(
            !std::is_move_constructible<
                eggs::variant<NonCopyConstructibleTrivial>
            >::value);
#endif
    }
}

TEST_CASE("variant<>::variant(variant<>&&)", "[variant.cnstr]")
{
    eggs::variant<> v1;

    REQUIRE(bool(v1) == false);
    REQUIRE(v1.which() == eggs::variant_npos);

    eggs::variant<> v2(::move(v1));

    CHECK(bool(v1) == false);
    CHECK(bool(v2) == false);
    CHECK(v2.which() == v1.which());

    CHECK(noexcept(eggs::variant<>(::move(v1))) == true);

    // list-initialization
    {
        eggs::variant<> v = {};

        CHECK(bool(v) == false);
        CHECK(v.which() == eggs::variant_npos);
        CHECK(v.target() == nullptr);

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(void));
#endif
    }

#if EGGS_CXX14_HAS_CONSTEXPR
    // constexpr
    {
        struct test { static constexpr int call()
        {
            eggs::variant<> v1;
            eggs::variant<> v2(::move(v1));
            return 0;
        }};
        constexpr int c = test::call();
    }
#endif
}
