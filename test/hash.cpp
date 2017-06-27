// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
template <typename ...Ts>
struct _void
{
    using type = void;
};

template <typename T, typename Enable = void>
struct has_hash
  : std::false_type
{};

template <typename T>
struct has_hash<
    T, typename _void<
        decltype(std::hash<T>{}(std::declval<T const&>()))
    >::type
> : std::true_type
{};

template <bool NoThrow>
struct NoThrowHashDefaultConstructible {};
namespace std
{
    template <bool NoThrow>
    struct hash<NoThrowHashDefaultConstructible<NoThrow>>
    {
        hash() noexcept(NoThrow) {}
        std::size_t operator()(NoThrowHashDefaultConstructible<NoThrow> const&) const noexcept { return 0; }
    };
}

template <bool NoThrow>
struct NoThrowHashable {};
namespace std
{
    template <bool NoThrow>
    struct hash<NoThrowHashable<NoThrow>>
    {
        hash() noexcept {}
        std::size_t operator()(NoThrowHashable<NoThrow> const&) const noexcept(NoThrow) { return 0; }
    };
}

template <typename T, bool HasHash = has_hash<T>::value>
struct has_nothrow_hash
{
    EGGS_CXX11_STATIC_CONSTEXPR bool value =
        noexcept(std::hash<T>{}(std::declval<T const&>()));
};

template <typename T>
struct has_nothrow_hash<T, false>
{
    EGGS_CXX11_STATIC_CONSTEXPR bool value = false;
};
#endif

struct NonHashable {};
// explicitly disable (pre C++17)
namespace std
{
    template <> struct hash<NonHashable> {};
}

TEST_CASE("std::hash<variant<Ts...>>", "[variant.hash]")
{
    eggs::variant<int, std::string> const v(42);

    REQUIRE(v.which() == 0u);
    REQUIRE(*v.target<int>() == 42);

    std::hash<eggs::variant<int, std::string>> variant_hasher;
    std::hash<int> int_hasher;

    CHECK(variant_hasher(v) == int_hasher(42));

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
    // noexcept
    {
        REQUIRE((
            has_hash<
                eggs::variant<NoThrowHashDefaultConstructible<true>>
            >::value));
        CHECK((
            has_nothrow_hash<
                eggs::variant<NoThrowHashDefaultConstructible<true>>
            >::value));

        REQUIRE((
            has_hash<
                eggs::variant<NoThrowHashDefaultConstructible<false>>
            >::value));
        CHECK((
            !has_nothrow_hash<
                eggs::variant<NoThrowHashDefaultConstructible<false>>
            >::value));

        REQUIRE((
            has_hash<
                eggs::variant<NoThrowHashable<true>>
            >::value));
        CHECK((
            has_nothrow_hash<
                eggs::variant<NoThrowHashable<true>>
            >::value));

        REQUIRE((
            has_hash<
                eggs::variant<NoThrowHashable<false>>
            >::value));
        CHECK((
            !has_nothrow_hash<
                eggs::variant<NoThrowHashable<false>>
            >::value));
    }

    // sfinae
    {
        CHECK((
            !std::is_constructible<
                std::hash<eggs::variant<NonHashable>>
            >::value));
        CHECK((
            !std::is_copy_constructible<
                std::hash<eggs::variant<NonHashable>>
            >::value));
        CHECK((
            !std::is_move_constructible<
                std::hash<eggs::variant<NonHashable>>
            >::value));
        CHECK((
            !std::is_copy_assignable<
                std::hash<eggs::variant<NonHashable>>
            >::value));
        CHECK((
            !std::is_move_assignable<
                std::hash<eggs::variant<NonHashable>>
            >::value));
        CHECK((
            !has_hash<
                eggs::variant<NonHashable>
            >::value));
    }
#endif
}
