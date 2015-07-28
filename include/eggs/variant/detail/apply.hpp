//! \file eggs/variant/detail/apply.hpp
// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2015
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EGGS_VARIANT_DETAIL_APPLY_HPP
#define EGGS_VARIANT_DETAIL_APPLY_HPP

#include <eggs/variant/detail/pack.hpp>
#include <eggs/variant/detail/storage.hpp>
#include <eggs/variant/detail/visitor.hpp>

#include <eggs/variant/bad_variant_access.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

#include <eggs/variant/detail/config/prefix.hpp>

namespace eggs { namespace variants { namespace detail
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename F, typename ...Args>
    EGGS_CXX11_CONSTEXPR auto _invoke(F&& f, Args&&... args)
        EGGS_CXX11_NOEXCEPT_IF(EGGS_CXX11_NOEXCEPT_EXPR(
            std::forward<F>(f)(std::forward<Args>(args)...)))
     -> decltype(std::forward<F>(f)(std::forward<Args>(args)...))
    {
        return std::forward<F>(f)(std::forward<Args>(args)...);
    }

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
    template <typename F, typename Arg0, typename ...Args>
    EGGS_CXX11_CONSTEXPR auto _invoke(F&& f, Arg0&& arg0, Args&&... args)
        EGGS_CXX11_NOEXCEPT_IF(EGGS_CXX11_NOEXCEPT_EXPR(
            (arg0.*f)(std::forward<Args>(args)...)))
     -> decltype((arg0.*f)(std::forward<Args>(args)...))
    {
        return (arg0.*f)(std::forward<Args>(args)...);
    }

    template <typename F, typename Arg0, typename ...Args>
    EGGS_CXX11_CONSTEXPR auto _invoke(F&& f, Arg0&& arg0, Args&&... args)
        EGGS_CXX11_NOEXCEPT_IF(EGGS_CXX11_NOEXCEPT_EXPR(
            ((*arg0).*f)(std::forward<Args>(args)...)))
     -> decltype(((*arg0).*f)(std::forward<Args>(args)...))
    {
        return ((*arg0).*f)(std::forward<Args>(args)...);
    }

    template <typename F, typename Arg0>
    EGGS_CXX11_CONSTEXPR auto _invoke(F&& f, Arg0&& arg0) EGGS_CXX11_NOEXCEPT
     -> decltype(arg0.*f)
    {
        return arg0.*f;
    }

    template <typename F, typename Arg0>
    EGGS_CXX11_CONSTEXPR auto _invoke(F&& f, Arg0&& arg0) EGGS_CXX11_NOEXCEPT
     -> decltype((*arg0).*f)
    {
        return (*arg0).*f;
    }
#endif

    template <typename R>
    struct _invoke_guard
    {
        template <typename ...Ts>
        EGGS_CXX11_CONSTEXPR R operator()(Ts&&... vs) const
            EGGS_CXX11_NOEXCEPT_IF(EGGS_CXX11_NOEXCEPT_EXPR(
                _invoke(std::forward<Ts>(vs)...)))
        {
            return detail::_invoke(std::forward<Ts>(vs)...);
        }
    };

    template <>
    struct _invoke_guard<void>
    {
        template <typename ...Ts>
        EGGS_CXX14_CONSTEXPR void operator()(Ts&&... vs) const
            EGGS_CXX11_NOEXCEPT_IF(EGGS_CXX11_NOEXCEPT_EXPR(
                _invoke(std::forward<Ts>(vs)...)))
        {
            detail::_invoke(std::forward<Ts>(vs)...);
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename T, typename Is = make_index_pack<T::size>>
    struct _make_apply_pack;

    template <typename T, std::size_t I, std::size_t ...Is>
    struct _make_apply_pack<T, pack_c<std::size_t, I, Is...>>
    {
        using type = pack<index<Is>...>;
    };

    template <typename T>
    using _apply_pack = typename _make_apply_pack<T>::type;

    ///////////////////////////////////////////////////////////////////////////
    template <typename V, typename I>
    struct _apply_get
    {
        using type = decltype(std::declval<V>().get(I{}));

        type EGGS_CXX11_CONSTEXPR operator()(V& v) const
        {
            return v.get(I{});
        }
    };

    template <typename V, typename I>
    struct _apply_get<V&&, I>
    {
        using type = typename std::remove_reference<
            decltype(std::declval<V>().get(I{}))>::type;

        EGGS_CXX11_CONSTEXPR type operator()(V& v) const
        {
            return std::move(v.get(I{}));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename R, typename F, typename Ms, typename Vs>
    struct _apply;

    template <
        typename R, typename F, typename ...Ms, typename V
    >
    struct _apply<R, F, pack<Ms...>, pack<V>>
      : visitor<
            _apply<R, F, pack<Ms...>, pack<V>>
          , R(F&&, Ms..., V&&)
        >
    {
        template <typename I>
        static EGGS_CXX11_CONSTEXPR R call(F&& f, Ms... ms, V&& v)
        {
            return _invoke_guard<R>{}(
                std::forward<F>(f), std::forward<Ms>(ms)...
              , _apply_get<V, I>{}(v));
        }
    };

    template <
        typename R, typename F, typename ...Ms
      , typename V0, typename V1, typename ...Vs
    >
    struct _apply<R, F, pack<Ms...>, pack<V0, V1, Vs...>>
      : visitor<
            _apply<R, F, pack<Ms...>, pack<V0, V1, Vs...>>
          , R(F&&, Ms..., V0&&, V1&&, Vs&&...)
        >
    {
        template <typename I>
        static EGGS_CXX11_CONSTEXPR R call(F&& f, Ms... ms, V0&& v0,
            V1&& v1, Vs&&... vs)
        {
            using T = typename _apply_get<V0, I>::type;
            return v0.which() != 0
              ? _apply<R, F, pack<Ms..., T>, pack<V1, Vs...>>{}(
                    _apply_pack<typename std::decay<V1>::type>{}, v1.which() - 1
                  , std::forward<F>(f)
                  , std::forward<Ms>(ms)..., _apply_get<V0, I>{}(v0)
                  , std::forward<V1>(v1), std::forward<Vs>(vs)...
                )
              : throw_bad_variant_access<R>();
        }
    };

    template <typename R, typename F, typename V, typename ...Vs>
    EGGS_CXX11_CONSTEXPR R apply(F&& f, V&& v, Vs&&... vs)
    {
        return v.which() != 0
          ? _apply<R, F, pack<>, pack<V&&, Vs&&...>>{}(
                _apply_pack<typename std::decay<V>::type>{}, v.which() - 1
              , std::forward<F>(f)
              , std::forward<V>(v), std::forward<Vs>(vs)...
            )
          : throw_bad_variant_access<R>();
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename ...Ts>
    struct _always_void
    {
        using type = void;
    };

    template <typename T, typename Enable = void>
    struct _weak_result
    {};

    template <typename R, typename ...Args>
    struct _weak_result<R(*)(Args...)>
    {
        using type = R;
    };

    template <typename R, typename ...Args>
    struct _weak_result<R(*)(Args..., ...)>
    {
        using type = R;
    };

    template <typename C>
    struct _weak_result<C, typename _always_void<typename C::result_type>::type>
    {
        using type = typename C::result_type;
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename F, typename Ms, typename Vs>
    struct _apply_result_deduce;

    template <typename R, typename ...Rs>
    struct _apply_result_combine
      : std::enable_if<all_of<pack<std::is_same<R, Rs>...>>::value, R>
    {};

    template <
        typename F, typename Ms, typename V, typename Vs,
        typename Is = _apply_pack<typename std::decay<V>::type>
    >
    struct _apply_result_expand;

    template <
        typename F, typename ...Ms, typename V, typename ...Vs,
        typename ...Is
    >
    struct _apply_result_expand<F, pack<Ms...>, V, pack<Vs...>, pack<Is...>>
      : _apply_result_combine<
            typename _apply_result_deduce<
                F, pack<Ms..., typename _apply_get<V, Is>::type>
              , pack<Vs...>
            >::type...
        >
    {};

    template <typename F, typename ...Ms>
    struct _apply_result_deduce<F, pack<Ms...>, pack<>>
    {
        using type = decltype(detail::_invoke(
            std::declval<F>(), std::declval<Ms>()...));
    };

    template <typename F, typename ...Ms, typename V, typename ...Vs>
    struct _apply_result_deduce<F, pack<Ms...>, pack<V, Vs...>>
      : _apply_result_expand<F, pack<Ms...>, V, pack<Vs...>>
    {};

    template <typename F, typename Vs, typename Enable = void>
    struct _apply_result
      : _apply_result_deduce<F, pack<>, Vs>
    {};

    template <typename F, typename ...Vs>
    struct _apply_result<
        F, pack<Vs...>
      , typename _always_void<
            typename _weak_result<typename std::decay<F>::type>::type
        >::type
    > : _weak_result<typename std::decay<F>::type>
    {};

    template <typename F, typename ...Vs>
    using apply_result = typename _apply_result<F, pack<Vs...>>::type;
}}}

#include <eggs/variant/detail/config/suffix.hpp>

#endif /*EGGS_VARIANT_DETAIL_APPLY_HPP*/
