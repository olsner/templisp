#pragma once

#include "print.h"

namespace {

// The heap is represented as a tuple of types, pointer 0 points to the first value in the heap.
template<typename... T> struct heap {};

template <typename X, typename H> struct cons_heap;
template <typename X, typename H> using cons_heap_t = typename cons_heap<X, H>::value;
template <typename X, typename... Xs>
struct cons_heap<X, heap<Xs...>>
{
	typedef heap<X, Xs...> value;
};

template <typename H,typename P> struct peek;
template <typename H,typename P> using peek_t = typename peek<H, P>::value;
template <typename H,typename P,typename Val> struct poke;
template <typename H,typename P,typename Val> using poke_t = typename poke<H, P, Val>::value;
template <typename H, typename Val> struct alloc;

template <typename X, typename... Xs>
struct peek<heap<X, Xs...>, ptr<0>>
{
	using value = X;
};
template <typename X, typename... Xs, int p>
struct peek<heap<X, Xs...>, ptr<p>>
{
	using value = peek_t<heap<Xs...>, ptr<p-1>>;
};

template <typename X, typename... Xs, typename Val>
struct poke<heap<X, Xs...>, ptr<0>, Val>
{
	using value = heap<Val, Xs...>;
};

template <typename X, typename... Xs, int p, typename Val>
struct poke<heap<X, Xs...>, ptr<p>, Val>
{
	using rest_heap = poke_t<heap<Xs...>, ptr<p-1>, Val>;
    using value = cons_heap_t<X, rest_heap>;
};

template <typename... Xs, typename V>
struct alloc<heap<Xs...>, V>
{
    // Used to work around naming conflicts between the heap template and our heap return value
    template<typename... T> struct mkheap { typedef heap<T...> value; };

    typedef typename mkheap<Xs..., V>::value heap;
    typedef ptr<sizeof...(Xs)> value;
};

// peek<> that reads a pointer and dereferences all pointers to their values recursively.
// Looks like for now only cons cells are handled? That can be fixed when you
// get a printed value containing "#0" (pointer) instead of values.
// This doesn't handle recursive data structures either, so I think we'll
// eventually want a nicer formatting template that dereferences pointers once,
// keeps track of printed values and prints back references for other things.
// (e.g. ptr<0> on heap<cons<ptr<0>, ptr<0>>>)
template <typename ENV, typename V> struct deep_peek { typedef V value; };
template <typename ENV, typename V> using deep_peek_t = typename deep_peek<ENV, V>::value;

template <typename ENV, int p>
struct deep_peek<ENV,ptr<p>>
{
	typedef peek_t<ENV, ptr<p>> val1;
	typedef deep_peek_t<ENV, val1> value;
};

template <typename ENV, typename CAR, typename CDR>
struct deep_peek<ENV,cons<CAR,CDR>>
{
	typedef deep_peek_t<ENV, CAR> car;
	typedef deep_peek_t<ENV, CDR> cdr;
	typedef cons<car,cdr> value;
};

}
