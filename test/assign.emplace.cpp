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
#include "dtor.hpp"
#include "throw.hpp"

constexpr std::size_t npos = eggs::variant<>::npos;

TEST_CASE("variant<Ts...>::emplace<I>(Args&&...)", "[variant.assign]")
{
    SECTION("empty target")
    {
        eggs::variant<int, std::string> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == npos);

        v.emplace<0>(42);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(v.target_type() == typeid(int));
        REQUIRE(*v.target<int>() == 42);
    }

    SECTION("same target")
    {
        eggs::variant<int, std::string> v(43);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 43);

        v.emplace<0>(42);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(v.target_type() == typeid(int));
        REQUIRE(*v.target<int>() == 42);
    }

    SECTION("different target")
    {
        eggs::variant<int, std::string> v(std::string{""});

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "");

        v.emplace<0>(42);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(v.target_type() == typeid(int));
        REQUIRE(*v.target<int>() == 42);

        SECTION("exception-safety")
        {
            eggs::variant<Dtor, Throw> v;
            v.emplace<Dtor>();

            REQUIRE(bool(v) == true);
            REQUIRE(v.which() == 0u);
            REQUIRE(Dtor::called == false);

            bool exception_thrown = false;
            try
            {
                v.emplace<1>(0);
            } catch (...) {
                exception_thrown = true;
            }
            REQUIRE(exception_thrown);
            REQUIRE(bool(v) == false);
            REQUIRE(v.which() == npos);
            REQUIRE(Dtor::called == true);
        }
        Dtor::called = false;
    }
}

TEST_CASE("variant<Ts...>::emplace<I>(std::initializer_list<U>, Args&&...)", "[variant.assign]")
{
    SECTION("empty target")
    {
        eggs::variant<int, std::string> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == npos);

        v.emplace<1>({'4', '2'});

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(v.target_type() == typeid(std::string));
        REQUIRE(*v.target<std::string>() == "42");
    }

    SECTION("same target")
    {
        eggs::variant<int, std::string> v(std::string{""});

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "");

        v.emplace<1>({'4', '2'});

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(v.target_type() == typeid(std::string));
        REQUIRE(*v.target<std::string>() == "42");
    }

    SECTION("different target")
    {
        eggs::variant<int, std::string> v(43);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 43);

        v.emplace<1>({'4', '2'});

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(v.target_type() == typeid(std::string));
        REQUIRE(*v.target<std::string>() == "42");

        SECTION("exception-safety")
        {
            eggs::variant<Dtor, Throw> v;
            v.emplace<Dtor>();

            REQUIRE(bool(v) == true);
            REQUIRE(v.which() == 0u);
            REQUIRE(Dtor::called == false);

            bool exception_thrown = false;
            try
            {
                v.emplace<1>({0});
            } catch (...) {
                exception_thrown = true;
            }
            REQUIRE(exception_thrown);
            REQUIRE(bool(v) == false);
            REQUIRE(v.which() == npos);
            REQUIRE(Dtor::called == true);
        }
        Dtor::called = false;
    }
}

TEST_CASE("variant<Ts...>::emplace<T>(Args&&...)", "[variant.assign]")
{
    SECTION("empty target")
    {
        eggs::variant<int, std::string> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == npos);

        v.emplace<int>(42);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(v.target_type() == typeid(int));
        REQUIRE(*v.target<int>() == 42);
    }

    SECTION("same target")
    {
        eggs::variant<int, std::string> v(43);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 43);

        v.emplace<int>(42);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(v.target_type() == typeid(int));
        REQUIRE(*v.target<int>() == 42);
    }

    SECTION("different target")
    {
        eggs::variant<int, std::string> v(std::string{""});

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "");

        v.emplace<int>(42);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(v.target_type() == typeid(int));
        REQUIRE(*v.target<int>() == 42);

        SECTION("exception-safety")
        {
            eggs::variant<Dtor, Throw> v;
            v.emplace<Dtor>();

            REQUIRE(bool(v) == true);
            REQUIRE(v.which() == 0u);
            REQUIRE(Dtor::called == false);

            bool exception_thrown = false;
            try
            {
                v.emplace<Throw>(0);
            } catch (...) {
                exception_thrown = true;
            }
            REQUIRE(exception_thrown);
            REQUIRE(bool(v) == false);
            REQUIRE(v.which() == npos);
            REQUIRE(Dtor::called == true);
        }
        Dtor::called = false;
    }
}

TEST_CASE("variant<Ts...>::emplace<T>(std::initializer_list<U>, Args&&...)", "[variant.assign]")
{
    SECTION("empty target")
    {
        eggs::variant<int, std::string> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == npos);

        v.emplace<std::string>({'4', '2'});

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(v.target_type() == typeid(std::string));
        REQUIRE(*v.target<std::string>() == "42");
    }

    SECTION("same target")
    {
        eggs::variant<int, std::string> v(std::string{""});

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "");

        v.emplace<std::string>({'4', '2'});

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(v.target_type() == typeid(std::string));
        REQUIRE(*v.target<std::string>() == "42");
    }

    SECTION("different target")
    {
        eggs::variant<int, std::string> v(43);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 43);

        v.emplace<std::string>({'4', '2'});

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(v.target_type() == typeid(std::string));
        REQUIRE(*v.target<std::string>() == "42");

        SECTION("exception-safety")
        {
            eggs::variant<Dtor, Throw> v;
            v.emplace<Dtor>();

            REQUIRE(bool(v) == true);
            REQUIRE(v.which() == 0u);
            REQUIRE(Dtor::called == false);

            bool exception_thrown = false;
            try
            {
                v.emplace<Throw>({0});
            } catch (...) {
                exception_thrown = true;
            }
            REQUIRE(exception_thrown);
            REQUIRE(bool(v) == false);
            REQUIRE(v.which() == npos);
            REQUIRE(Dtor::called == true);
        }
        Dtor::called = false;
    }
}
