namespace {

template<typename... T> struct list;
template<typename... T> using list_t = typename list<T...>::value;
template<typename CONS> using car_t = typename CONS::car;
template<typename CONS> using cdr_t = typename CONS::cdr;

template<typename LIST, typename Out> struct reverse;
template<typename LIST, typename Out = nil> using reverse_t = typename reverse<LIST, Out>::value;

template<typename L1, typename L2> struct append;
template<typename L1, typename L2> using append_t = typename append<L1, L2>::value;

template<typename CONS> struct length;
template<typename CONS> constexpr uint length_v = length<CONS>::value;

template<> struct list<> { typedef nil value; };
template<typename X, typename... Xs> struct list<X, Xs...> { typedef cons<X, list_t<Xs...>> value; };

template <typename CONS>
struct length
{
	static constexpr uint value = 1 + length_v<cdr_t<CONS>>;
};
template<> struct length<nil> { static constexpr uint value = 0; };

template<typename CONS, typename OUT> struct reverse
{
    using value = reverse_t<cdr_t<CONS>, cons<car_t<CONS>, OUT>>;
};
template<typename Out> struct reverse<nil, Out> { using value = Out; };


template<typename L1, typename L2> struct append {
	using value = cons<car_t<L1>, append_t<cdr_t<L1>, L2>>;
};

template<typename L2> struct append<nil, L2> { using value = L2; };

template <typename... Xs, typename... Ys>
struct append<list<Xs...>, list<Ys...>>
{
	using value = list_t<Xs..., Ys...>;
};

}
