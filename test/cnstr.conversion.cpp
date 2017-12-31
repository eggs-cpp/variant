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

template <typename T>
struct Explicit
{
    T x;

    template <
        typename U
      , typename Enable = typename std::enable_if<
            std::is_constructible<T, U&&>::value
        >::type
    >
    explicit Explicit(U&& x) : x(std::forward<U>(x)) {}
};

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
struct AnyConstructible
{
    template <typename T>
    AnyConstructible(T&&){}
};

struct NoneConstructible
{
    NoneConstructible() = delete;
};

struct WeirdConstructor
{
    WeirdConstructor(int) {}
    explicit WeirdConstructor(long) = delete;
};
#endif

TEST_CASE("variant<Ts...>::variant(T&&)", "[variant.cnstr]")
{
    eggs::variant<int, std::string> v(42);

    CHECK(bool(v) == true);
    CHECK(v.which() == 0u);
    REQUIRE(v.target<int>() != nullptr);
    CHECK(*v.target<int>() == 42);

#if EGGS_CXX98_HAS_RTTI
    CHECK(v.target_type() == typeid(int));
#endif

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<int, Constexpr> v(Constexpr(42));

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

    // conversion
    {
        eggs::variant<int, std::string> v("42");

        CHECK(bool(v) == true);
        CHECK(v.which() == 1u);
        REQUIRE(v.target<std::string>() != nullptr);
        CHECK(*v.target<std::string>() == "42");

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(std::string));
#endif

        // implicit conversion
        {
            eggs::variant<int, std::string> v = "42";

            CHECK(bool(v) == true);
            CHECK(v.which() == 1u);
            REQUIRE(v.target<std::string>() != nullptr);
            CHECK(*v.target<std::string>() == "42");

#if EGGS_CXX98_HAS_RTTI
            CHECK(v.target_type() == typeid(std::string));
#endif
        }

        // explicit conversion
        {
            eggs::variant<int, Explicit<std::string>> v("42");

            CHECK(bool(v) == true);
            CHECK(v.which() == 1u);
            REQUIRE(v.target<Explicit<std::string>>() != nullptr);
            CHECK(v.target<Explicit<std::string>>()->x == "42");

#if EGGS_CXX98_HAS_RTTI
            CHECK(v.target_type() == typeid(Explicit<std::string>));
#endif
        }
    }

    // sfinae
    {
        CHECK(
            !std::is_constructible<
                eggs::variant<int>, std::string
            >::value);
        CHECK(
            !std::is_constructible<
                eggs::variant<int, int>, int
            >::value);
        CHECK(
            !std::is_constructible<
                eggs::variant<int, int const>, int
            >::value);
        CHECK(
            !std::is_constructible<
                eggs::variant<Explicit<int>, Explicit<int>>, int
            >::value);
        CHECK(
            !std::is_constructible<
                eggs::variant<Explicit<int>, Explicit<int const>>, int
            >::value);
        CHECK(
            !std::is_constructible<
                eggs::variant<int, int, Explicit<int>>, int
            >::value);
        CHECK(
            !std::is_constructible<
                eggs::variant<int, int, Explicit<long>>, long
            >::value);
#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
        CHECK(
            !std::is_constructible<
                eggs::variant<NoneConstructible, AnyConstructible>,
                eggs::variants::in_place_index_t<0>
            >::value);
        CHECK(
            !std::is_constructible<
                eggs::variant<NoneConstructible, AnyConstructible>,
                eggs::variants::in_place_type_t<NoneConstructible>
            >::value);
        CHECK(
            !std::is_constructible<
                eggs::variant<NoneConstructible, Explicit<AnyConstructible>>,
                eggs::variants::in_place_index_t<0>
            >::value);
        CHECK(
            !std::is_constructible<
                eggs::variant<NoneConstructible, Explicit<AnyConstructible>>,
                eggs::variants::in_place_type_t<NoneConstructible>
            >::value);
        CHECK(
            !std::is_constructible<
                eggs::variant<WeirdConstructor>, long
            >::value);
#endif

        CHECK(
            !std::is_convertible<
                int, eggs::variant<Explicit<int>>
            >::value);
    }
}
