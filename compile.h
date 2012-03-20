
template <ob proc(ob env, ob args)>
struct prim
{
	static ob reified;
};
template <ob proc(ob env, ob args)>
ob prim<proc>::reified = obnew(otproc, 2, proc, NULL);

ob prim_CAR(ob, ob args)
{
	return args->obs[1]->car;
}

ob prim_CDR(ob, ob args)
{
	return args->obs[1]->cdr;
}

ob prim_null(ob, ob args)
{
	return args->obs[1] ? NULL : T::reified;
}

#define forvec(entry, vec) \
	for (ob forvec__vec = (vec), \
			*forvec__start = forvec__vec->obs + 1, \
			*forvec__end = forvec__start + forvec__vec->val, \
			entry = *forvec__start; \
		forvec__start < forvec__end; \
		entry = *++forvec__start)

ob prim_PLUS(ob, ob args)
{
	int res = 0;
	forvec(arg, args)
	{
		assert(arg->tag == otint);
		res += arg->val;
	}
	return obnew(otint, 1, res);
}

template <typename T>
ob eval(const T&)
{
#define reg_prim(p) \
	cons<p, prim<prim_##p> >

	ob env = list<list<
reg_prim(PLUS),
reg_prim(CAR),
reg_prim(CDR),
reg_prim(null)
		>::value>::value::reified;

	return analyze<T>().ret(env);
}

