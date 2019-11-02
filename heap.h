#pragma once

#include "print.h"

namespace {

template<typename... T> struct heap {};

template <typename X, typename H> struct cons_heap;
template <typename X, typename... Xs>
struct cons_heap<X, heap<Xs...>>
{
	typedef heap<X, Xs...> value;
};
template <typename X, typename H> using cons_heap_t = typename cons_heap<X, H>::value;

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

template <typename ENV, typename V> struct deepeek { typedef V value; };
template <typename ENV, typename V> using deepeek_t = typename deepeek<ENV, V>::value;

template <typename ENV, int p>
struct deepeek<ENV,ptr<p>>
{
	typedef peek_t<ENV, ptr<p>> val1;
	typedef deepeek_t<ENV, val1> value;
};

template <typename ENV, typename CAR, typename CDR>
struct deepeek<ENV,cons<CAR,CDR>>
{
	typedef deepeek_t<ENV, CAR> car;
	typedef deepeek_t<ENV, CDR> cdr;
	typedef cons<car,cdr> value;
};

template <typename ENV, typename V> using print_env = print<deepeek_t<ENV, V>>;

}
