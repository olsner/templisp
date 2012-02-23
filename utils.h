typedef unsigned int uint;

template <typename T, bool b, T true_val, T false_val>
struct select_value;

template <typename T, T true_val, T false_val>
struct select_value<T, true, true_val, false_val>
{
	static const T value=true_val;
};

template <typename T, T true_val, T false_val>
struct select_value<T, false, true_val, false_val>
{
	static const T value=false_val;
};

template <bool b, typename true_type, typename false_type>
struct select_type;

template <typename true_type, typename false_type>
struct select_type<true, true_type, false_type>
{
	typedef true_type type;
};

template <typename true_type, typename false_type>
struct select_type<false, true_type, false_type>
{
	typedef false_type type;
};

template <typename A, typename B>
struct same_type
{
	static const bool value=false;
};

template <typename A>
struct same_type<A, A>
{
	static const bool value=true;
};

struct nil
{
	typedef nil car;
	typedef nil cdr;
};

template <typename CAR, typename CDR>
struct cons
{
	typedef CAR car;
	typedef CDR cdr;
};

/*
	There are two kinds of values: ones that have an actual c++ constant value,
	and those that are represented by a type (i.e. conses). c++ constant values
	are wrapped in this class, other simply use their respective class as the
	"value type"
*/
template <typename T, T val>
struct value_type
{
	typedef T type;
	static const T value=val;
};

#define INT(_i) value_type<int, _i>
#define BOOL(_b) value_type<bool, _b>

