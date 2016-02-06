# Design

The ultimate design goal is to generalize and enhance the _discriminated `union`_ construct, without compromising its functionality. That is, there should be little or no room for choosing:

    [[syntax: cpp, gutter: false; toolbar: false;]]
    struct U {
      union { T0 m0; ...; TN mN; };
      std::size_t which;
    } u;

over:

    [[syntax: cpp, gutter: false; toolbar: false;]]
    using V = eggs::variant<T0, ..., TN>;
    V v;

In particular:

  - The size of `V` shall match that of the corresponding `U`. Any active member of `v` shall be allocated in a region of `V` suitably aligned for the types `T0, ... TN`; the use of additional storage, such as dynamic memory, is not permitted.

  - Well defined semantics of `u` shall be matched or improved by `v`. Undefined behavior, such as referring to a non-active member of `u`, shall not be allowed by the interface of `v`.

  - All special member functions shall be provided by `V` with the expected semantics.

The interface is largely based on that of [`std::experimental::optional<T>`](http://en.cppreference.com/w/cpp/experimental/optional "std::experimental::optional - cppreference.com"), as defined by the [Library Fundamentals TS](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4082.pdf "C++ Extensions for Library Fundamentals"). The conceptual model for `optional<T>` is that of a _discriminated `union`_ of types `nullopt_t` and `T`. The design decisions taken for `optional<T>` easily translate to `variant<Ts...>`, whose conceptual model is that of a _discriminated `union`_ of types `nullvariant_t` and those in `Ts`. The semantics of all special member functions and relational operators, as well as the interface for switching the active member &mdash;via construction, assignment, or emplacement&mdash;, derives from `optional<T>`.

Access to the active member is based on the design of [`std::function`](http://en.cppreference.com/w/cpp/utility/functional/function "std::function - cppreference.com"), which gives back a pointer to the target if queried with the correct target type &mdash;as a poor man's `dynamic_cast`&mdash;. Additionally, it is also possible to obtain a `void` pointer to the active member (if any), which proved useful to simplify the implementation of helper functions.

Finally, helper classes similar to those of [`std::tuple`](http://en.cppreference.com/w/cpp/utility/tuple/tuple "std::tuple - cppreference.com") are provided, as well as index or type based element access &mdash;albeit with surprising semantics, closer to those of a runtime-checked cast&mdash;.

---

> Copyright _Agustín Bergé_, _Fusion Fenix_ 2014-2016
> 
> Distributed under the Boost Software License, Version 1.0. (See accompanying
> file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
