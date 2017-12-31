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

#include "catch.hpp"
#include "constexpr.hpp"
#include "dtor.hpp"
#include "throw.hpp"

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
struct NoThrowMoveAssignable
{
    NoThrowMoveAssignable() {}
    NoThrowMoveAssignable(NoThrowMoveAssignable&&) noexcept {}
    NoThrowMoveAssignable& operator=(NoThrowMoveAssignable&&) noexcept(NoThrow) { return *this; };
};

template <bool NoThrow>
struct NoThrowMoveConstructible
{
    NoThrowMoveConstructible() {}
    NoThrowMoveConstructible(NoThrowMoveConstructible&&) noexcept(NoThrow) {}
    NoThrowMoveConstructible& operator=(NoThrowMoveConstructible&&) noexcept { return *this; };
};
#endif

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
struct NonCopyAssignable
{
    NonCopyAssignable() {}
    NonCopyAssignable(NonCopyAssignable const&) {}; // not trivially copyable
    NonCopyAssignable& operator=(NonCopyAssignable const&) = delete;
};

struct NonCopyConstructible
{
    NonCopyConstructible() {}
    NonCopyConstructible(NonCopyConstructible const&) = delete;
    NonCopyConstructible& operator=(NonCopyConstructible const&) { return *this; }; // not trivially copyable
};

struct NonCopyAssignableTrivial
{
    NonCopyAssignableTrivial() {}
    NonCopyAssignableTrivial(NonCopyAssignableTrivial const&) = default;
    NonCopyAssignableTrivial& operator=(NonCopyAssignableTrivial const&) = delete;
};

#  if !EGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE || !EGGS_CXX11_STD_HAS_IS_TRIVIALLY_DESTRUCTIBLE
namespace eggs { namespace variants { namespace detail
{
    template <> struct is_trivially_copyable<NonCopyAssignableTrivial> : std::true_type {};
    template <> struct is_trivially_destructible<NonCopyAssignableTrivial> : std::true_type {};
}}}
#  endif
#endif

TEST_CASE("variant<Ts...>::operator=(variant<Ts...>&&)", "[variant.assign]")
{
    // empty source
    {
        eggs::variant<int, MovableOnly> v1;

        REQUIRE(bool(v1) == false);
        REQUIRE(v1.which() == eggs::variant_npos);

        eggs::variant<int, MovableOnly> v2(42);

        REQUIRE(bool(v2) == true);
        REQUIRE(v2.which() == 0u);
        REQUIRE(*v2.target<int>() == 42);

        v2 = ::move(v1);

        CHECK(bool(v1) == false);
        CHECK(bool(v2) == false);
        CHECK(v2.which() == v1.which());

        // dtor
        {
            eggs::variant<int, Dtor> v1;
            eggs::variant<int, Dtor> v2(eggs::variants::in_place<Dtor>);

            REQUIRE(v1.which() == eggs::variant_npos);
            REQUIRE(v2.which() == 1u);
            REQUIRE(Dtor::calls == 0u);

            v2 = ::move(v1);

            CHECK(v2.which() == eggs::variant_npos);
            CHECK(Dtor::calls == 1u);
        }
        Dtor::calls = 0u;

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v1;
                eggs::variant<int, ConstexprTrivial> v2(ConstexprTrivial(42));
                v2 = ::move(v1);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    // empty target
    {
        eggs::variant<int, MovableOnly> v1(42);

        REQUIRE(bool(v1) == true);
        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, MovableOnly> v2;

        REQUIRE(bool(v2) == false);
        REQUIRE(v2.which() == eggs::variant_npos);

        v2 = ::move(v1);

        CHECK(bool(v1) == true);
        CHECK(bool(v2) == true);
        CHECK(v2.which() == v1.which());
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v1(ConstexprTrivial(42));
                eggs::variant<int, ConstexprTrivial> v2;
                v2 = ::move(v1);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    // same target
    {
        eggs::variant<int, MovableOnly> v1(42);

        REQUIRE(bool(v1) == true);
        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, MovableOnly> v2(43);

        REQUIRE(bool(v2) == true);
        REQUIRE(v2.which() == v1.which());
        REQUIRE(*v2.target<int>() == 43);

        v2 = ::move(v1);

        CHECK(bool(v1) == true);
        CHECK(bool(v2) == true);
        CHECK(v2.which() == v1.which());
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);

        // dtor
        {
            eggs::variant<int, Dtor> v1(eggs::variants::in_place<Dtor>);
            eggs::variant<int, Dtor> v2(eggs::variants::in_place<Dtor>);

            REQUIRE(v1.which() == 1u);
            REQUIRE(v2.which() == 1u);
            REQUIRE(Dtor::calls == 0u);

            v2 = ::move(v1);

            CHECK(v2.which() == 1u);
            CHECK(Dtor::calls == 0u);
        }
        Dtor::calls = 0u;

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v1(ConstexprTrivial(42));
                eggs::variant<int, ConstexprTrivial> v2(ConstexprTrivial(43));
                v2 = ::move(v1);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    // different target
    {
        eggs::variant<int, MovableOnly> v1(42);

        REQUIRE(bool(v1) == true);
        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, MovableOnly> v2(MovableOnly(""));

        REQUIRE(bool(v2) == true);
        REQUIRE(v2.which() == 1u);
        REQUIRE(v2.target<MovableOnly>()->x == "");

        v2 = ::move(v1);

        CHECK(bool(v1) == true);
        CHECK(bool(v2) == true);
        CHECK(v2.which() == v1.which());
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);

        // dtor
        {
            eggs::variant<int, Dtor> v1(42);
            eggs::variant<int, Dtor> v2(eggs::variants::in_place<Dtor>);

            REQUIRE(v1.which() == 0u);
            REQUIRE(v2.which() == 1u);
            REQUIRE(Dtor::calls == 0u);

            v2 = ::move(v1);

            CHECK(v2.which() == 0u);
            CHECK(Dtor::calls == 1u);
        }
        Dtor::calls = 0u;

#if EGGS_CXX98_HAS_EXCEPTIONS
        // exception-safety
        {
            eggs::variant<Dtor, Throw> v1;
            v1.emplace<1>();

            REQUIRE(bool(v1) == true);
            REQUIRE(v1.which() == 1u);

            eggs::variant<Dtor, Throw> v2;
            v2.emplace<0>();

            REQUIRE(bool(v2) == true);
            REQUIRE(v2.which() == 0u);
            REQUIRE(Dtor::calls == 0u);

            CHECK_THROWS(v2 = ::move(v1));

            CHECK(bool(v1) == true);
            CHECK(bool(v2) == false);
            CHECK(v1.which() == 1u);
            CHECK(v2.which() == eggs::variant_npos);
            CHECK(Dtor::calls == 1u);
        }
        Dtor::calls = 0u;
#endif

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v1(ConstexprTrivial(42));
                eggs::variant<int, ConstexprTrivial> v2(43);
                v2 = ::move(v1);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    // list-initialization
    {
        eggs::variant<int, std::string> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == eggs::variant_npos);

        v = {};

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

#if EGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE
        CHECK(std::is_trivially_copyable<decltype(v1)>::value == true);
#endif

        REQUIRE(bool(v1) == true);
        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, float> v2(42.f);

        REQUIRE(bool(v2) == true);
        REQUIRE(v2.which() == 1u);
        REQUIRE(*v2.target<float>() == 42.f);

        v2 = ::move(v1);

        CHECK(bool(v1) == true);
        CHECK(bool(v2) == true);
        CHECK(v2.which() == v1.which());
        REQUIRE(v1.target<int>() != nullptr);
        CHECK(*v1.target<int>() == 42);
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);
    }

#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
    // noexcept
    {
        REQUIRE(
            std::is_move_assignable<
                eggs::variant<int, NoThrowMoveAssignable<true>>
            >::value);
        CHECK(
            std::is_nothrow_move_assignable<
                eggs::variant<int, NoThrowMoveAssignable<true>>
            >::value);

        REQUIRE(
            std::is_move_assignable<
                eggs::variant<int, NoThrowMoveAssignable<false>>
            >::value);
        CHECK(
            !std::is_nothrow_move_assignable<
                eggs::variant<int, NoThrowMoveAssignable<false>>
            >::value);

        REQUIRE(
            std::is_move_assignable<
                eggs::variant<int, NoThrowMoveConstructible<true>>
            >::value);
        CHECK(
            std::is_nothrow_move_assignable<
                eggs::variant<int, NoThrowMoveConstructible<true>>
            >::value);

        REQUIRE(
            std::is_move_assignable<
                eggs::variant<int, NoThrowMoveConstructible<false>>
            >::value);
        CHECK(
            !std::is_nothrow_move_assignable<
                eggs::variant<int, NoThrowMoveConstructible<false>>
            >::value);
    }
#endif

    // sfinae
    {
#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
        CHECK(
            !std::is_move_assignable<
                eggs::variant<NonCopyAssignable>
            >::value);
        CHECK(
            !std::is_move_assignable<
                eggs::variant<NonCopyConstructible>
            >::value);
        CHECK(
            !std::is_move_assignable<
                eggs::variant<NonCopyAssignableTrivial>
            >::value);
#endif
    }
}

TEST_CASE("variant<>::operator=(variant<>&&)", "[variant.assign]")
{
    eggs::variant<> v1;

    REQUIRE(bool(v1) == false);
    REQUIRE(v1.which() == eggs::variant_npos);

    eggs::variant<> v2;

    REQUIRE(bool(v2) == false);
    REQUIRE(v2.which() == eggs::variant_npos);

    v2 = ::move(v1);

    CHECK(bool(v1) == false);
    CHECK(bool(v2) == false);
    CHECK(v2.which() == v1.which());

    CHECK(noexcept(v2 = ::move(v1)) == true);

    // list-initialization
    {
        eggs::variant<> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == eggs::variant_npos);

        v = {};

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
            eggs::variant<> v2;
            v2 = ::move(v1);
            return 0;
        }};
        constexpr int c = test::call();
    }
#endif
}
