#include <string.h>

/*
templ_lists2.h: These lists have types as values all the way. This is to
allow templ_lisp to represent different kinds of values as different types
(also enabling using templates to match c++ type and c++ value of a lisp val)
*/

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

/*
(defun my-reverse (lst &optional (out nil)) 
  (cond ((null lst) out)
        (t (my-reverse-aux (rest lst) (cons (first lst) out)))))*/
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
