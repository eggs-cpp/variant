//! \file eggs/variant/variant.hpp
// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EGGS_VARIANT_VARIANT_HPP
#define EGGS_VARIANT_VARIANT_HPP

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace eggs { namespace variants
{
    namespace detail
    {
        struct empty {};

        template <typename T>
        struct identity
        {
            using type = T;
        };

        template <typename ...Ts>
        struct pack
        {
            using type = pack;
        };

        template <typename T, T ...Vs>
        struct pack_c
        {
            using type = pack_c;
        };

        ///////////////////////////////////////////////////////////////////////
        template <typename ...Ts>
        struct _always_void
        {
            using type = void;
        };

        template <typename ...Ts>
        using always_void = typename _always_void<Ts...>::type;

        ///////////////////////////////////////////////////////////////////////
        template <typename Left, typename Right>
        struct _make_index_pack_join;

        template <std::size_t... Left, std::size_t... Right>
        struct _make_index_pack_join<
            pack_c<std::size_t, Left...>
          , pack_c<std::size_t, Right...>
        > : pack_c<std::size_t, Left..., (sizeof...(Left) + Right)...>
        {};

        template <std::size_t N>
        struct _make_index_pack
          : _make_index_pack_join<
                typename _make_index_pack<N / 2>::type
              , typename _make_index_pack<N - N / 2>::type
            >
        {};

        template <>
        struct _make_index_pack<1>
          : pack_c<std::size_t, 0>
        {};

        template <>
        struct _make_index_pack<0>
          : pack_c<std::size_t>
        {};

        template <typename Ts>
        struct _index_pack;

        template <typename ...Ts>
        struct _index_pack<pack<Ts...>>
          : _make_index_pack<sizeof...(Ts)>
        {};

        template <typename Ts>
        using index_pack = typename _index_pack<Ts>::type;

        ///////////////////////////////////////////////////////////////////////
        namespace swap_adl
        {
            using std::swap;

            template <typename T>
            struct is_nothrow_swappable
              : std::integral_constant<
                    bool
                  , noexcept(swap(std::declval<T&>(), std::declval<T&>()))
                >
            {};
        }

        template <typename T>
        struct is_nothrow_swappable
          : swap_adl::is_nothrow_swappable<T>
        {};

        ///////////////////////////////////////////////////////////////////////
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
        struct _weak_result<C, always_void<typename C::result_type>>
        {
            using type = typename C::result_type;
        };

        template <typename T>
        using weak_result = typename _weak_result<T>::type;

        ///////////////////////////////////////////////////////////////////////
        template <typename Vs>
        struct all_of;

        template <bool ...Vs>
        struct all_of<pack_c<bool, Vs...>>
          : std::integral_constant<
                bool
              , std::is_same<
                    pack_c<bool, Vs...>
                  , pack_c<bool, (Vs || true)...> // true...
                >::value
            >
        {};

        template <typename ...Ts>
        struct all_of<pack<Ts...>>
          : all_of<pack_c<bool, Ts::value...>>
        {};

        template <typename ...Vs>
        struct any_of;

        template <bool ...Vs>
        struct any_of<pack_c<bool, Vs...>>
          : std::integral_constant<
                bool
              , !all_of<pack_c<bool, !Vs...>>::value
            >
        {};

        template <typename ...Ts>
        struct any_of<pack<Ts...>>
          : any_of<pack_c<bool, Ts::value...>>
        {};

        template <typename T, typename Ts>
        struct contains;

        template <typename T, typename ...Ts>
        struct contains<T, pack<Ts...>>
          : any_of<pack<std::is_same<T, Ts>...>>
        {};

        ///////////////////////////////////////////////////////////////////////
        template <std::size_t I, typename T>
        struct _indexed {};

        template <typename Ts, typename Is = index_pack<Ts>>
        struct _indexer;

        template <typename ...Ts, std::size_t ...Is>
        struct _indexer<pack<Ts...>, pack_c<std::size_t, Is...>>
          : _indexed<Is, Ts>...
        {};

        empty _at_index(...);

        template <std::size_t I, typename T>
        identity<T> _at_index(_indexed<I, T> const&);

        template <std::size_t I, typename Ts>
        struct at_index
          : decltype(_at_index(_indexer<Ts>{}))
        {};

        empty _index_of(...);

        template <typename T, std::size_t I>
        std::integral_constant<std::size_t, I> _index_of(_indexed<I, T> const&);

        template <typename T, typename Ts>
        struct index_of
          : decltype(_index_of<T>(_indexer<Ts>{}))
        {};

        ///////////////////////////////////////////////////////////////////////
        template <typename F, typename Sig>
        struct visitor;

        template <typename F, typename R, typename ...Args>
        struct visitor<F, R(Args...)>
        {
            template <typename ...Ts>
            R operator()(pack<Ts...>, std::size_t which, Args&&... args) const
            {
                using fun_ptr = R(*)(Args...);
                static constexpr fun_ptr table[] = {&F::template call<Ts>...};

                return table[which](std::forward<Args>(args)...);
            }

            R operator()(pack<>, std::size_t /*which*/, Args&&... /*args*/) const
            {}
        };

        ///////////////////////////////////////////////////////////////////////
        struct copy_construct
          : visitor<copy_construct, void(void*, void const*)>
        {
            template <typename T>
            static void call(void* ptr, void const* other)
            {
                new (ptr) T(*static_cast<T const*>(other));
            }
        };

        struct move_construct
          : visitor<move_construct, void(void*, void*)>
        {
            template <typename T>
            static void call(void* ptr, void* other)
            {
                new (ptr) T(std::move(*static_cast<T*>(other)));
            }
        };

        struct copy_assign
          : visitor<copy_assign, void(void*, void const*)>
        {
            template <typename T>
            static void call(void* ptr, void const* other)
            {
                *static_cast<T*>(ptr) = *static_cast<T const*>(other);
            }
        };

        struct move_assign
          : visitor<move_assign, void(void*, void*)>
        {
            template <typename T>
            static void call(void* ptr, void* other)
            {
                *static_cast<T*>(ptr) = std::move(*static_cast<T*>(other));
            }
        };

        struct destroy
          : visitor<destroy, void(void*)>
        {
            template <typename T>
            static void call(void* ptr)
            {
                static_cast<T*>(ptr)->~T();
            }
        };

        struct swap
          : visitor<swap, void(void*, void*)>
        {
            template <typename T>
            static void call(void* ptr, void* other)
            {
                using std::swap;
                swap(*static_cast<T*>(ptr), *static_cast<T*>(other));
            }
        };

        struct equal_to
          : visitor<equal_to, bool(void const*, void const*)>
        {
            template <typename T>
            static bool call(void const* ptr, void const* other)
            {
                return *static_cast<T const*>(ptr) == *static_cast<T const*>(other);
            }
        };

        struct less
          : visitor<less, bool(void const*, void const*)>
        {
            template <typename T>
            static bool call(void const* ptr, void const* other)
            {
                return *static_cast<T const*>(ptr) < *static_cast<T const*>(other);
            }
        };

        struct type_id
          : visitor<type_id, std::type_info const&()>
        {
            template <typename T>
            static std::type_info const& call()
            {
                return typeid(T);
            }
        };

        ///////////////////////////////////////////////////////////////////////
        template <typename F, typename ...Args>
        constexpr auto _invoke(F&& f, Args&&... args)
            noexcept(noexcept(std::forward<F>(f)(std::forward<Args>(args)...)))
         -> decltype(std::forward<F>(f)(std::forward<Args>(args)...))
        {
            return std::forward<F>(f)(std::forward<Args>(args)...);
        }

        //template <typename F, typename Arg0, typename ...Args>
        //constexpr auto _invoke(F&& f, Arg0&& arg0, Args&&... args)
        //    noexcept(noexcept((arg0.*f)(std::forward<Args>(args)...)))
        // -> decltype((arg0.*f)(std::forward<Args>(args)...))
        //{
        //    return (arg0.*f)(std::forward<Args>(args)...);
        //}

        //template <typename F, typename Arg0, typename ...Args>
        //constexpr auto _invoke(F&& f, Arg0&& arg0, Args&&... args)
        //    noexcept(noexcept(((*arg0).*f)(std::forward<Args>(args)...)))
        // -> decltype(((*arg0).*f)(std::forward<Args>(args)...))
        //{
        //    return ((*arg0).*f)(std::forward<Args>(args)...);
        //}

        //template <typename F, typename Arg0>
        //constexpr auto _invoke(F&& f, Arg0&& arg0) noexcept
        // -> decltype(arg0.*f)
        //{
        //    return arg0.*f;
        //}

        //template <typename F, typename Arg0>
        //constexpr auto _invoke(F&& f, Arg0&& arg0) noexcept
        // -> decltype((*arg0).*f)
        //{
        //    return (*arg0).*f;
        //}

        template <typename R>
        struct _void_guard
        {};

        template <>
        struct _void_guard<void>
        {
            template <typename T>
            void operator,(T const&) const noexcept {}
        };

        template <typename R, typename F>
        struct apply
          : visitor<apply<R, F>, R(void*, F&&)>
          , visitor<apply<R, F>, R(void const*, F&&)>
        {
            template <typename T>
            static R call(void* ptr, F&& f)
            {
                using value_type = std::decay_t<T>;
                return _void_guard<R>{}, _invoke(
                    std::forward<F>(f)
                  , std::forward<T>(*static_cast<value_type*>(ptr)));
            }

            template <typename T>
            static R call(void const* ptr, F&& f)
            {
                using value_type = std::decay_t<T> const;
                return _void_guard<R>{}, _invoke(
                    std::forward<F>(f)
                  , std::forward<T>(*static_cast<value_type*>(ptr)));
            }
        };

        ///////////////////////////////////////////////////////////////////////
        struct hash
        {
            using result_type = std::size_t;

            template <typename T>
            std::size_t operator()(T const& v) const
            {
                return std::hash<T>{}(v);
            }
        };
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename ...Ts>
    class variant
    {
        static_assert(
            !detail::any_of<detail::pack<std::is_function<Ts>...>>::value
          , "variant member has function type");

        static_assert(
            !detail::any_of<detail::pack<std::is_reference<Ts>...>>::value
          , "variant member has reference type");

        static_assert(
            !detail::any_of<detail::pack<std::is_void<Ts>...>>::value
          , "variant member has void type");

        using _types = detail::pack<detail::empty, std::remove_cv_t<Ts>...>;

        template <std::size_t I>
        using _at_index = detail::at_index<I, _types>;

        template <typename T>
        using _index_of = detail::index_of<std::decay_t<T>, _types>;

    public:
        static constexpr std::size_t npos = static_cast<std::size_t>(-1);

    public:
        variant() noexcept
          : _which{0}
        {}

        template <
            typename T
          , typename Enable = std::enable_if_t<
                detail::contains<
                    std::remove_cv_t<std::remove_reference_t<T>>
                  , detail::pack<std::remove_cv_t<Ts>...>
                >::value
            >
        > variant(T&& value)
            noexcept(
                std::is_nothrow_constructible<std::decay_t<T>, T&&>::value)
          : _which{_index_of<T>::value}
        {
            new (&_storage) T(std::forward<T>(value));
        }

        variant(variant const& other)
            noexcept(
                detail::all_of<detail::pack<
                    std::is_nothrow_copy_constructible<Ts>...
                >>::value)
          : _which{other._which}
        {
            detail::copy_construct{}(
                detail::pack<detail::empty, Ts...>{}, _which
              , &_storage, &other._storage
            );
        }

        variant(variant&& other)
            noexcept(
                detail::all_of<detail::pack<
                    std::is_nothrow_move_constructible<Ts>...
                >>::value)
          : _which{other._which}
        {
            detail::move_construct{}(
                detail::pack<detail::empty, Ts...>{}, _which
              , &_storage, &other._storage
            );
        }

        ~variant()
        {
            detail::destroy{}(
                detail::pack<detail::empty, Ts...>{}, _which
              , &_storage
            );
        }

        variant& operator=(variant const& other)
            noexcept(
                detail::all_of<detail::pack<
                    std::is_nothrow_copy_assignable<Ts>...
                  , std::is_nothrow_copy_constructible<Ts>...
                >>::value)
        {
            if (_which == other._which)
            {
                detail::copy_assign{}(
                    detail::pack<detail::empty, Ts...>{}, _which
                  , &_storage, &other._storage
                );
            } else {
                detail::destroy{}(
                    detail::pack<detail::empty, Ts...>{}, _which
                  , &_storage
                );
                _which = 0;

                detail::copy_construct{}(
                    detail::pack<detail::empty, Ts...>{}, other._which
                  , &_storage, &other._storage
                );
                _which = other._which;
            }
            return *this;
        }

        variant& operator=(variant&& other)
            noexcept(
                detail::all_of<detail::pack<
                    std::is_nothrow_move_assignable<Ts>...
                  , std::is_nothrow_move_constructible<Ts>...
                >>::value)
        {
            if (_which == other._which)
            {
                detail::move_assign{}(
                    detail::pack<detail::empty, Ts...>{}, _which
                  , &_storage, &other._storage
                );
            } else {
                detail::destroy{}(
                    detail::pack<detail::empty, Ts...>{}, _which
                  , &_storage
                );
                _which = 0;

                detail::move_construct{}(
                    detail::pack<detail::empty, Ts...>{}, other._which
                  , &_storage, &other._storage
                );
                _which = other._which;
            }
            return *this;
        }

        template <typename T, typename ...Args>
        void emplace(Args&&... args)
            noexcept(
                std::is_nothrow_constructible<T, Args&&...>::value)
        {
            detail::destroy{}(
                detail::pack<detail::empty, Ts...>{}, _which
              , &_storage
            );
            _which = 0;

            new (&_storage) T(std::forward<Args>(args)...);
            _which = _index_of<T>::value;
        }

        template <
            typename T, typename U, typename ...Args
          , typename Enable = std::enable_if_t<
                std::is_constructible<T,
                    std::initializer_list<U>&, Args&&...>::value
            >
        >
        void emplace(std::initializer_list<U> il, Args&&... args)
            noexcept(
                std::is_nothrow_constructible<T,
                    std::initializer_list<U>&, Args&&...>::value)
        {
            detail::destroy{}(
                detail::pack<detail::empty, Ts...>{}, _which
              , &_storage
            );
            _which = 0;

            new (&_storage) T(il, std::forward<Args>(args)...);
            _which = _index_of<T>::value;
        }

        void swap(variant& other)
            noexcept(
                detail::all_of<detail::pack<
                    detail::is_nothrow_swappable<Ts>...
                  , std::is_nothrow_move_constructible<Ts>...
                >>::value)
        {
            if (_which == other._which)
            {
                detail::swap{}(
                    detail::pack<detail::empty, Ts...>{}, _which
                  , &_storage, &other._storage
                );
            } else {
                variant tmp{std::move(other)};
                other = std::move(*this);
                *this = std::move(tmp);
            }
        }

        explicit operator bool() const noexcept
        {
            return _which != 0;
        }

        std::size_t which() const noexcept
        {
            return _which != 0 ? _which - 1 : npos;
        }

        friend bool operator==(variant const& left, variant const& right)
        {
            return left._which == right._which
              ? left._which == 0 || detail::equal_to{}(
                    detail::pack<Ts...>{}, left._which - 1
                  , &left._storage, &right._storage
                )
              : false;
        }

        friend bool operator!=(variant const& left, variant const& right)
        {
            return !(left == right);
        }

        friend bool operator<(variant const& left, variant const& right)
        {
            return left._which == right._which
              ? detail::less{}(
                    detail::pack<Ts...>{}, left._which - 1
                  , &left._storage, &right._storage
                )
              : left._which < right._which;
        }

        friend bool operator<=(variant const& left, variant const& right)
        {
            return !(right < left);
        }

        friend bool operator>(variant const& left, variant const& right)
        {
            return right < left;
        }

        friend bool operator>=(variant const& left, variant const& right)
        {
            return !(left < right);
        }

        std::type_info const& target_type() const noexcept
        {
            return _which != 0
              ? detail::type_id{}(
                    detail::pack<Ts...>{}, _which - 1
                )
              : typeid(void);
        }

        template <typename T>
        T* target() noexcept
        {
            return _which == _index_of<T>::value
              ? static_cast<T*>(static_cast<void*>(&_storage))
              : nullptr;
        }

        template <typename T>
        T const* target() const noexcept
        {
            return _which == _index_of<T>::value
              ? static_cast<T const*>(static_cast<void const*>(&_storage))
              : nullptr;
        }

        template <typename R, typename F, typename ...Us>
        friend R apply(F&& f, variant<Us...>& v);

        template <typename R, typename F, typename ...Us>
        friend R apply(F&& f, variant<Us...> const& v);

        template <typename R, typename F, typename ...Us>
        friend R apply(F&& f, variant<Us...>&& v);

    private:
        std::size_t _which;
        std::aligned_union_t<0, Ts...> _storage;
    };

    template <typename R, typename F, typename ...Ts>
    R apply(F&& f, variant<Ts...>& v)
    {
        return v._which != 0
          ? detail::apply<R, F>{}(
                detail::pack<Ts&...>{}, v._which - 1
              , &v._storage, std::forward<F>(f)
            )
          : throw std::bad_cast();
    }

    template <
        typename F, typename ...Ts
      , typename R = detail::weak_result<std::decay_t<F>>
    >
    R apply(F&& f, variant<Ts...>& v)
    {
        return apply<R>(std::forward<F>(f), v);
    }

    template <typename R, typename F, typename ...Ts>
    R apply(F&& f, variant<Ts...> const& v)
    {
        return v._which != 0
          ? detail::apply<R, F>{}(
                detail::pack<Ts const&...>{}, v._which - 1
              , &v._storage, std::forward<F>(f)
            )
          : throw std::bad_cast();
    }

    template <
        typename F, typename ...Ts
      , typename R = detail::weak_result<std::decay_t<F>>
    >
    R apply(F&& f, variant<Ts...> const& v)
    {
        return apply<R>(std::forward<F>(f), v);
    }

    template <typename R, typename F, typename ...Ts>
    R apply(F&& f, variant<Ts...>&& v)
    {
        return v._which != 0
          ? detail::apply<R, F>{}(
                detail::pack<Ts&&...>{}, v._which - 1
              , &v._storage, std::forward<F>(f)
            )
          : throw std::bad_cast();
    }

    template <
        typename F, typename ...Ts
      , typename R = detail::weak_result<std::decay_t<F>>
    >
    R apply(F&& f, variant<Ts...>&& v)
    {
        return apply<R>(std::forward<F>(f), std::move(v));
    }

    template <typename ...Ts>
    void swap(variant<Ts...>& left, variant<Ts...>& right)
        noexcept(noexcept(left.swap(right)))
    {
        left.swap(right);
    }
}}

namespace std
{
    template <typename ...Ts>
    struct hash<::eggs::variants::variant<Ts...>>
    {
        using argument_type = ::eggs::variants::variant<Ts...>;
        using result_type = std::size_t;

        std::size_t operator()(::eggs::variants::variant<Ts...> const& v) const
        {
            return bool(v)
              ? ::eggs::variants::apply<std::size_t>(
                    ::eggs::variants::detail::hash{}, v)
              : 0u;
        }
    };
}

#endif /*EGGS_VARIANT_VARIANT_HPP*/
