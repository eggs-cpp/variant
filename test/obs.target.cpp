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

#include "catch.hpp"
#include "constexpr.hpp"

struct Base {};
struct Derived : Base {};

struct HasMemberAddressofOperator
{
    int x;

    HasMemberAddressofOperator() : x(0) {}
#if EGGS_CXX11_HAS_CONSTEXPR
    constexpr HasMemberAddressofOperator(int i) : x(i) {}
#endif

    void operator&() const {}
};

#if !EGGS_CXX11_STD_HAS_IS_TRIVIALLY_DESTRUCTIBLE
namespace eggs { namespace variants { namespace detail
{
    template <> struct is_trivially_destructible<HasMemberAddressofOperator> : std::true_type {};
}}}
#endif

struct HasFreeAddressofOperator
{
    int x;

    HasFreeAddressofOperator() : x(0) {}
#if EGGS_CXX11_HAS_CONSTEXPR
    constexpr HasFreeAddressofOperator(int i) : x(i) {}
#endif
};
void operator&(HasFreeAddressofOperator const&) {}

#if !EGGS_CXX11_STD_HAS_IS_TRIVIALLY_DESTRUCTIBLE
namespace eggs { namespace variants { namespace detail
{
    template <> struct is_trivially_destructible<HasFreeAddressofOperator> : std::true_type {};
}}}
#endif

TEST_CASE("variant<Ts...>::target()", "[variant.obs]")
{
    // non-empty
    {
        eggs::variant<int, std::string> const v(42);

        CHECK(v.target() != nullptr);

        CHECK(noexcept(v.target()) == true);

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            static constexpr eggs::variant<int, Constexpr> v(42);
            constexpr bool vttb = v.target() != nullptr;

#  if EGGS_CXX14_HAS_CONSTEXPR
            struct test { static constexpr int call()
            {
                eggs::variant<int, Constexpr> v(Constexpr(42));
                void* vtt = v.target();
                return 0;
            }};
            constexpr int c = test::call();
#  endif
        }
#endif
    }

    // empty
    {
        eggs::variant<int, std::string> const v;

        CHECK(v.target() == nullptr);

        CHECK(noexcept(v.target()) == true);

#if EGGS_CXX11_HAS_CONSTEXPR
        // constexpr
        {
            constexpr eggs::variant<int, Constexpr> v;
            constexpr bool vttb = v.target() == nullptr;

#  if EGGS_CXX14_HAS_CONSTEXPR
            struct test { static constexpr int call()
            {
                eggs::variant<int, Constexpr> v;
                void* vtt = v.target();
                return 0;
            }};
            constexpr int c = test::call();
#  endif
        }
#endif
    }
}

TEST_CASE("variant<>::target()", "[variant.obs]")
{
    eggs::variant<> const v;

    CHECK(v.target() == nullptr);

    CHECK(noexcept(v.target()) == true);

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<> v;
        constexpr bool vttb = v.target() == nullptr;

#  if EGGS_CXX14_HAS_CONSTEXPR
        struct test { static constexpr int call()
        {
            eggs::variant<> v;
            void* vtt = v.target();
            return 0;
        }};
        constexpr int c = test::call();
#  endif
    }
#endif
}

TEST_CASE("variant<Ts...>::target<T>()", "[variant.obs]")
{
    // non-empty
    {
        eggs::variant<int, std::string> const v(42);

        CHECK(v.target<int>() == v.target());
        CHECK(v.target<float>() == nullptr);
        CHECK(v.target<std::string>() == nullptr);

        CHECK(noexcept(v.target<int>()) == true);
    }

    // non-empty, base
    {
        eggs::variant<Derived, std::string> const v(Derived{});

        CHECK(v.target<Base>() == v.target());
        CHECK(v.target<Derived>() == v.target());
        CHECK(v.target<float>() == nullptr);
        CHECK(v.target<std::string>() == nullptr);

        CHECK(noexcept(v.target<int>()) == true);
    }

    // empty
    {
        eggs::variant<int, std::string> const v;

        CHECK(v.target<int>() == nullptr);
        CHECK(v.target<float>() == nullptr);
        CHECK(v.target<std::string>() == nullptr);

        CHECK(noexcept(v.target<int>()) == true);
    }
}

TEST_CASE("variant<Ts..., Fundamental>::target<Fundamental>()", "[variant.obs]")
{
    eggs::variant<int, float> const v(42.f);

    REQUIRE(v.which() == 1u);

    CHECK(v.target<float>() == v.target());

    CHECK(noexcept(v.target<float>()) == true);

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<int, float> v(42.f);
        constexpr bool vttb = *v.target<float>() == 42.f;

#  if EGGS_CXX14_HAS_CONSTEXPR
        struct test { static constexpr int call()
        {
            eggs::variant<int, float> v(42.f);
            *v.target<float>() = 43.f;
            return 0;
        }};
        constexpr int c = test::call();
#  endif
    }
#endif
}

TEST_CASE("variant<Ts..., Class>::target<Class>()", "[variant.obs]")
{
    eggs::variant<int, std::string> const v(std::string{"42"});

    REQUIRE(v.which() == 1u);

    CHECK(v.target<std::string>() == v.target());

    CHECK(noexcept(v.target<std::string>()) == true);

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<int, Constexpr> v(Constexpr(42));
        constexpr bool vttb = v.target<Constexpr>()->x == 42;

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
}

TEST_CASE("variant<Ts..., HasMemberAddressofOperator>::target<HasMemberAddressofOperator>()", "[variant.obs]")
{
    eggs::variant<int, HasMemberAddressofOperator> const v(HasMemberAddressofOperator{});

    REQUIRE(v.which() == 1u);

    CHECK(v.target<HasMemberAddressofOperator>() == v.target());

#if EGGS_CXX17_STD_HAS_CONSTEXPR_ADDRESSOF && EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        static constexpr eggs::variant<int, HasMemberAddressofOperator> v(HasMemberAddressofOperator(42));
        constexpr bool vttb = v.target<HasMemberAddressofOperator>()->x == 42;

#  if EGGS_CXX14_HAS_CONSTEXPR
        struct test { static constexpr int call()
        {
            eggs::variant<int, HasMemberAddressofOperator> v(HasMemberAddressofOperator(42));
            v.target<HasMemberAddressofOperator>()->x = 43;
            return 0;
        }};
        constexpr int c = test::call();
#  endif
    }
#endif
}

TEST_CASE("variant<Ts..., HasFreeAddressofOperator>::target<HasFreeAddressofOperator>()", "[variant.obs]")
{
    eggs::variant<int, HasFreeAddressofOperator> const v(HasFreeAddressofOperator{});

    REQUIRE(v.which() == 1u);

    CHECK(v.target<HasFreeAddressofOperator>() == v.target());

#if EGGS_CXX17_STD_HAS_CONSTEXPR_ADDRESSOF && EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        static constexpr eggs::variant<int, HasFreeAddressofOperator> v(HasFreeAddressofOperator(42));
        constexpr bool vttb = v.target<HasFreeAddressofOperator>()->x == 42;

#  if EGGS_CXX14_HAS_CONSTEXPR
        struct test { static constexpr int call()
        {
            eggs::variant<int, HasFreeAddressofOperator> v(HasFreeAddressofOperator(42));
            v.target<HasFreeAddressofOperator>()->x = 43;
            return 0;
        }};
        constexpr int c = test::call();
#  endif
    }
#endif
}

TEST_CASE("variant<>::target<T>()", "[variant.obs]")
{
    eggs::variant<> const v;

    CHECK(v.target<int>() == nullptr);

    CHECK(noexcept(v.target<int>()) == true);

#if EGGS_CXX11_HAS_CONSTEXPR
    // constexpr
    {
        constexpr eggs::variant<> v;
        constexpr bool vttb = v.target<int>() == nullptr;

#  if EGGS_CXX14_HAS_CONSTEXPR
        struct test { static constexpr int call()
        {
            eggs::variant<> v;
            int* vtt = v.target<int>();
            return 0;
        }};
        constexpr int c = test::call();
#  endif
    }
#endif
}
