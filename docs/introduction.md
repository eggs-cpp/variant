# Introduction

A [`union`](http://en.cppreference.com/w/cpp/language/union "Union declaration - cppreference.com") is a special class type that can hold only one of its non-static data members at a time. It's just big enough to accommodate the largest of its members.

> **9 [class]/5**
> A _union_ is a class defined with the class-key union; it holds at most one data member at a time (9.5). _[...]_

> **9.5 [class.union]/1**
> In a union, at most one of the non-static data members can be active at any time, that is, the value of at most one of the non-static data members can be stored in a union at any time. _[...]_ The size of a union is sufficient to contain the largest of its non-static data members. Each non-static data member is allocated as if it were the sole member of a struct. All non-static data members of a union object have the same address.

In **C++98**, members of a `union` where restricted to trivial object types. For these types, lifetime begins when appropriate storage is obtained, and ends when the storage is reused or released.

> **3.8 [basic.life]/1**
> _[...]_ The lifetime of an object of type `T` begins when:
>
>   - storage with the proper alignment and size for type `T` is obtained, and
>   - if the object has non-trivial initialization, its initialization is complete.
>
> The lifetime of an object of type `T` ends when:
>
>   - if `T` is a class type with a non-trivial destructor (12.4), the destructor call starts, or
>   - the storage which the object occupies is reused or released.

This special guarantee allows the active member of a `union` to be changed by simply assigning to it, effectively reusing the storage &mdash;that's at least the spirit, if not the letter, of the standard&mdash;.

Furthermore, a `union` does not know which member &mdash;if any&mdash; is active, so its special member functions have to be implemented without that information. Since members are restricted to trivial types, the special member functions can be implemented in terms of the underlying bytes, independently of the active member.

> **9.5 [class.union]/1**
> _[...]_ An object of a class with a non-trivial constructor (12.1), a non-trivial copy constructor (12.8), a non-trivial destructor (12.4), or a non-trivial copy assignment operator (13.5.3, 12.8) cannot be a member of a union, nor can an array of such objects. _[...]_

In **C++11**, this restriction was lifted; members of a `union` can now be of any object type. Switching between non-trivial members requires explicitly destroying the currently active member, and using placement `new` to construct the newly active member.

> **9.5 [class.union]/4**
> _[Note:_ In general, one must use explicit destructor calls and placement new operators to change the active member of a union. _—end note]_ _[Example:_ Consider an object `u` of a union type `U` having non-static data members `m` of type `M` and `n` of type `N`. If `M` has a non-trivial destructor and `N` has a non-trivial constructor (for instance, if they declare or inherit virtual functions), the active member of `u` can be safely switched from `m` to `n` using the destructor and placement new operator as follows:
>
>     [[syntax: cpp, gutter: false; toolbar: false;]]
>     u.m.~M();
>     new (&u.n) N;
>
> _—end example]_

If a special member function is non-trivial for any of the members, then the `union` special member function will be implicitly defined as _deleted_ when not user-provided.

> **9.5 [class.union]/2**
> _[Note:_ If any non-static data member of a union has a non-trivial default constructor (12.1), copy constructor (12.8), move constructor (12.8), copy assignment operator (12.8), move assignment operator (12.8), or destructor (12.4), the corresponding member function of the union must be user-provided or it will be implicitly deleted (8.4.3) for the union. _—end note]_
>
> **9.5 [class.union]/3**
> _[Example:_ Consider the following union:
>
>     [[syntax: cpp, gutter: false; toolbar: false;]]
>     union U {
>       int i;
>       float f;
>       std::string s;
>     };
>
> Since `std::string` (21.3) declares non-trivial versions of all of the special member functions, `U` will have an implicitly deleted default constructor, copy/move constructor, copy/move assignment operator, and destructor. To use `U`, some or all of these member functions must be user-provided. _-end example]_

These non-trivial member functions can only be provided &mdash;with their usual semantics&mdash; by knowing if a member is active, and then forwarding to it. A _discriminated `union`_ is a `union` or _union-like class_ that is self-aware, that is, it contains some form of identifier that lets it know which member &mdash;if any&mdash; is active. A _discriminated `union`_ can provide all special member functions, trivial or not.

An instance of `eggs::variant<Ts...>` is a _discriminated `union`_ with members of object types `Ts`. It offers a natural interface for switching the active member, and it provides all special member functions with the usual semantics:

    [[syntax: cpp, gutter: false; toolbar: false;]]
    eggs::variants<N, M> u; // u has no active members
    u = M{}; // u has an active member of type M
    u = N{}; // u has an active member of type N, the previous active member was destroyed

    // all special member functions are provided
    using U = eggs::variants<int, float, std::string>;

---

> Copyright _Agustín Bergé_, _Fusion Fenix_ 2014-2016
> 
> Distributed under the Boost Software License, Version 1.0. (See accompanying
> file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
