// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

struct fun
{
    std::size_t nonconst_lvalue, const_lvalue, rvalue;

    fun() : nonconst_lvalue{0}, const_lvalue{0}, rvalue{0} {}

    using result_type = std::size_t;

    template <typename T>
    std::size_t operator()(T&)
    {
        return ++nonconst_lvalue;
    }

    template <typename T>
    std::size_t operator()(T const&)
    {
        return ++const_lvalue;
    }

    template <typename T>
    std::size_t operator()(T&&)
    {
        return ++rvalue;
    }
};

TEST_CASE("apply<R>(F&&, variant<Ts...>&)", "[variant.apply]")
{
    eggs::variant<int, std::string> v(42);

    REQUIRE(v.which() == 0);
    REQUIRE(*v.target<int>() == 42);

    fun f;
    std::size_t calls = eggs::variants::apply<std::size_t>(f, v);

    REQUIRE(f.nonconst_lvalue == 1);
    REQUIRE(calls == f.nonconst_lvalue);

    SECTION("throws")
    {
        try
        {
            eggs::variant<> empty;

            REQUIRE(empty.which() == npos);

            eggs::variants::apply<void>(fun{}, empty);
        } catch (eggs::variants::bad_variant_access&) {
            REQUIRE(true);
        } catch (...) {
            REQUIRE(false);
        }
    }
}

TEST_CASE("apply<R>(F&&, variant<Ts...> const&)", "[variant.apply]")
{
    eggs::variant<int, std::string> const v(42);

    REQUIRE(v.which() == 0);
    REQUIRE(*v.target<int>() == 42);

    fun f;
    std::size_t calls = eggs::variants::apply<std::size_t>(f, v);

    REQUIRE(f.const_lvalue == 1);
    REQUIRE(calls == f.const_lvalue);

    SECTION("throws")
    {
        try
        {
            eggs::variant<> const empty;

            REQUIRE(empty.which() == npos);

            eggs::variants::apply<void>(fun{}, empty);
        } catch (eggs::variants::bad_variant_access&) {
            REQUIRE(true);
        } catch (...) {
            REQUIRE(false);
        }
    }
}

TEST_CASE("apply<R>(F&&, variant<Ts...>&&)", "[variant.apply]")
{
    eggs::variant<int, std::string> v(42);

    REQUIRE(v.which() == 0);
    REQUIRE(*v.target<int>() == 42);

    fun f;
    std::size_t calls = eggs::variants::apply<std::size_t>(f, std::move(v));

    REQUIRE(f.rvalue == 1);
    REQUIRE(calls == f.rvalue);

    SECTION("throws")
    {
        try
        {
            eggs::variant<> empty;

            REQUIRE(empty.which() == npos);

            eggs::variants::apply<void>(fun{}, std::move(empty));
        } catch (eggs::variants::bad_variant_access&) {
            REQUIRE(true);
        } catch (...) {
            REQUIRE(false);
        }
    }
}

TEST_CASE("apply(F&&, variant<Ts...>&)", "[variant.apply]")
{
    eggs::variant<int, std::string> v(42);

    REQUIRE(v.which() == 0);
    REQUIRE(*v.target<int>() == 42);

    fun f;
    std::size_t calls = eggs::variants::apply(f, v);

    REQUIRE(f.nonconst_lvalue == 1);
    REQUIRE(calls == f.nonconst_lvalue);
}

TEST_CASE("apply(F&&, variant<Ts...> const&)", "[variant.apply]")
{
    eggs::variant<int, std::string> const v(42);

    REQUIRE(v.which() == 0);
    REQUIRE(*v.target<int>() == 42);

    fun f;
    std::size_t calls = eggs::variants::apply(f, v);

    REQUIRE(f.const_lvalue == 1);
    REQUIRE(calls == f.const_lvalue);
}

TEST_CASE("apply(F&&, variant<Ts...>&&)", "[variant.apply]")
{
    eggs::variant<int, std::string> v(42);

    REQUIRE(v.which() == 0);
    REQUIRE(*v.target<int>() == 42);

    fun f;
    std::size_t calls = eggs::variants::apply(f, std::move(v));

    REQUIRE(f.rvalue == 1);
    REQUIRE(calls == f.rvalue);
}

struct fun2
{
    std::size_t nonconst_lvalue, const_lvalue, rvalue;

    fun2() : nonconst_lvalue{0}, const_lvalue{0}, rvalue{0} {}

    using result_type = std::size_t;

    template <typename T, typename U>
    std::size_t operator()(T&, U&)
    {
        return ++nonconst_lvalue;
    }

    template <typename T, typename U>
    std::size_t operator()(T const&, U const&)
    {
        return ++const_lvalue;
    }

    template <typename T, typename U>
    std::size_t operator()(T&&, U&&)
    {
        return ++rvalue;
    }
};

TEST_CASE("apply<R>(F&&, variant<Ts...>&, variant<Us...>&)", "[variant.apply]")
{
    eggs::variant<int, std::string> v1(42);

    REQUIRE(v1.which() == 0);
    REQUIRE(*v1.target<int>() == 42);

    eggs::variant<std::string, int> v2(std::string{"42"});

    REQUIRE(v2.which() == 0);
    REQUIRE(*v2.target<std::string>() == "42");

    fun2 f;
    std::size_t calls = eggs::variants::apply<std::size_t>(f, v1, v2);

    REQUIRE(f.nonconst_lvalue == 1);
    REQUIRE(calls == f.nonconst_lvalue);
}

TEST_CASE("apply<R>(F&&, variant<Ts...> const&, variant<Us...> const&)", "[variant.apply]")
{
    eggs::variant<int, std::string> const v1(42);

    REQUIRE(v1.which() == 0);
    REQUIRE(*v1.target<int>() == 42);

    eggs::variant<std::string, int> const v2(std::string{"42"});

    REQUIRE(v2.which() == 0);
    REQUIRE(*v2.target<std::string>() == "42");

    fun2 f;
    std::size_t calls = eggs::variants::apply<std::size_t>(f, v1, v2);

    REQUIRE(f.const_lvalue == 1);
    REQUIRE(calls == f.const_lvalue);
}

TEST_CASE("apply<R>(F&&, variant<Ts...>&&, variant<Us...>&&)", "[variant.apply]")
{
    eggs::variant<int, std::string> v1(42);

    REQUIRE(v1.which() == 0);
    REQUIRE(*v1.target<int>() == 42);

    eggs::variant<std::string, int> v2(std::string{"42"});

    REQUIRE(v2.which() == 0);
    REQUIRE(*v2.target<std::string>() == "42");

    fun2 f;
    std::size_t calls = eggs::variants::apply<std::size_t>(f, std::move(v1), std::move(v2));

    REQUIRE(f.rvalue == 1);
    REQUIRE(calls == f.rvalue);
}
