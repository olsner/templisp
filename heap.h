template <typename... T>
struct heap
{};
template <typename X, typename H> struct cons_heap;
template <typename X, typename... Xs>
struct cons_heap<X, heap<Xs...> >
{
	typedef heap<X, Xs...> value;
};

template <typename H,typename P> struct peek;
template <typename H,typename P,typename Val> struct poke;
template <typename H, typename Val> struct alloc;

template <int p> struct ptr;
template <int p>
struct print_val<ptr<p> >: print_val<value_type<char,'#'> >,print_int<p> {};

template <typename X, typename... Xs>
struct peek<heap<X, Xs...>, ptr<0> >
{
	typedef X value;
};
template <typename X, typename... Xs, int p>
struct peek<heap<X, Xs...>, ptr<p> >
{
	typedef typename peek<heap<Xs...>, ptr<p-1> >::value value;
};

template <typename X, typename... Xs, typename Val>
struct poke<heap<X, Xs...>, ptr<0>, Val>
{
	typedef heap<Val, Xs...> value;
};

template <typename X, typename... Xs, int p, typename Val>
struct poke<heap<X, Xs...>, ptr<p>, Val>
{
	typedef typename poke<heap<Xs...>, ptr<p-1>, Val>::value rest_heap;
	typedef typename cons_heap<X, rest_heap>::value value;
};

template <typename... Xs, typename V>
struct alloc<heap<Xs...>, V>
{
	typedef heap<Xs..., V> heap;
	typedef ptr<sizeof...(Xs)> value;
};
