template <typename... T>
struct heap
{};
// Template aliases require gcc 4.7, can't be bothered installing that :)
//template <typename... T>
//using mkheap = heap<T...>;
template <typename... T>
struct mkheap
{
	typedef heap<T...> value;
};
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
	typedef typename mkheap<Xs..., V>::value heap;
	typedef ptr<sizeof...(Xs)> value;
};

namespace gc
{
template <typename T> struct fwd;
template <typename S, typename D, typename R, typename Val>
struct scavenge2;

template <typename S, typename D, typename R>
struct scavenge
{
private:
	typedef typename peek<S, R>::value old_val;
	typedef scavenge2<S,D,R,old_val> s;
public:
	typedef typename s::src src;
	typedef typename s::dest dest;
	typedef typename s::value value;
};

// Takes a heap of roots, returns (in value) a heap with the new-heap pointers
// for those roots.
template <typename S, typename D, typename Rs>
struct gc;

template <typename S, typename D, typename R>
struct gc<S,D,heap<R> >
{
	typedef scavenge<S,D,R> s;
	typedef typename s::src src;
	typedef typename s::dest dest;
	typedef heap<typename s::value> value;
};

template <typename S, typename D, typename R, typename... Rs>
struct gc<S,D,heap<R,Rs...> >
{
	typedef scavenge<S,D,R> s;
	typedef gc<typename s::src, typename s::dest, heap<Rs...> > s2;
	typedef typename s2::src src;
	typedef typename s2::dest dest;
	typedef typename cons_heap<typename s::value, typename s2::value>::value value;
};
template <typename S, typename R>
struct get_fwd
{
	typedef typename peek<S, R>::value v;
	typedef typename get_fwd<S, v>::value value;
};
template <typename S, typename V>
struct get_fwd<S,fwd<V> >
{
	typedef V value;
};

template <typename S, typename D, typename R>
struct scavenge_object
{
	// Must be specialized for any object that has pointers!
	typedef S src;
	typedef D dest;
	typedef R value;
};

// 1. Peek the old value
// 2. If the old value is a forwarding pointer, just give back the original new heap and the forwarded pointer
// 3. Otherwise, store a forwarding pointer, allocate a new target object, scavenge all references, return the forwarded pointer
template <typename S, typename D, typename R, typename Val>
struct scavenge2
{
private:
	typedef alloc<D, nil> new_copy;
	typedef typename new_copy::value new_ptr;
	typedef typename new_copy::heap d2;
	typedef typename poke<S, R, fwd<new_ptr> >::value s2;
	typedef scavenge_object<s2, d2, Val> scaved;
public:
	typedef typename scaved::src src;
	typedef typename poke<typename scaved::dest, new_ptr, typename scaved::value>::value dest;
	typedef new_ptr value;
};
template <typename S, typename D, typename R, typename Fwd>
struct scavenge2<S,D,R,fwd<Fwd> >
{
	typedef S src;
	typedef D dst;
	typedef Fwd value;
};

template <typename S, typename D, typename CAR, typename CDR>
struct scavenge_object<S,D,cons<CAR, CDR> >
{
	typedef scavenge<S, D, CAR> scavCar;
	typedef scavenge<typename scavCar::src, typename scavCar::dest, CDR> scavCdr;
	typedef typename scavCdr::src src;
	typedef typename scavCdr::dest dest;
	typedef cons<typename scavCar::value, typename scavCdr::value> value;
};

}; // namespace gc

template <typename ENV, typename V>
struct deepeek
{
	typedef V value;
};

template <typename ENV, int p>
struct deepeek<ENV,ptr<p> >
{
	typedef typename peek<ENV, ptr<p> >::value val1;
	typedef typename deepeek<ENV, val1>::value value;
};

template <typename ENV, typename CAR, typename CDR>
struct deepeek<ENV,cons<CAR,CDR> >
{
	typedef typename deepeek<ENV, CAR>::value car;
	typedef typename deepeek<ENV, CDR>::value cdr;
	typedef cons<car,cdr> value;
};

template <typename ENV, typename V>
struct print_env:
	print<typename deepeek<ENV, V>::value>
{
};
