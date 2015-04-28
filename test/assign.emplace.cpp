// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2015
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "constexpr.hpp"
#include "dtor.hpp"
#include "throw.hpp"

EGGS_CXX11_STATIC_CONSTEXPR std::size_t npos = eggs::variant<>::npos;

TEST_CASE("variant<Ts...>::emplace<I>(Args&&...)", "[variant.assign]")
{
    SECTION("empty target")
    {
        eggs::variant<int, std::string> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == npos);

        v.emplace<0>(42);

        CHECK(bool(v) == true);
        CHECK(v.which() == 0u);
        CHECK(v.target_type() == typeid(int));
        REQUIRE(v.target<int>() != nullptr);
        CHECK(*v.target<int>() == 42);

#if EGGS_CXX14_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v;
                v.emplace<1>(42);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    SECTION("same target")
    {
        eggs::variant<int, std::string> v(43);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 43);

        v.emplace<0>(42);

        CHECK(bool(v) == true);
        CHECK(v.which() == 0u);
        CHECK(v.target_type() == typeid(int));
        REQUIRE(v.target<int>() != nullptr);
        CHECK(*v.target<int>() == 42);

#if EGGS_CXX14_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v(ConstexprTrivial(43));
                v.emplace<1>(42);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    SECTION("different target")
    {
        eggs::variant<int, std::string> v(std::string{""});

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "");

        v.emplace<0>(42);

        CHECK(bool(v) == true);
        CHECK(v.which() == 0u);
        CHECK(v.target_type() == typeid(int));
        REQUIRE(v.target<int>() != nullptr);
        CHECK(*v.target<int>() == 42);

        SECTION("exception-safety")
        {
            eggs::variant<Dtor, Throw> v;
            v.emplace<0>();

            REQUIRE(bool(v) == true);
            REQUIRE(v.which() == 0u);
            REQUIRE(Dtor::called == false);

            CHECK_THROWS(v.emplace<1>(0));

            CHECK(bool(v) == false);
            CHECK(v.which() == npos);
            CHECK(Dtor::called == true);
        }
        Dtor::called = false;

#if EGGS_CXX14_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v(43);
                v.emplace<1>(42);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }
}

TEST_CASE("variant<T, T>::emplace<I>(Args&&...)", "[variant.assign]")
{
    eggs::variant<int, int> v;

    REQUIRE(bool(v) == false);
    REQUIRE(v.which() == npos);

    v.emplace<0>(42);

    CHECK(bool(v) == true);
    CHECK(v.which() == 0u);
    CHECK(v.target_type() == typeid(int));
    REQUIRE(v.target() != nullptr);
}

#if EGGS_CXX11_HAS_INITIALIZER_LIST_OVERLOADING
TEST_CASE("variant<Ts...>::emplace<I>(std::initializer_list<U>, Args&&...)", "[variant.assign]")
{
    SECTION("empty target")
    {
        eggs::variant<int, std::string> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == npos);

        v.emplace<1>({'4', '2'});

        CHECK(bool(v) == true);
        CHECK(v.which() == 1u);
        CHECK(v.target_type() == typeid(std::string));
        REQUIRE(v.target<std::string>() != nullptr);
        CHECK(*v.target<std::string>() == "42");

#if EGGS_CXX14_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v;
                v.emplace<1>({4, 2});
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    SECTION("same target")
    {
        eggs::variant<int, std::string> v(std::string{""});

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "");

        v.emplace<1>({'4', '2'});

        CHECK(bool(v) == true);
        CHECK(v.which() == 1u);
        CHECK(v.target_type() == typeid(std::string));
        REQUIRE(v.target<std::string>() != nullptr);
        CHECK(*v.target<std::string>() == "42");

#if EGGS_CXX14_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v(ConstexprTrivial(43));
                v.emplace<1>({4, 2});
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    SECTION("different target")
    {
        eggs::variant<int, std::string> v(43);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 43);

        v.emplace<1>({'4', '2'});

        CHECK(bool(v) == true);
        CHECK(v.which() == 1u);
        CHECK(v.target_type() == typeid(std::string));
        REQUIRE(v.target<std::string>() != nullptr);
        CHECK(*v.target<std::string>() == "42");

        SECTION("exception-safety")
        {
            eggs::variant<Dtor, Throw> v;
            v.emplace<0>();

            REQUIRE(bool(v) == true);
            REQUIRE(v.which() == 0u);
            REQUIRE(Dtor::called == false);

            CHECK_THROWS(v.emplace<1>({0}));

            CHECK(bool(v) == false);
            CHECK(v.which() == npos);
            CHECK(Dtor::called == true);
        }
        Dtor::called = false;

#if EGGS_CXX14_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v(43);
                v.emplace<1>({4, 2});
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }
}

TEST_CASE("variant<T, T>::emplace<I>(std::initializer_list<U>, Args&&...)", "[variant.assign]")
{
    eggs::variant<std::string, std::string> v;

    REQUIRE(bool(v) == false);
    REQUIRE(v.which() == npos);

    v.emplace<1>({'4', '2'});

    CHECK(bool(v) == true);
    CHECK(v.which() == 1u);
    CHECK(v.target_type() == typeid(std::string));
    REQUIRE(v.target() != nullptr);
}
#endif

#if EGGS_CXX11_HAS_TEMPLATE_ARGUMENT_OVERLOADING
TEST_CASE("variant<Ts...>::emplace<T>(Args&&...)", "[variant.assign]")
{
    SECTION("empty target")
    {
        eggs::variant<int, std::string> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == npos);

        v.emplace<int>(42);

        CHECK(bool(v) == true);
        CHECK(v.which() == 0u);
        CHECK(v.target_type() == typeid(int));
        REQUIRE(v.target<int>() != nullptr);
        CHECK(*v.target<int>() == 42);

#if EGGS_CXX14_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v;
                v.emplace<ConstexprTrivial>(42);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    SECTION("same target")
    {
        eggs::variant<int, std::string> v(43);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 43);

        v.emplace<int>(42);

        CHECK(bool(v) == true);
        CHECK(v.which() == 0u);
        CHECK(v.target_type() == typeid(int));
        REQUIRE(v.target<int>() != nullptr);
        CHECK(*v.target<int>() == 42);

#if EGGS_CXX14_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v(ConstexprTrivial(43));
                v.emplace<ConstexprTrivial>(42);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    SECTION("different target")
    {
        eggs::variant<int, std::string> v(std::string{""});

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "");

        v.emplace<int>(42);

        CHECK(bool(v) == true);
        CHECK(v.which() == 0u);
        CHECK(v.target_type() == typeid(int));
        REQUIRE(v.target<int>() != nullptr);
        CHECK(*v.target<int>() == 42);

        SECTION("exception-safety")
        {
            eggs::variant<Dtor, Throw> v;
            v.emplace<0>();

            REQUIRE(bool(v) == true);
            REQUIRE(v.which() == 0u);
            REQUIRE(Dtor::called == false);

            CHECK_THROWS(v.emplace<Throw>(0));

            CHECK(bool(v) == false);
            CHECK(v.which() == npos);
            CHECK(Dtor::called == true);
        }
        Dtor::called = false;

#if EGGS_CXX14_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v(43);
                v.emplace<ConstexprTrivial>(42);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }
}

#if EGGS_CXX11_HAS_INITIALIZER_LIST_OVERLOADING
TEST_CASE("variant<Ts...>::emplace<T>(std::initializer_list<U>, Args&&...)", "[variant.assign]")
{
    SECTION("empty target")
    {
        eggs::variant<int, std::string> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == npos);

        v.emplace<std::string>({'4', '2'});

        CHECK(bool(v) == true);
        CHECK(v.which() == 1u);
        CHECK(v.target_type() == typeid(std::string));
        REQUIRE(v.target<std::string>() != nullptr);
        CHECK(*v.target<std::string>() == "42");

#if EGGS_CXX14_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v;
                v.emplace<ConstexprTrivial>({4, 2});
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    SECTION("same target")
    {
        eggs::variant<int, std::string> v(std::string{""});

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "");

        v.emplace<std::string>({'4', '2'});

        CHECK(bool(v) == true);
        CHECK(v.which() == 1u);
        CHECK(v.target_type() == typeid(std::string));
        REQUIRE(v.target<std::string>() != nullptr);
        CHECK(*v.target<std::string>() == "42");

#if EGGS_CXX14_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v(ConstexprTrivial(43));
                v.emplace<ConstexprTrivial>({4, 2});
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    SECTION("different target")
    {
        eggs::variant<int, std::string> v(43);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 43);

        v.emplace<std::string>({'4', '2'});

        CHECK(bool(v) == true);
        CHECK(v.which() == 1u);
        CHECK(v.target_type() == typeid(std::string));
        REQUIRE(v.target<std::string>() != nullptr);
        CHECK(*v.target<std::string>() == "42");

        SECTION("exception-safety")
        {
            eggs::variant<Dtor, Throw> v;
            v.emplace<0>();

            REQUIRE(bool(v) == true);
            REQUIRE(v.which() == 0u);
            REQUIRE(Dtor::called == false);

            CHECK_THROWS(v.emplace<Throw>({0}));

            CHECK(bool(v) == false);
            CHECK(v.which() == npos);
            CHECK(Dtor::called == true);
        }
        Dtor::called = false;

#if EGGS_CXX14_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v(43);
                v.emplace<ConstexprTrivial>({4, 2});
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }
}
#endif
#endif
