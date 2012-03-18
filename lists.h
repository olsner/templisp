#include <string.h>

template <typename... T> struct list;
template <> struct list<>
{ typedef nil value; };
template <typename X, typename... Xs> struct list<X, Xs...>
{ typedef cons<X, typename list<Xs...>::value > value; };

template <typename CONS>
struct length
{
	static const uint value=1+length<typename CONS::cdr>::value;
};

template <>
struct length<nil>
{
	static const uint value=0;
};

template <typename CONS, typename OUT=nil>
class reverse
{
	typedef typename CONS::car car;
	typedef typename CONS::cdr cdr;
public:
	typedef typename reverse<cdr, cons<car, OUT> >::value value;
};

template <typename OUT>
struct reverse<nil, OUT>
{
	typedef OUT value;
};

template <typename L1, typename L2>
class append
{
public:
	typedef cons<typename L1::car,
				typename append<typename L1::cdr, L2>::value>
		value;
};

template <typename L2>
struct append<nil, L2>
{
	typedef L2 value;
};
template <typename... Xs, typename... Ys>
struct append<list<Xs...>, list<Ys...>>
{
	typedef typename list<Xs..., Ys...>::value value;
};
template <typename... Xs, typename B>
struct append<list<Xs...>, B>
{
	typedef typename append<typename list<Xs...>::value, B>::value value;
};

