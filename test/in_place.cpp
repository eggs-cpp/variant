// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <cstddef>
#include <type_traits>
#include <utility>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

struct fun_index
{
    template <std::size_t I>
    eggs::variants::in_place_index_t<I>
    operator()(eggs::variants::in_place_index_t<I> tag)
    {
        return tag;
    }
};

struct fun_type
{
    template <typename T>
    eggs::variants::in_place_type_t<T>
    operator()(eggs::variants::in_place_type_t<T> tag)
    {
        return tag;
    }
};

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
template <typename ...Ts>
struct _void
{
    using type = void;
};

template <typename T, typename Arg, typename Enable = void>
struct is_callable
  : std::false_type
{};

template <typename T, typename Arg>
struct is_callable<
    T, Arg, typename _void<
        decltype(std::declval<T>()(std::declval<Arg>()))
    >::type
> : std::true_type
{};
#endif

TEST_CASE("in_place_index_t<I>", "[variant.in_place]")
{
#if !defined(__GNUC__)
    // triggers https://gcc.gnu.org/bugzilla/show_bug.cgi?id=81059
    eggs::variants::in_place_index_t<0> tag = eggs::variants::in_place<0>;

    CHECK((tag == eggs::variants::in_place<0>));

    tag = fun_index{}(eggs::variants::in_place<0>);

    CHECK((tag == eggs::variants::in_place<0>));
#endif

    using decayed_tag = std::decay<eggs::variants::in_place_index_t<0>>::type;

    CHECK((std::is_same<decayed_tag, eggs::variants::in_place_index_t<0>>::value));

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
    // nullptr
    {
        CHECK((
            !is_callable<
                fun_index, std::nullptr_t
            >::value));
    }
#endif
}

TEST_CASE("in_place_type_t<T>", "[variant.in_place]")
{
#if !defined(__GNUC__)
    // triggers https://gcc.gnu.org/bugzilla/show_bug.cgi?id=81059
    eggs::variants::in_place_type_t<int> tag = eggs::variants::in_place<int>;

    CHECK((tag == eggs::variants::in_place<int>));

    tag = fun_type{}(eggs::variants::in_place<int>);

    CHECK((tag == eggs::variants::in_place<int>));
#endif

    using decayed_tag = std::decay<eggs::variants::in_place_type_t<int>>::type;

    CHECK((std::is_same<decayed_tag, eggs::variants::in_place_type_t<int>>::value));

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
    // nullptr
    {
        CHECK((
            !is_callable<
                fun_type, std::nullptr_t
            >::value));
    }
#endif
}
