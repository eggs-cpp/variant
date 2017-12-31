// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>
#include <type_traits>
#include <utility>

#include <eggs/variant/detail/config/prefix.hpp>

#include "catch.hpp"
#include "constexpr.hpp"
#include "dtor.hpp"

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
template <typename ...Ts>
struct _void
{
    using type = void;
};

template <typename T, typename Enable = void>
struct has_swap
  : std::false_type
{};

template <typename T>
struct has_swap<
    T, typename _void<
        decltype(swap(std::declval<T>(), std::declval<T>()))
    >::type
> : std::true_type
{};
#endif

#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
template <bool NoThrow>
struct NoThrowMoveConstructible
{
    NoThrowMoveConstructible() {}
    NoThrowMoveConstructible(NoThrowMoveConstructible&&) noexcept(NoThrow) {}
    NoThrowMoveConstructible& operator=(NoThrowMoveConstructible&&) noexcept { return *this; };
};
template <bool NoThrow>
void swap(NoThrowMoveConstructible<NoThrow>&, NoThrowMoveConstructible<NoThrow>&) noexcept {}

template <bool NoThrow>
struct NoThrowMoveSwappable
{
    NoThrowMoveSwappable() {}
    NoThrowMoveSwappable(NoThrowMoveSwappable&&) noexcept {}
    NoThrowMoveSwappable& operator=(NoThrowMoveSwappable&&) noexcept { return *this; };
};
template <bool NoThrow>
void swap(NoThrowMoveSwappable<NoThrow>&, NoThrowMoveSwappable<NoThrow>&) noexcept(NoThrow) {}
#endif
struct NonAssignable
{
    NonAssignable() {}
    NonAssignable(NonAssignable&&) {}; // not trivially copyable
    NonAssignable& operator=(NonAssignable const&) = delete;
    ~NonAssignable() {}
};
void swap(NonAssignable&, NonAssignable&) {}

struct NonAssignableTrivial
{
    NonAssignableTrivial() {}
    NonAssignableTrivial(NonAssignableTrivial&&) = default;
    NonAssignableTrivial& operator=(NonAssignableTrivial const&) = delete;
};
void swap(NonAssignableTrivial&, NonAssignableTrivial&) {}

#  if !EGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE || !EGGS_CXX11_STD_HAS_IS_TRIVIALLY_DESTRUCTIBLE
namespace eggs { namespace variants { namespace detail
{
    template <> struct is_trivially_copyable<NonAssignableTrivial> : std::true_type {};
    template <> struct is_trivially_destructible<NonAssignableTrivial> : std::true_type {};
}}}
#  endif

struct NonSwappable
{
    NonSwappable() {}
    NonSwappable(NonSwappable&&) {}
    NonSwappable& operator=(NonSwappable const&) { return *this; };
};
void swap(NonSwappable&, NonSwappable&) = delete;

struct StdAssociateSwappable : std::true_type {};

#if !EGGS_CXX17_STD_HAS_SWAPPABLE_TRAITS
namespace std
{
    namespace swappable_traits
    {
        using ::eggs::variants::detail::is_swappable;
        using ::eggs::variants::detail::is_nothrow_swappable;
    }
    using namespace swappable_traits;
}
#endif

TEST_CASE("variant<Ts...>::swap(variant<Ts...>&)", "[variant.swap]")
{
    // empty source
    {
        eggs::variant<int, std::string> v1;

        REQUIRE(v1.which() == eggs::variant_npos);

        eggs::variant<int, std::string> v2(42);

        REQUIRE(v2.which() == 0u);
        REQUIRE(*v2.target<int>() == 42);

        v2.swap(v1);

        CHECK(v1.which() == 0u);
        REQUIRE(v1.target<int>() != nullptr);
        CHECK(*v1.target<int>() == 42);
        CHECK(v2.which() == eggs::variant_npos);

        // dtor
        {
            eggs::variant<int, Dtor> v1;
            eggs::variant<int, Dtor> v2(eggs::variants::in_place<Dtor>);

            REQUIRE(v1.which() == eggs::variant_npos);
            REQUIRE(v2.which() == 1u);
            REQUIRE(Dtor::calls == 0u);

            v1.swap(v2);

            CHECK(v1.which() == 1u);
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
                v2.swap(v1);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    // empty target
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> v2;

        REQUIRE(v2.which() == eggs::variant_npos);

        v2.swap(v1);

        CHECK(v1.which() == eggs::variant_npos);
        CHECK(v2.which() == 0u);
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);

        // dtor
        {
            eggs::variant<int, Dtor> v1(eggs::variants::in_place<Dtor>);
            eggs::variant<int, Dtor> v2;

            REQUIRE(v1.which() == 1u);
            REQUIRE(v2.which() == eggs::variant_npos);
            REQUIRE(Dtor::calls == 0u);

            v1.swap(v2);

            CHECK(v1.which() == eggs::variant_npos);
            CHECK(v2.which() == 1u);
            CHECK(Dtor::calls == 1u);
        }
        Dtor::calls = 0u;

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v1(ConstexprTrivial(42));
                eggs::variant<int, ConstexprTrivial> v2;
                v2.swap(v1);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    // same target
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> v2(43);

        REQUIRE(v2.which() == v1.which());
        REQUIRE(*v2.target<int>() == 43);

        v2.swap(v1);

        CHECK(v1.which() == v1.which());
        REQUIRE(v1.target<int>() != nullptr);
        CHECK(*v1.target<int>() == 43);
        CHECK(v2.which() == 0u);
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);

        // dtor
        {
            eggs::variant<int, Dtor> v1(eggs::variants::in_place<Dtor>);
            eggs::variant<int, Dtor> v2(eggs::variants::in_place<Dtor>);

            REQUIRE(v1.which() == 1u);
            REQUIRE(v2.which() == 1u);
            REQUIRE(Dtor::calls == 0u);

            v1.swap(v2);

            CHECK(v1.which() == 1u);
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
                v2.swap(v1);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    // different target
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> v2(std::string(""));

        REQUIRE(v2.which() == 1u);
        REQUIRE(*v2.target<std::string>() == "");

        v2.swap(v1);

        CHECK(v1.which() == 1u);
        REQUIRE(v1.target<std::string>() != nullptr);
        CHECK(*v1.target<std::string>() == "");
        CHECK(v2.which() == 0u);
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);

        // dtor
        {
            eggs::variant<int, Dtor> v1(42);
            eggs::variant<int, Dtor> v2(eggs::variants::in_place<Dtor>);

            REQUIRE(v1.which() == 0u);
            REQUIRE(v2.which() == 1u);
            REQUIRE(Dtor::calls == 0u);

            v1.swap(v2);

            CHECK(v1.which() == 1u);
            CHECK(v2.which() == 0u);
            CHECK(Dtor::calls == 1u);
        }
        Dtor::calls = 0u;

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v1(ConstexprTrivial(42));
                eggs::variant<int, ConstexprTrivial> v2(43);
                v2.swap(v1);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
    // noexcept
    {
        REQUIRE(
            std::is_swappable<
                eggs::variant<int, NoThrowMoveConstructible<true>>
            >::value);
        CHECK(
            std::is_nothrow_swappable<
                eggs::variant<int, NoThrowMoveConstructible<true>>
            >::value);

        REQUIRE(
            std::is_swappable<
                eggs::variant<int, NoThrowMoveConstructible<false>>
            >::value);
        CHECK(
            !std::is_nothrow_swappable<
                eggs::variant<int, NoThrowMoveConstructible<false>>
            >::value);

        REQUIRE(
            std::is_swappable<
                eggs::variant<int, NoThrowMoveSwappable<true>>
            >::value);
        CHECK(
            std::is_nothrow_swappable<
                eggs::variant<int, NoThrowMoveSwappable<true>>
            >::value);

        REQUIRE(
            std::is_swappable<
                eggs::variant<int, NoThrowMoveSwappable<false>>
            >::value);
        CHECK(
            !std::is_nothrow_swappable<
                eggs::variant<int, NoThrowMoveSwappable<false>>
            >::value);
    }
#endif

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
    // sfinae
    {
        eggs::variant<StdAssociateSwappable> v1, v2;
        eggs::variants::swap(v1, v2);

        CHECK(
            !has_swap<
                eggs::variant<NonSwappable>
            >::value);
    }
#endif
}

TEST_CASE("variant<NonAssignable>::swap(variant<...>&)", "[variant.swap]")
{
    eggs::variant<int, NonAssignable> v1(42);

    REQUIRE(v1.which() == 0u);

    eggs::variant<int, NonAssignable> v2(NonAssignable{});

    REQUIRE(v2.which() == 1u);

    v2.swap(v1);

    CHECK(v1.which() == 1u);
    CHECK(v2.which() == 0u);

    // trivially_copyable
    {
        eggs::variant<int, NonAssignableTrivial> v1(42);

        REQUIRE(v1.which() == 0u);

        eggs::variant<int, NonAssignableTrivial> v2(NonAssignableTrivial{});

        REQUIRE(v2.which() == 1u);

        v2.swap(v1);

        CHECK(v1.which() == 1u);
        CHECK(v2.which() == 0u);
    }
}

TEST_CASE("variant<>::swap(variant<>&)", "[variant.swap]")
{
    eggs::variant<> v1;

    REQUIRE(v1.which() == eggs::variant_npos);

    eggs::variant<> v2;

    REQUIRE(v2.which() == eggs::variant_npos);

    v2.swap(v1);

    CHECK(v1.which() == eggs::variant_npos);
    CHECK(v2.which() == eggs::variant_npos);

    CHECK(noexcept(v2.swap(v1)) == true);

#if EGGS_CXX14_HAS_CONSTEXPR
    // constexpr
    {
        struct test { static constexpr int call()
        {
            eggs::variant<> v1;
            eggs::variant<> v2;
            v2.swap(v1);
            return 0;
        }};
        constexpr int c = test::call();
    }
#endif
}
