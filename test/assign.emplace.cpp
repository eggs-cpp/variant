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
#include "throw.hpp"

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
template <typename ...Ts>
struct _void
{
    using type = void;
};

template <typename V, std::size_t I, typename Args, typename Enable = void>
struct _has_emplace_index
  : std::false_type
{};

template <typename V, std::size_t I, typename ...Args>
struct _has_emplace_index<
    V, I, void(Args...), typename _void<
        decltype(std::declval<V>().template emplace<I>(std::declval<Args>()...))
    >::type
> : std::true_type
{};

template <typename V, std::size_t I, typename ...Args>
struct has_emplace_index
  : _has_emplace_index<V, I, void(Args&&...)>
{};

template <typename V, typename T, typename Args, typename Enable = void>
struct _has_emplace_type
  : std::false_type
{};

template <typename V, typename T, typename ...Args>
struct _has_emplace_type<
    V, T, void(Args...), typename _void<
        decltype(std::declval<V>().template emplace<T>(std::declval<Args>()...))
    >::type
> : std::true_type
{};

template <typename V, typename T, typename ...Args>
struct has_emplace_type
  : _has_emplace_type<V, T, void(Args&&...)>
{};
#endif

struct NonAssignable
{
    NonAssignable() {}
    NonAssignable(NonAssignable&&) {}; // not trivially copyable
    NonAssignable& operator=(NonAssignable const&) = delete;
    ~NonAssignable() {}
};

struct NonAssignableTrivial
{
    NonAssignableTrivial() {}
    NonAssignableTrivial(NonAssignableTrivial&&) = default;
    NonAssignableTrivial& operator=(NonAssignableTrivial const&) = delete;
};

#if !EGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE || !EGGS_CXX11_STD_HAS_IS_TRIVIALLY_DESTRUCTIBLE
namespace eggs { namespace variants { namespace detail
{
    template <> struct is_trivially_copyable<NonAssignableTrivial> : std::true_type {};
    template <> struct is_trivially_destructible<NonAssignableTrivial> : std::true_type {};
}}}
#endif

TEST_CASE("variant<Ts...>::emplace<I>(Args&&...)", "[variant.assign]")
{
    // empty target
    {
        eggs::variant<int, std::string> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == eggs::variant_npos);

        int& r = v.emplace<0>(42);

        CHECK(bool(v) == true);
        CHECK(v.which() == 0u);
        CHECK(v.target<int>() == &r);
        CHECK(r == 42);

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(int));
#endif

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v;
                ConstexprTrivial& r = v.emplace<1>(42);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    // same target
    {
        eggs::variant<int, std::string> v(43);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 43);

        int& r = v.emplace<0>(42);

        CHECK(bool(v) == true);
        CHECK(v.which() == 0u);
        CHECK(v.target<int>() == &r);
        CHECK(r == 42);

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(int));
#endif

        // dtor
        {
            eggs::variant<int, Dtor> v(eggs::variants::in_place<Dtor>);

            REQUIRE(v.which() == 1u);
            REQUIRE(Dtor::calls == 0u);

            v.emplace<1>();

            CHECK(v.which() == 1u);
            CHECK(Dtor::calls == 1u);
        }
        Dtor::calls = 0u;

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v(ConstexprTrivial(43));
                ConstexprTrivial& r = v.emplace<1>(42);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    // different target
    {
        eggs::variant<int, std::string> v(std::string(""));

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "");

        int& r = v.emplace<0>(42);

        CHECK(bool(v) == true);
        CHECK(v.which() == 0u);
        CHECK(v.target<int>() == &r);
        CHECK(r == 42);

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(int));
#endif

        // dtor
        {
            eggs::variant<int, Dtor> v(eggs::variants::in_place<Dtor>);

            REQUIRE(v.which() == 1u);
            REQUIRE(Dtor::calls == 0u);

            v.emplace<0>(42);

            CHECK(v.which() == 0u);
            CHECK(Dtor::calls == 1u);
        }
        Dtor::calls = 0u;

#if EGGS_CXX98_HAS_EXCEPTIONS
        // exception-safety
        {
            eggs::variant<Dtor, Throw> v;
            v.emplace<0>();

            REQUIRE(bool(v) == true);
            REQUIRE(v.which() == 0u);
            REQUIRE(Dtor::calls == 0u);

            CHECK_THROWS(v.emplace<1>(0));

            CHECK(bool(v) == false);
            CHECK(v.which() == eggs::variant_npos);
            CHECK(Dtor::calls == 1u);
        }
        Dtor::calls = 0u;
#endif

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v(43);
                ConstexprTrivial& r = v.emplace<1>(42);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
    // sfinae
    {
        CHECK(
            !has_emplace_index<
                eggs::variant<int>,
                0, std::string
            >::value);
    }
#endif
}

TEST_CASE("variant<T, T>::emplace<I>(Args&&...)", "[variant.assign]")
{
    eggs::variant<int, int> v;

    REQUIRE(bool(v) == false);
    REQUIRE(v.which() == eggs::variant_npos);

    int& r = v.emplace<0>(42);

    CHECK(bool(v) == true);
    CHECK(v.which() == 0u);
    CHECK(v.target<int>() == &r);
    CHECK(r == 42);

#if EGGS_CXX98_HAS_RTTI
    CHECK(v.target_type() == typeid(int));
#endif
}

TEST_CASE("variant<NonAssignable>::emplace<I>(Args&&...)", "[variant.assign]")
{
    eggs::variant<int, NonAssignable> v(42);

    REQUIRE(v.which() == 0);

    NonAssignable& r = v.emplace<1>();

    CHECK(bool(v) == true);
    CHECK(v.which() == 1u);
    CHECK(v.target<NonAssignable>() == &r);

    // trivially_copyable
    {
        eggs::variant<int, NonAssignableTrivial> v(42);

        REQUIRE(v.which() == 0);

        NonAssignableTrivial& r = v.emplace<1>();

        CHECK(bool(v) == true);
        CHECK(v.which() == 1u);
        CHECK(v.target<NonAssignableTrivial>() == &r);
    }
}

TEST_CASE("variant<Ts...>::emplace<I>(std::initializer_list<U>, Args&&...)", "[variant.assign]")
{
    // empty target
    {
        eggs::variant<int, std::string> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == eggs::variant_npos);

        std::string& r = v.emplace<1>({'4', '2'});

        CHECK(bool(v) == true);
        CHECK(v.which() == 1u);
        CHECK(v.target<std::string>() == &r);
        CHECK(r == "42");

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(std::string));
#endif

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v;
                ConstexprTrivial& r = v.emplace<1>({4, 2});
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    // same target
    {
        eggs::variant<int, std::string> v(std::string(""));

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "");

        std::string& r = v.emplace<1>({'4', '2'});

        CHECK(bool(v) == true);
        CHECK(v.which() == 1u);
        CHECK(v.target<std::string>() == &r);
        CHECK(r == "42");

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(std::string));
#endif

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v(ConstexprTrivial(43));
                ConstexprTrivial& r = v.emplace<1>({4, 2});
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    // different target
    {
        eggs::variant<int, std::string> v(43);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 43);

        std::string& r = v.emplace<1>({'4', '2'});

        CHECK(bool(v) == true);
        CHECK(v.which() == 1u);
        CHECK(v.target<std::string>() == &r);
        CHECK(r == "42");

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(std::string));
#endif

#if EGGS_CXX98_HAS_EXCEPTIONS
        // exception-safety
        {
            eggs::variant<Dtor, Throw> v;
            v.emplace<0>();

            REQUIRE(bool(v) == true);
            REQUIRE(v.which() == 0u);
            REQUIRE(Dtor::calls == 0u);

            CHECK_THROWS(v.emplace<1>({0}));

            CHECK(bool(v) == false);
            CHECK(v.which() == eggs::variant_npos);
            CHECK(Dtor::calls == 1u);
        }
        Dtor::calls = 0u;
#endif

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v(43);
                ConstexprTrivial& r = v.emplace<1>({4, 2});
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
    // sfinae
    {
        CHECK(
            !has_emplace_index<
                eggs::variant<std::string>,
                0, std::initializer_list<int>
            >::value);
    }
#endif
}

TEST_CASE("variant<T, T>::emplace<I>(std::initializer_list<U>, Args&&...)", "[variant.assign]")
{
    eggs::variant<std::string, std::string> v;

    REQUIRE(bool(v) == false);
    REQUIRE(v.which() == eggs::variant_npos);

    std::string& r = v.emplace<1>({'4', '2'});

    CHECK(bool(v) == true);
    CHECK(v.which() == 1u);
    CHECK(v.target<std::string>() == &r);
    CHECK(r == "42");

#if EGGS_CXX98_HAS_RTTI
    CHECK(v.target_type() == typeid(std::string));
#endif
}

TEST_CASE("variant<Ts...>::emplace<T>(Args&&...)", "[variant.assign]")
{
    // empty target
    {
        eggs::variant<int, std::string> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == eggs::variant_npos);

        int& r = v.emplace<int>(42);

        CHECK(bool(v) == true);
        CHECK(v.which() == 0u);
        CHECK(v.target<int>() == &r);
        CHECK(r == 42);

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(int));
#endif

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v;
                ConstexprTrivial& r = v.emplace<ConstexprTrivial>(42);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    // same target
    {
        eggs::variant<int, std::string> v(43);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 43);

        int& r = v.emplace<int>(42);

        CHECK(bool(v) == true);
        CHECK(v.which() == 0u);
        CHECK(v.target<int>() == &r);
        CHECK(r == 42);

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(int));
#endif

        // dtor
        {
            eggs::variant<int, Dtor> v(eggs::variants::in_place<Dtor>);

            REQUIRE(v.which() == 1u);
            REQUIRE(Dtor::calls == 0u);

            v.emplace<Dtor>();

            CHECK(v.which() == 1u);
            CHECK(Dtor::calls == 1u);
        }
        Dtor::calls = 0u;

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v(ConstexprTrivial(43));
                ConstexprTrivial& r = v.emplace<ConstexprTrivial>(42);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    // different target
    {
        eggs::variant<int, std::string> v(std::string(""));

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "");

        int& r = v.emplace<int>(42);

        CHECK(bool(v) == true);
        CHECK(v.which() == 0u);
        CHECK(v.target<int>() == &r);
        CHECK(r == 42);

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(int));
#endif

        // dtor
        {
            eggs::variant<int, Dtor> v(eggs::variants::in_place<Dtor>);

            REQUIRE(v.which() == 1u);
            REQUIRE(Dtor::calls == 0u);

            v.emplace<int>(42);

            CHECK(v.which() == 0u);
            CHECK(Dtor::calls == 1u);
        }
        Dtor::calls = 0u;

#if EGGS_CXX98_HAS_EXCEPTIONS
        // exception-safety
        {
            eggs::variant<Dtor, Throw> v;
            v.emplace<0>();

            REQUIRE(bool(v) == true);
            REQUIRE(v.which() == 0u);
            REQUIRE(Dtor::calls == 0u);

            CHECK_THROWS(v.emplace<Throw>(0));

            CHECK(bool(v) == false);
            CHECK(v.which() == eggs::variant_npos);
            CHECK(Dtor::calls == 1u);
        }
        Dtor::calls = 0u;
#endif

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v(43);
                ConstexprTrivial& r = v.emplace<ConstexprTrivial>(42);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
    // sfinae
    {
        CHECK(
            !has_emplace_type<
                eggs::variant<int>,
                int, std::string
            >::value);
    }
#endif
}

TEST_CASE("variant<NonAssignable>::emplace<T>(Args&&...)", "[variant.assign]")
{
    eggs::variant<int, NonAssignable> v(42);

    REQUIRE(v.which() == 0);

    NonAssignable& r = v.emplace<NonAssignable>();

    CHECK(bool(v) == true);
    CHECK(v.which() == 1u);
    CHECK(v.target<NonAssignable>() == &r);

    // trivially_copyable
    {
        eggs::variant<int, NonAssignableTrivial> v(42);

        REQUIRE(v.which() == 0);

        NonAssignableTrivial& r = v.emplace<NonAssignableTrivial>();

        CHECK(bool(v) == true);
        CHECK(v.which() == 1u);
        CHECK(v.target<NonAssignableTrivial>() == &r);
    }
}

TEST_CASE("variant<Ts...>::emplace<T>(std::initializer_list<U>, Args&&...)", "[variant.assign]")
{
    // empty target
    {
        eggs::variant<int, std::string> v;

        REQUIRE(bool(v) == false);
        REQUIRE(v.which() == eggs::variant_npos);

        std::string& r = v.emplace<std::string>({'4', '2'});

        CHECK(bool(v) == true);
        CHECK(v.which() == 1u);
        CHECK(v.target<std::string>() == &r);
        CHECK(r == "42");

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(std::string));
#endif

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v;
                ConstexprTrivial& r = v.emplace<ConstexprTrivial>({4, 2});
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    // same target
    {
        eggs::variant<int, std::string> v(std::string(""));

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 1u);
        REQUIRE(*v.target<std::string>() == "");

        std::string& r = v.emplace<std::string>({'4', '2'});

        CHECK(bool(v) == true);
        CHECK(v.which() == 1u);
        CHECK(v.target<std::string>() == &r);
        CHECK(r == "42");

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(std::string));
#endif

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v(ConstexprTrivial(43));
                ConstexprTrivial& r = v.emplace<ConstexprTrivial>({4, 2});
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    // different target
    {
        eggs::variant<int, std::string> v(43);

        REQUIRE(bool(v) == true);
        REQUIRE(v.which() == 0u);
        REQUIRE(*v.target<int>() == 43);

        std::string& r = v.emplace<std::string>({'4', '2'});

        CHECK(bool(v) == true);
        CHECK(v.which() == 1u);
        CHECK(v.target<std::string>() == &r);
        CHECK(r == "42");

#if EGGS_CXX98_HAS_RTTI
        CHECK(v.target_type() == typeid(std::string));
#endif

#if EGGS_CXX98_HAS_EXCEPTIONS
        // exception-safety
        {
            eggs::variant<Dtor, Throw> v;
            v.emplace<0>();

            REQUIRE(bool(v) == true);
            REQUIRE(v.which() == 0u);
            REQUIRE(Dtor::calls == 0u);

            CHECK_THROWS(v.emplace<Throw>({0}));

            CHECK(bool(v) == false);
            CHECK(v.which() == eggs::variant_npos);
            CHECK(Dtor::calls == 1u);
        }
        Dtor::calls = 0u;
#endif

#if EGGS_CXX14_HAS_CONSTEXPR
        // constexpr
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v(43);
                ConstexprTrivial& r = v.emplace<ConstexprTrivial>({4, 2});
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
    // sfinae
    {
        CHECK(
            !has_emplace_type<
                eggs::variant<std::string>,
                std::string, std::initializer_list<int>
            >::value);
    }
#endif
}
