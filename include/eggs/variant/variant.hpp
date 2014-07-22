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
#include <stdexcept>
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
          : decltype(_at_index<I>(_indexer<Ts>{}))
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
                using value_type = std::remove_cv_t<std::remove_reference_t<T>>;
                return _void_guard<R>(), _invoke(
                    std::forward<F>(f)
                  , std::forward<T>(*static_cast<value_type*>(ptr)));
            }

            template <typename T>
            static R call(void const* ptr, F&& f)
            {
                using value_type = std::remove_cv_t<std::remove_reference_t<T>> const;
                return _void_guard<R>(), _invoke(
                    std::forward<F>(f)
                  , std::forward<T>(*static_cast<value_type*>(ptr)));
            }

            //~ workaround for gcc and msvc issues with multiple inherited operator()
            //~ \see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61726
            //~ \see https://connect.microsoft.com/VisualStudio/feedback/details/914574
            using visitor<apply<R, F>, R(void*, F&&)>::operator();
            using visitor<apply<R, F>, R(void const*, F&&)>::operator();
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
    //! The struct `in_place_t` is an empty structure type used as a unique
    //! type to disambiguate constructor and function overloading.
    //! Specifically, `variant<Ts...>` has a constructor with an unspecified
    //! first parameter that matches an expression of the form `in_place<T>`,
    //! followed by a parameter pack; this indicates that `T` should be
    //! constructed in-place (as if by a call to a placement new expression)
    //! with the forwarded pack expansion as arguments for the initialization
    //! of `T`.
    struct in_place_t {};

    template <std::size_t I>
    inline in_place_t in_place(detail::pack_c<std::size_t, I> = {})
    {
        return {};
    }

    template <typename T>
    inline in_place_t in_place(detail::pack<T> = {})
    {
        return {};
    }

    ///////////////////////////////////////////////////////////////////////////
    //! The struct `nullvariant_t` is an empty structure type used as a unique
    //! type to indicate the state of not having an active member for `variant`
    //! objects. In particular, `variant<Ts...>` has a constructor with
    //! `nullvariant_t` as a single argument; this indicates that a variant
    //! object with no active member shall be constructed.
    //!
    //! Type `nullvariant_t` shall not have a default constructor. It shall be
    //! a literal type. Constant `nullvariant` shall be initialized with an
    //! argument of literal type.
    struct nullvariant_t
    {
        nullvariant_t() = delete;

        constexpr explicit nullvariant_t(detail::empty) {}
    };

    constexpr nullvariant_t nullvariant{{}};

    ///////////////////////////////////////////////////////////////////////////
    //! The class `bad_variant_access` defines the type of objects thrown as
    //! exceptions to report the situation where an attempt is made to access
    //! an inactive member of a `variant` object.
    class bad_variant_access
      : public std::logic_error
    {
    public:
        //! \effects Constructs an object of class `bad_variant_access`.
        //!  `what()` returns an implementation-defined NTBS.
        bad_variant_access()
          : std::logic_error{"bad_variant_access"}
        {};

        //! \effects Constructs an object of class `bad_variant_access`.
        //!  `strcmp(what(), what_arg.c_str()) == 0`.
        explicit bad_variant_access(std::string const& what_arg)
          : std::logic_error{what_arg}
        {};

        //! \effects Constructs an object of class `bad_variant_access`.
        //!  `strcmp(what(), what_arg) == 0`.
        explicit bad_variant_access(char const* what_arg)
          : std::logic_error{what_arg}
        {};
    };

    ///////////////////////////////////////////////////////////////////////////
    //! In a `variant`, at most one of the members can be active at any time,
    //! that is, the value of at most one of the members can be stored in a
    //! `variant` at any time.  Implementations are not permitted to use
    //! additional storage, such as dynamic memory, to allocate its contained
    //! value. The contained value shall be allocated in a region of the
    //! `variant<Ts...>` storage suitably aligned for the types `Ts...`.
    //!
    //! All `T` in `Ts...` shall be object types and shall satisfy the
    //! requirements of `Destructible`.
    template <typename ...Ts>
    class variant
    {
        static_assert(
            !detail::any_of<detail::pack<
                std::is_function<Ts>...>>::value
          , "variant member has function type");

        static_assert(
            !detail::any_of<detail::pack<
                std::is_reference<Ts>...>>::value
          , "variant member has reference type");

        static_assert(
            !detail::any_of<detail::pack<
                std::is_void<Ts>...>>::value
          , "variant member has void type");

        static_assert(
            !detail::any_of<detail::pack<
                std::is_same<std::remove_cv_t<Ts>, nullvariant_t>...>>::value
          , "variant member has nullvariant_t type");

    public:
        static constexpr std::size_t npos = std::size_t(-1);

    public:
        //! \postconditions `*this` does not have an active member.
        //!
        //! \remarks No member is initialized.
        variant() noexcept
          : _which{0}
        {}

        //! \postconditions `*this` does not have an active member.
        //!
        //! \remarks No member is initialized.
        variant(nullvariant_t) noexcept
          : _which{0}
        {}

        //! \requires `std::is_copy_constructible_v<T>` is `true` for all `T`
        //!  in `Ts...`.
        //!
        //! \effects If `rhs` has an active member of type `T`, initializes
        //!  the active member as if direct-non-list-initializing an object of
        //!  type `T` with the expression `*rhs.target<T>()`.
        //!
        //! \postconditions `rhs.which() == this->which()`.
        //!
        //! \throws Any exception thrown by the selected constructor of `T`.
        variant(variant const& rhs)
            noexcept(
                detail::all_of<detail::pack<
                    std::is_nothrow_copy_constructible<Ts>...
                >>::value)
          : _which{rhs._which}
        {
            detail::copy_construct{}(
                detail::pack<detail::empty, Ts...>{}, _which
              , &_storage, &rhs._storage
            );
        }

        //! \requires `std::is_move_constructible_v<T>` is `true` for all `T`
        //!  in `Ts...`.
        //!
        //! \effects If `rhs` has an active member of type `T`, initializes
        //!  the active member as if direct-non-list-initializing an object of
        //!  type `T` with the expression `std::move(*rhs.target<T>())`.
        //!  `bool(rhs)` is unchanged.
        //!
        //! \postconditions `rhs.which() == this->which()`.
        //!
        //! \throws Any exception thrown by the selected constructor of `T`.
        variant(variant&& rhs)
            noexcept(
                detail::all_of<detail::pack<
                    std::is_nothrow_move_constructible<Ts>...
                >>::value)
          : _which{rhs._which}
        {
            detail::move_construct{}(
                detail::pack<detail::empty, Ts...>{}, _which
              , &_storage, &rhs._storage
            );
        }

        //! Let `T` be `std::remove_cv_t<std::remove_reference_t<U>>`
        //!
        //! \requires `std::is_constructible_v<T, U&&>` is `true`.
        //!
        //! \effects Initializes the active member as if direct-non-list-
        //!  initializing an object of type `T` with the expression
        //!  `std::forward<U>(v)`.
        //!
        //! \postconditions `*this` has an active member.
        //!
        //! \throws Any exception thrown by the selected constructor of `T`.
        //!
        //! \remarks This constructor shall not participate in overload
        //!  resolution unless `T` occurs exactly once in
        //!  `std::remove_cv_t<Ts>...`.
        template <
            typename U
          , typename T = std::remove_cv_t<std::remove_reference_t<U>>
          , typename Enable = std::enable_if_t<
                detail::contains<T, detail::pack<std::remove_cv_t<Ts>...>>::value
            >
        > variant(U&& v)
            noexcept(
                std::is_nothrow_constructible<T, U&&>::value)
        {
            new (&_storage) T(std::forward<U>(v));
            _which = detail::index_of<
                T, detail::pack<std::remove_cv_t<Ts>...>>::value + 1;
        }

        //! Let `T` the `I`th element in `Ts...`, where indexing is zero-based.
        //!
        //! \requires `I < sizeof...(Ts)` and `std::is_constructible_v<T,
        //!  Args&&...>` is `true`.
        //!
        //! \effects Initializes the active member as if direct-non-list-
        //!  initializing an object of type `T` with the arguments
        //!  `std::forward<Args>(args)...`.
        //!
        //! \postconditions `*this` has an active member of type `T`.
        //!
        //! \throws Any exception thrown by the selected constructor of `T`.
        //!
        //! \remarks The first argument shall be the expression `in_place<I>`.
        template <
            std::size_t I, typename ...Args
          , typename T = typename detail::at_index<
                I, detail::pack<Ts...>>::type
        >
        explicit variant(
            in_place_t(detail::pack_c<std::size_t, I>)
          , Args&&... args)
            noexcept(
                std::is_nothrow_constructible<T, Args&&...>::value)
        {
            new (&_storage) T(std::forward<Args>(args)...);
            _which = I + 1;
        }

        //! Let `T` the `I`th element in `Ts...`, where indexing is zero-based.
        //!
        //! \requires `I < sizeof...(Ts)` and  `std::is_constructible_v<T,
        //!  initializer_list<U>&, Args&&...>` is `true`.
        //!
        //! \effects Initializes the active member as if direct-non-list-
        //!  initializing an object of type `T` with the arguments `il,
        //!  std::forward<Args>(args)...`.
        //!
        //! \postconditions `*this` has an active member of type `T`.
        //!
        //! \throws Any exception thrown by the selected constructor of `T`.
        //!
        //! \remarks The first argument shall be the expression `in_place<I>`.
        //!  This function shall not participate in overload resolution unless
        //!  `is_constructible_v<T, initializer_list<U>&, Args&&...>` is
        //!  `true`.
        template <
            std::size_t I, typename U, typename ...Args
          , typename T = typename detail::at_index<
                I, detail::pack<Ts...>>::type
          , typename Enable = std::enable_if_t<
                std::is_constructible<T,
                    std::initializer_list<U>&, Args&&...>::value
            >
        >
        explicit variant(
            in_place_t(detail::pack_c<std::size_t, I>)
          , std::initializer_list<U> il, Args&&... args)
            noexcept(
                std::is_nothrow_constructible<T,
                    std::initializer_list<U>&, Args&&...>::value)
        {
            new (&_storage) T(il, std::forward<Args>(args)...);
            _which = I + 1;
        }

        //! \requires `T` shall occur exactly once in `Ts...`.
        //!
        //! \effects Equivalent to `variant(in_place<I>,
        //!  std::forward<Args>(args)...)` where `I` is the zero-based index
        //!  of `T` in `Ts...`.
        //!
        //! \remarks The first argument shall be the expression `in_place<T>`.
        template <typename T, typename ...Args>
        explicit variant(
            in_place_t(detail::pack<T>)
          , Args&&... args)
            noexcept(
                std::is_nothrow_constructible<T, Args&&...>::value)
        {
            new (&_storage) T(std::forward<Args>(args)...);
            _which = detail::index_of<T, detail::pack<Ts...>>::value + 1;
        }

        //! \requires `T` shall occur exactly once in `Ts...`.
        //!
        //! \effects Equivalent to `variant(in_place<I>, il,
        //!  std::forward<Args>(args)...)` where `I` is the zero-based index
        //!  of `T` in `Ts...`.
        //!
        //! \remarks The first argument shall be the expression `in_place<T>`.
        //!  This function shall not participate in overload resolution unless
        //!  `is_constructible_v<T, initializer_list<U>&, Args&&...>` is
        //!  `true`.
        template <
            typename T, typename U, typename ...Args
          , typename Enable = std::enable_if_t<
                std::is_constructible<T,
                    std::initializer_list<U>&, Args&&...>::value
            >
        >
        explicit variant(
            in_place_t(detail::pack<T>)
          , std::initializer_list<U> il, Args&&... args)
            noexcept(
                std::is_nothrow_constructible<T,
                    std::initializer_list<U>&, Args&&...>::value)
        {
            new (&_storage) T(il, std::forward<Args>(args)...);
            _which = detail::index_of<T, detail::pack<Ts...>>::value + 1;
        }

        //! \effects If `*this` has an active member of type `T`, destroys the
        //!  active member by calling `T::~T()`.
        ~variant()
        {
            detail::destroy{}(
                detail::pack<detail::empty, Ts...>{}, _which
              , &_storage
            );
        }

        //! \effects If `*this` has an active member of type `T`, destroys the
        //!  active member by calling `T::~T()`.
        //!
        //! \returns `*this`.
        //!
        //! \postconditions `*this` does not have an active member.
        //!
        variant& operator=(nullvariant_t) noexcept
        {
            detail::destroy{}(
                detail::pack<detail::empty, Ts...>{}, _which
              , &_storage
            );
            _which = 0;

            return *this;
        }

        //! \requires `std::is_copy_constructible_v<T>` and
        //!  `std::is_copy_assignable_v<T>` is `true` for all `T` in `Ts...`.
        //!
        //! \effects
        //!  - If both `*this` and `rhs` have an active member of type `T`,
        //!    assigns to the active member the expression `*rhs.target<T>()`;
        //!
        //!  - otherwise, calls `*this = nullvariant`. Then, if `rhs` has an
        //!    active member of type `T`, initializes the active member as if
        //!    direct-non-list-initializing an object of type `T` with the
        //!    expression `*rhs.target<T>()`.
        //!
        //! \returns `*this`.
        //!
        //! \postconditions `rhs.which() == this->which()`.
        //!
        //! \exceptionsafety If an exception is thrown during the call to
        //!  `T`'s copy assignment, the state of the active member is as
        //!  defined by the exception safety guarantee of `T`'s copy
        //!  assignment. If an exception is thrown during the call to `T`'s
        //!  copy constructor, `*this` has no active member, and the previous
        //!  active member (if any) has been destroyed.
        variant& operator=(variant const& rhs)
            noexcept(
                detail::all_of<detail::pack<
                    std::is_nothrow_copy_assignable<Ts>...
                  , std::is_nothrow_copy_constructible<Ts>...
                >>::value)
        {
            if (_which == rhs._which)
            {
                detail::copy_assign{}(
                    detail::pack<detail::empty, Ts...>{}, _which
                  , &_storage, &rhs._storage
                );
            } else {
                *this = nullvariant;

                detail::copy_construct{}(
                    detail::pack<detail::empty, Ts...>{}, rhs._which
                  , &_storage, &rhs._storage
                );
                _which = rhs._which;
            }
            return *this;
        }

        //! \requires `std::is_move_constructible_v<T>` and
        //!  `std::is_move_assignable_v<T>` is `true` for all `T` in `Ts...`.
        //!
        //! \effects
        //!  - If both `*this` and `rhs` have an active member of type `T`,
        //!    assigns to the active member the expression
        //!    `std::move(*rhs.target<T>())`;
        //!
        //!  - otherwise, calls `*this = nullvariant`. Then, if `rhs` has an
        //!    active member of type `T`, initializes the active member as if
        //!    direct-non-list-initializing an object of type `T` with the
        //!    expression `std::move(*rhs.target<Tn>())`.
        //!
        //!  `bool(rhs)` is unchanged.
        //!
        //! \returns `*this`.
        //!
        //! \postconditions `rhs.which() == this->which()`.
        //!
        //! \exceptionsafety If an exception is thrown during the call to
        //!  `T`'s move assignment, the state of both active members is
        //!  determined by the exception safety guarantee of `T`'s move
        //!  assignment. If an exception is thrown during the call to `T`'s
        //!  move constructor, `*this` has no active member, the previous
        //!  active member (if any) has been destroyed, and the state of the
        //!  active member of `rhs` is determined by the exception safety
        //!  guarantee of `T`'s move constructor.
        variant& operator=(variant&& rhs)
            noexcept(
                detail::all_of<detail::pack<
                    std::is_nothrow_move_assignable<Ts>...
                  , std::is_nothrow_move_constructible<Ts>...
                >>::value)
        {
            if (_which == rhs._which)
            {
                detail::move_assign{}(
                    detail::pack<detail::empty, Ts...>{}, _which
                  , &_storage, &rhs._storage
                );
            } else {
                *this = nullvariant;

                detail::move_construct{}(
                    detail::pack<detail::empty, Ts...>{}, rhs._which
                  , &_storage, &rhs._storage
                );
                _which = rhs._which;
            }
            return *this;
        }

        //! Let `T` be `std::remove_cv_t<std::remove_reference_t<U>>`
        //!
        //! \requires `std::is_constructible_v<T, U&&>` and
        //!  `std::is_assignable_v<T, U&&>` are `true`.
        //!
        //! \effects
        //!  - If `*this` has an active member of type `T`, assigns to the
        //!    active member the expression `std::forward<U>(v)`;
        //!
        //!  - otherwise, calls `*this = nullvariant`. Then, initializes the
        //!    active member as if direct-non-list-initializing an object of
        //!    type `T` with the expression `std::forward<U>(v)`.
        //!
        //! \returns `*this`.
        //!
        //! \postconditions `*this` has an active member.
        //!
        //! \exceptionsafety If an exception is thrown during the call to
        //!  `T`'s assignment, the state of the active member is as defined
        //!  by the exception safety guarantee of `T`'s copy assignment. If
        //!  an exception is thrown during the call to `T`'s constructor,
        //!  `*this` has no active member, and the previous active member
        //!  (if any) has been destroyed.
        //!
        //! \remarks This operator shall not participate in overload
        //!  resolution unless `T` occurs exactly once in
        //!  `std::remove_cv_t<Ts>...`.
        template <
            typename U
          , typename T = std::remove_cv_t<std::remove_reference_t<U>>
          , typename Enable = std::enable_if_t<
                detail::contains<T, detail::pack<std::remove_cv_t<Ts>...>>::value
            >
        >
        variant& operator=(U&& v)
            noexcept(
                std::is_nothrow_assignable<T, U&&>::value
             && std::is_nothrow_constructible<T, U&&>::value)
        {
            static constexpr std::size_t t_which = detail::index_of<
                T, detail::pack<std::remove_cv_t<Ts>...>>::value + 1;

            if (_which == t_which)
            {
                T* active_member_ptr =
                    static_cast<T*>(static_cast<void*>(&_storage));

                *active_member_ptr = std::forward<U>(v);
            } else {
                *this = nullvariant;

                new (&_storage) T(std::forward<U>(v));
                _which = t_which;
            }
            return *this;
        }

        //! Let `T` the `I`th element in `Ts...`, where indexing is zero-based.
        //!
        //! \requires `I < sizeof...(Ts)` and `std::is_constructible_v<T,
        //!  Args&&...>` is `true`.
        //!
        //! \effects Calls `*this = nullvariant`. Then, initializes the active
        //!  member as if direct-non-list-initializing  an object of type `T`
        //!  with the arguments `std::forward<Args>(args)...`.
        //!
        //! \postconditions `*this` has an active member of type `T`.
        //!
        //! \throws Any exception thrown by the selected constructor of `T`.
        //!
        //! \exceptionsafety If an exception is thrown during the call to
        //!  `T`'s constructor, `*this` has no active member, and the previous
        //!  active member (if any) has been destroyed.
        template <
            std::size_t I, typename ...Args
          , typename T = typename detail::at_index<
                I, detail::pack<Ts...>>::type
        >
        void emplace(Args&&... args)
            noexcept(
                std::is_nothrow_constructible<T, Args&&...>::value)
        {
            *this = nullvariant;

            new (&_storage) T(std::forward<Args>(args)...);
            _which = I + 1;
        }

        //! Let `T` the `I`th element in `Ts...`, where indexing is zero-based.
        //!
        //! \requires `I < sizeof...(Ts)` and  `std::is_constructible_v<T,
        //!  initializer_list<U>&, Args&&...>` is `true`.
        //!
        //! \effects Calls `*this = nullvariant`. Then, initializes the active
        //!  member as if direct-non-list-initializing an object of type `T`
        //!  with the arguments `il, std::forward<Args>(args)...`.
        //!
        //! \postconditions `*this` has an active member of type `T`.
        //!
        //! \throws Any exception thrown by the selected constructor of `T`.
        //!
        //! \exceptionsafety If an exception is thrown during the call to
        //!  `T`'s constructor, `*this` has no active member, and the previous
        //!  active member (if any) has been destroyed.
        //!
        //! \remarks This function shall not participate in overload resolution
        //!  unless `is_constructible_v<T, initializer_list<U>&, Args&&...>`
        //!  is `true`.
        template <
            std::size_t I, typename U, typename ...Args
          , typename T = typename detail::at_index<
                I, detail::pack<Ts...>>::type
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
            *this = nullvariant;

            new (&_storage) T(il, std::forward<Args>(args)...);
            _which = I + 1;
        }

        //! \requires `T` shall occur exactly once in `Ts...`.
        //!
        //! \effects Equivalent to `emplace<I>(std::forward<Args>(args)...)`
        //!  where `I` is the zero-based index of `T` in `Ts...`.
        template <typename T, typename ...Args>
        void emplace(Args&&... args)
            noexcept(
                std::is_nothrow_constructible<T, Args&&...>::value)
        {
            return emplace<detail::index_of<T, detail::pack<Ts...>>::value>(
                std::forward<Args>(args)...);
        }

        //! \requires `T` shall occur exactly once in `Ts...`.
        //!
        //! \effects Equivalent to `emplace<I>(il, std::forward<Args>(args)...)`
        //!  where `I` is the zero-based index of `T` in `Ts...`.
        //!
        //! \remarks This function shall not participate in overload resolution
        //!  unless `is_constructible_v<T, initializer_list<U>&, Args&&...>`
        //!  is `true`.
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
            return emplace<detail::index_of<T, detail::pack<Ts...>>::value>(
                il, std::forward<Args>(args)...);
        }

        //! \requires Lvalues of `T` shall be swappable and
        //!  `std::is_move_constructible_v<T>` is `true` for all `T` in
        //!  `Ts...`.
        //!
        //! \effects
        //!  - If both `*this` and `rhs` have an active member of type `T`,
        //!    calls `swap(*this->target<T>(), *rhs.target<T>())`;
        //!
        //!  - otherwise, calls `std::swap(*this, rhs)`.
        //!
        //! \exceptionsafety If an exception is thrown during the call to
        //!  function `swap` the state of the active members of type `T` is
        //!  determined by the exception safety guarantee of `swap` for
        //!  lvalues of `T`. If an exception is thrown during the call to
        //!  a move constructor, the state of `*this` and `rhs` is
        //!  unspecified.
        void swap(variant& rhs)
            noexcept(
                detail::all_of<detail::pack<
                    detail::is_nothrow_swappable<Ts>...
                  , std::is_nothrow_move_constructible<Ts>...
                >>::value)
        {
            if (_which == rhs._which)
            {
                detail::swap{}(
                    detail::pack<detail::empty, Ts...>{}, _which
                  , &_storage, &rhs._storage
                );
            } else {
                std::swap(*this, rhs);
            }
        }

        //! \returns `true` if and only if `*this` has an active member.
        explicit operator bool() const noexcept
        {
            return _which != 0;
        }

        //! \returns The zero-based index of the active member if `*this` has
        //!  one. Otherwise, returns `npos`.
        std::size_t which() const noexcept
        {
            return _which != 0 ? _which - 1 : npos;
        }

        //! \returns If `*this` has an active member of type `T`, `typeid(T)`;
        //!  otherwise `typeid(void)`.
        std::type_info const& target_type() const noexcept
        {
            return _which != 0
              ? detail::type_id{}(
                    detail::pack<Ts...>{}, _which - 1
                )
              : typeid(void);
        }

        //! \returns If `*this` has an active member, a pointer to the active
        //!  member; otherwise a null pointer.
        void* target() noexcept
        {
            return _which != 0
              ? static_cast<void*>(&_storage)
              : nullptr;
        }

        //! \returns If `*this` has an active member, a pointer to the active
        //!  member; otherwise a null pointer.
        void const* target() const noexcept
        {
            return _which != 0
              ? static_cast<void const*>(&_storage)
              : nullptr;
        }

        //! \requires `T` shall occur exactly once in `Ts...`.
        //!
        //! \returns If `*this` has an active member of type `T`, a pointer to
        //!  the active member; otherwise a null pointer.
        template <typename T>
        T* target() noexcept
        {
            static constexpr std::size_t t_which = detail::index_of<
                T, detail::pack<Ts...>>::value + 1;

            return _which == t_which
              ? static_cast<T*>(static_cast<void*>(&_storage))
              : nullptr;
        }

        //! \requires `T` shall occur exactly once in `Ts...`.
        //!
        //! \returns If `*this` has an active member of type `T`, a pointer to
        //!  the active member; otherwise a null pointer.
        template <typename T>
        T const* target() const noexcept
        {
            static constexpr std::size_t t_which = detail::index_of<
                T, detail::pack<Ts...>>::value + 1;

            return _which == t_which
              ? static_cast<T const*>(static_cast<void const*>(&_storage))
              : nullptr;
        }

    private:
        std::size_t _which;
        std::aligned_union_t<0, Ts...> _storage;
    };

    ///////////////////////////////////////////////////////////////////////////
    //! \remarks All specializations of `variant_size<T>` shall meet the
    //!  `UnaryTypeTrait` requirements with a `BaseCharacteristic` of
    //!  `std::integral_constant<std::size_t, N>` for some `N`.
    template <typename T>
    struct variant_size; // undefined

    //! \remarks Has a `BaseCharacteristic` of `std::integral_constant<
    //!  std::size_t, sizeof...(Ts)>`.
    template <typename ...Ts>
    struct variant_size<variant<Ts...>>
      : std::integral_constant<std::size_t, sizeof...(Ts)>
    {};

    //! \remarks Let `VS` denote `variant_size<T>` of the cv-unqualified type
    //!  `T`. Has a `BaseCharacteristic` of `std::integral_constant<
    //!  std::size_t, VS::value>`
    template <typename T>
    struct variant_size<T const>
      : variant_size<T>
    {};

    template <typename T>
    constexpr std::size_t variant_size_v = variant_size<T>::value;

    ///////////////////////////////////////////////////////////////////////////
    //! \remarks All specializations of `variant_element<I, T>` shall meet the
    //!  `TransformationTrait` requirements with a member typedef `type` that
    //!  names the `I`th member of `T`, where indexing is zero-based.
    template <std::size_t I, typename T>
    struct variant_element; // undefined

    //! \requires `I < sizeof...(Ts)`.
    //!
    //! \remarks The member typedef `type` shall name the type of the `I`th
    //!  element of `Ts...`, where indexing is zero-based.
    template <std::size_t I, typename ...Ts>
    struct variant_element<I, variant<Ts...>>
      : detail::at_index<I, detail::pack<Ts...>>
    {};

    //! \remarks Let `VE` denote `variant_element<I, T>` of the cv-unqualified
    //!  type `T`. The member typedef `type` names `std::add_const_t<
    //!  typename VE::type>`.
    template <std::size_t I, typename T>
    struct variant_element<I, T const>
      : std::add_const<typename variant_element<I, T>::type>
    {};

    template <std::size_t I, typename T>
    using variant_element_t =  typename variant_element<I, T>::type;

    ///////////////////////////////////////////////////////////////////////////
    //! \requires `I < sizeof...(Ts)`.
    //!
    //! \returns A reference to the `I`th member of `v` if it is active, where
    //!  indexing is zero-based.
    //!
    //! \throws `bad_variant_access` if the `I`th member of `v` is not active.
    template <std::size_t I, typename ...Ts>
    variant_element_t<I, variant<Ts...>>& get(variant<Ts...>& v)
    {
        using value_type = variant_element_t<I, variant<Ts...>>;
        if (value_type* value = v.template target<value_type>())
            return *value;
        throw bad_variant_access{};
    }

    //! \requires `I < sizeof...(Ts)`.
    //!
    //! \returns A const reference to the `I`th member of `v` if it is active,
    //!  where indexing is zero-based.
    //!
    //! \throws `bad_variant_access` if the `I`th member of `v` is not active.
    template <std::size_t I, typename ...Ts>
    variant_element_t<I, variant<Ts...>> const& get(variant<Ts...> const& v)
    {
        using value_type = variant_element_t<I, variant<Ts...>>;
        if (value_type const* value = v.template target<value_type>())
            return *value;
        throw bad_variant_access{};
    }

    //! \effects Equivalent to return `std::forward<variant_element_t<I,
    //!  variant<Ts...>>&&>(get<I>(v))`.
    template <std::size_t I, typename ...Ts>
    variant_element_t<I, variant<Ts...>>&& get(variant<Ts...>&& v)
    {
        using value_type = variant_element_t<I, variant<Ts...>>;
        return std::forward<value_type&&>(get<I>(v));
    }

    //! \requires The type `T` occurs exactly once in `Ts...`.
    //!
    //! \returns A reference to the active member of `v` if it is of type `T`.
    //!
    //! \throws `bad_variant_access` if the active member of `v` is not of
    //!  type `T`.
    template <typename T, typename ...Ts>
    T& get(variant<Ts...>& v)
    {
        if (T* value = v.template target<T>())
            return *value;
        throw bad_variant_access{};
    }

    //! \requires The type `T` occurs exactly once in `Ts...`.
    //!
    //! \returns A const reference to the active member of `v` if it is of
    //!  type `T`.
    //!
    //! \throws `bad_variant_access` if the active member of `v` is not of
    //!  type `T`.
    template <typename T, typename ...Ts>
    T const& get(variant<Ts...> const& v)
    {
        if (T const* value = v.template target<T>())
            return *value;
        throw bad_variant_access{};
    }

    //! \effects Equivalent to return `std::forward<T&&>(get<T>(v))`.
    template <typename T, typename ...Ts>
    T&& get(variant<Ts...>&& v)
    {
        return std::forward<T&&>(get<T>(v));
    }

    ///////////////////////////////////////////////////////////////////////////
    //! \requires All `T` in `Ts...` shall meet the requirements of
    //!  `EqualityComparable`.
    //!
    //! \returns If both `lhs` and `rhs` have an active member of type `T`,
    //!  `*lhs.target<T>() == *rhs.target<T>()`; otherwise, if
    //!  `bool(lhs) == bool(rhs)`, `true`; otherwise, `false`.
    template <typename ...Ts>
    bool operator==(variant<Ts...> const& lhs, variant<Ts...> const& rhs)
    {
        return lhs.which() == rhs.which()
          ? !bool(lhs) || detail::equal_to{}(
                detail::pack<Ts...>{}, lhs.which()
              , lhs.target(), rhs.target()
            )
          : false;
    }

    //! \returns `!(lhs == rhs)`.
    template <typename ...Ts>
    bool operator!=(variant<Ts...> const& lhs, variant<Ts...> const& rhs)
    {
        return !(lhs == rhs);
    }

    //! \requires `T` shall meet the requirements of `EqualityComparable`.
    //!
    //! \returns If `lhs` has an active member of type `T`,
    //!  `*lhs.target<T>() == rhs`; otherwise, `false`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename ...Ts, typename T
      , typename Enable = std::enable_if_t<
            detail::contains<T, detail::pack<std::remove_cv_t<Ts>...>>::value
        >
    >
    bool operator==(variant<Ts...> const& lhs, T const& rhs)
    {
        static constexpr std::size_t rhs_which =
            detail::index_of<T, detail::pack<std::remove_cv_t<Ts>...>>::value;

        return lhs.which() == rhs_which
          ? *lhs.template target<T>() == rhs
          : false;
    }

    //! \returns `rhs == lhs`
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename T, typename ...Ts
      , typename Enable = std::enable_if_t<
            detail::contains<T, detail::pack<std::remove_cv_t<Ts>...>>::value
        >
    >
    bool operator==(T const& lhs, variant<Ts...> const& rhs)
    {
        return rhs == lhs;
    }

    //! \returns `!(lhs == rhs)`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename ...Ts, typename T
      , typename Enable = std::enable_if_t<
            detail::contains<T, detail::pack<std::remove_cv_t<Ts>...>>::value
        >
    >
    bool operator!=(variant<Ts...> const& lhs, T const& rhs)
    {
        return !(lhs == rhs);
    }

    //! \returns `!(lhs == rhs)`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename T, typename ...Ts
      , typename Enable = std::enable_if_t<
            detail::contains<T, detail::pack<std::remove_cv_t<Ts>...>>::value
        >
    >
    bool operator!=(T const& lhs, variant<Ts...> const& rhs)
    {
        return !(lhs == rhs);
    }

    //! \returns `!x`
    template <typename ...Ts>
    bool operator==(variant<Ts...>& x, nullvariant_t) noexcept
    {
        return !x;
    }

    //! \returns `!x`
    template <typename ...Ts>
    bool operator==(nullvariant_t, variant<Ts...>& x) noexcept
    {
        return !x;
    }

    //! \returns `bool(x)`
    template <typename ...Ts>
    bool operator!=(variant<Ts...>& x, nullvariant_t) noexcept
    {
        return bool(x);
    }

    //! \returns `bool(x)`
    template <typename ...Ts>
    bool operator!=(nullvariant_t, variant<Ts...>& x) noexcept
    {
        return bool(x);
    }

    //! \requires All `T` in `Ts...` shall meet the requirements of
    //!  `LessThanComparable`.
    //!
    //! \returns If both `lhs` and `rhs` have an active member of type `T`,
    //!  `*lhs.target<T>() < *rhs.target<T>()`; otherwise, if
    //!  `!bool(rhs)`, `false`; otherwise, if `!bool(lhs)`, `true`; otherwise,
    //!  `lhs.which() < rhs.which()`.
    template <typename ...Ts>
    bool operator<(variant<Ts...> const& lhs, variant<Ts...> const& rhs)
    {
        return lhs.which() == rhs.which()
          ? bool(lhs) && detail::less{}(
                detail::pack<Ts...>{}, lhs.which()
              , lhs.target(), rhs.target()
            )
          : bool(lhs) == bool(rhs) ? lhs.which() < rhs.which() : bool(rhs);
    }

    //! \returns `rhs < lhs`.
    template <typename ...Ts>
    bool operator>(variant<Ts...> const& lhs, variant<Ts...> const& rhs)
    {
        return rhs < lhs;
    }

    //! \returns `!(rhs < lhs)`.
    template <typename ...Ts>
    bool operator<=(variant<Ts...> const& lhs, variant<Ts...> const& rhs)
    {
        return !(rhs < lhs);
    }

    //! \returns `!(lhs < rhs)`.
    template <typename ...Ts>
    bool operator>=(variant<Ts...> const& lhs, variant<Ts...> const& rhs)
    {
        return !(lhs < rhs);
    }

    //! \requires `T` shall meet the requirements of `LessThanComparable`.
    //!
    //! \returns If `lhs` has an active member of type `T`,
    //!  `*lhs->target<T>() < rhs`; otherwise, if `lhs` has no active member
    //!  or if `lhs` has an active member of type `Td` and `Td` occurs in
    //!  `Ts...` before `T`, `true`; otherwise, `false`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename ...Ts, typename T
      , typename Enable = std::enable_if_t<
            detail::contains<T, detail::pack<std::remove_cv_t<Ts>...>>::value
        >
    >
    bool operator<(variant<Ts...> const& lhs, T const& rhs)
    {
        static constexpr std::size_t rhs_which =
            detail::index_of<T, detail::pack<std::remove_cv_t<Ts>...>>::value;

        return lhs.which() == rhs_which
          ? *lhs.template target<T>() < rhs
          : bool(lhs) ? lhs.which() < rhs_which : true;
    }

    //! \requires `T` shall meet the requirements of `LessThanComparable`.
    //!
    //! \returns If `rhs` has an active member of type `T`,
    //!  `lhs < *rhs->target<T>(); otherwise, if `rhs` has an active member
    //!  of type `Td` and `Td` occurs in `Ts...` after `T`, `true`;
    //!  otherwise, `false`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename T, typename ...Ts
      , typename Enable = std::enable_if_t<
            detail::contains<T, detail::pack<std::remove_cv_t<Ts>...>>::value
        >
    >
    bool operator<(T const& lhs, variant<Ts...> const& rhs)
    {
        static constexpr std::size_t lhs_which =
            detail::index_of<T, detail::pack<std::remove_cv_t<Ts>...>>::value;

        return lhs_which == rhs.which()
          ? lhs < *rhs.template target<T>()
          : bool(rhs) ? lhs_which < rhs.which() : false;
    }

    //! \returns `rhs < lhs`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename ...Ts, typename T
      , typename Enable = std::enable_if_t<
            detail::contains<T, detail::pack<std::remove_cv_t<Ts>...>>::value
        >
    >
    bool operator>(variant<Ts...> const& lhs, T const& rhs)
    {
        return rhs < lhs;
    }

    //! \returns `rhs < lhs`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename T, typename ...Ts
      , typename Enable = std::enable_if_t<
            detail::contains<T, detail::pack<std::remove_cv_t<Ts>...>>::value
        >
    >
    bool operator>(T const& lhs, variant<Ts...> const& rhs)
    {
        return rhs < lhs;
    }

    //! \returns `!(rhs < lhs)`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename ...Ts, typename T
      , typename Enable = std::enable_if_t<
            detail::contains<T, detail::pack<std::remove_cv_t<Ts>...>>::value
        >
    >
    bool operator<=(variant<Ts...> const& lhs, T const& rhs)
    {
        return !(rhs < lhs);
    }

    //! \returns `!(rhs < lhs)`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename T, typename ...Ts
      , typename Enable = std::enable_if_t<
            detail::contains<T, detail::pack<std::remove_cv_t<Ts>...>>::value
        >
    >
    bool operator<=(T const& lhs, variant<Ts...> const& rhs)
    {
        return !(rhs < lhs);
    }

    //! \returns `!(lhs < rhs)`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename ...Ts, typename T
      , typename Enable = std::enable_if_t<
            detail::contains<T, detail::pack<std::remove_cv_t<Ts>...>>::value
        >
    >
    bool operator>=(variant<Ts...> const& lhs, T const& rhs)
    {
        return !(lhs < rhs);
    }

    //! \returns `!(lhs < rhs)`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename T, typename ...Ts
      , typename Enable = std::enable_if_t<
            detail::contains<T, detail::pack<std::remove_cv_t<Ts>...>>::value
        >
    >
    bool operator>=(T const& lhs, variant<Ts...> const& rhs)
    {
        return !(lhs < rhs);
    }

    //! \returns `false`
    template <typename ...Ts>
    bool operator<(variant<Ts...>& /*x*/, nullvariant_t) noexcept
    {
        return false;
    }

    //! \returns `bool(x)`
    template <typename ...Ts>
    bool operator<(nullvariant_t, variant<Ts...>& x) noexcept
    {
        return bool(x);
    }

    //! \returns `bool(x)`
    template <typename ...Ts>
    bool operator>(variant<Ts...>& x, nullvariant_t) noexcept
    {
        return bool(x);
    }

    //! \returns `false`
    template <typename ...Ts>
    bool operator>(nullvariant_t, variant<Ts...>& /*x*/) noexcept
    {
        return false;
    }

    //! \returns `!x`
    template <typename ...Ts>
    bool operator<=(variant<Ts...>& x, nullvariant_t) noexcept
    {
        return !x;
    }

    //! \returns `true`
    template <typename ...Ts>
    bool operator<=(nullvariant_t, variant<Ts...>& /*x*/) noexcept
    {
        return true;
    }

    //! \returns `true`
    template <typename ...Ts>
    bool operator>=(variant<Ts...>& /*x*/, nullvariant_t) noexcept
    {
        return true;
    }

    //! \returns `!x`
    template <typename ...Ts>
    bool operator>=(nullvariant_t, variant<Ts...>& x) noexcept
    {
        return !x;
    }

    ///////////////////////////////////////////////////////////////////////////
    //! \requires `INVOKE(std::forward<F>(f), std::declval<T>(), R)` shall be
    //!  a valid expression for all `T` in `Ts...`.
    //!
    //! \effects Equivalent to `INVOKE(std::forward<F>(f), *v.target<T>(), R)`
    //!  if `v` has an active member of type `T`.
    //!
    //! \throws `bad_variant_access` if `v` has no active member.
    template <typename R, typename F, typename ...Ts>
    R apply(F&& f, variant<Ts...>& v)
    {
        return bool(v)
          ? detail::apply<R, F>{}(
                detail::pack<Ts&...>{}, v.which()
              , v.target(), std::forward<F>(f)
            )
          : throw bad_variant_access{};
    }

    //! \effects Equivalent to `apply<R>(std::forward<F>(f), v)` where `R` is
    //!  the weak result type of `F`.
    //!
    //! \remarks This function shall not participate in overload resolution
    //!  unless `F` has a weak result type.
    template <
        typename F, typename ...Ts
      , typename R = detail::weak_result<std::decay_t<F>>
    >
    R apply(F&& f, variant<Ts...>& v)
    {
        return apply<R>(std::forward<F>(f), v);
    }

    //! \requires `INVOKE(std::forward<F>(f), std::declval<T const>(), R)`
    //!  shall be a valid expression for all `T` in `Ts...`.
    //!
    //! \effects Equivalent to `INVOKE(std::forward<F>(f), *v.target<T>(), R)`
    //!  if `v` has an active member of type `T`.
    //!
    //! \throws `bad_variant_access` if `v` has no active member.
    template <typename R, typename F, typename ...Ts>
    R apply(F&& f, variant<Ts...> const& v)
    {
        return bool(v)
          ? detail::apply<R, F>{}(
                detail::pack<Ts const&...>{}, v.which()
              , v.target(), std::forward<F>(f)
            )
          : throw bad_variant_access{};
    }

    //! \effects Equivalent to `apply<R>(std::forward<F>(f), v)` where `R` is
    //!  the weak result type of `F`.
    //!
    //! \remarks This function shall not participate in overload resolution
    //!  unless `F` has a weak result type.
    template <
        typename F, typename ...Ts
      , typename R = detail::weak_result<std::decay_t<F>>
    >
    R apply(F&& f, variant<Ts...> const& v)
    {
        return apply<R>(std::forward<F>(f), v);
    }

    //! \requires `INVOKE(std::forward<F>(f), std::declval<T&&>(), R)` shall
    //!  be a valid expression for all `T` in `Ts...`.
    //!
    //! \effects Equivalent to `INVOKE(std::forward<F>(f), std::move(
    //!  *v.target<T>()), R)` if `v` has an active member of type `T`.
    //!
    //! \throws `bad_variant_access` if `v` has no active member.
    template <typename R, typename F, typename ...Ts>
    R apply(F&& f, variant<Ts...>&& v)
    {
        return bool(v)
          ? detail::apply<R, F>{}(
                detail::pack<Ts&&...>{}, v.which()
              , v.target(), std::forward<F>(f)
            )
          : throw bad_variant_access{};
    }

    //! \effects Equivalent to `apply<R>(std::forward<F>(f), std::move(v))`
    //!  where `R` is the weak result type of `F`.
    //!
    //! \remarks This function shall not participate in overload resolution
    //!  unless `F` has a weak result type.
    template <
        typename F, typename ...Ts
      , typename R = detail::weak_result<std::decay_t<F>>
    >
    R apply(F&& f, variant<Ts...>&& v)
    {
        return apply<R>(std::forward<F>(f), std::move(v));
    }

    ///////////////////////////////////////////////////////////////////////////
    //! \effects Calls `x.swap(y)`.
    template <typename ...Ts>
    void swap(variant<Ts...>& x, variant<Ts...>& y)
        noexcept(noexcept(x.swap(y)))
    {
        x.swap(y);
    }
}}

namespace std
{
    //! \requires The template specialization `std::hash<T>` shall meet the
    //!  requirements of class template `std::hash` for all `T` in `Ts...`.
    //!  The template specialization `std::hash<variant<Ts...>>` shall meet
    //!  the requirements of class template `std::hash`. For an object `v` of
    //!  type `variant<Ts...>`, if `v` has an active member of type `T`,
    //!  `std::hash<variant<Ts...>>()(v)` shall evaluate to the same value as
    //!  `std::hash<T>()(*v.target<T>())`; otherwise it evaluates to an
    //!  unspecified value.
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
