
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

ob prim_CONS(ob, ob args)
{
	return obnew(otcons, 2, args->obs[1], args->obs[2]);
}

ob prim_null(ob, ob args)
{
	return args->obs[1] ? NULL : T::reified;
}

static ob is_type(ob arg, obtype tag)
{
	return arg && arg->tag == tag ? T::reified : NULL;
}

ob prim_NUMBER(ob, ob args)
{
	return is_type(args->obs[1], otint);
}

ob prim_STRING(ob, ob args)
{
	return is_type(args->obs[1], otstring);
}

ob prim_SYMBOL(ob, ob args)
{
	return is_type(args->obs[1], otsymbol);
}

ob prim_PAIR(ob, ob args)
{
	return is_type(args->obs[1], otcons);
}

#define forvec(entry, vec) \
	for (ob forvec__vec = (vec), \
			*forvec__start = forvec__vec->obs + 1, \
			*forvec__end = forvec__start + forvec__vec->val, \
			entry; \
		forvec__start < forvec__end && (entry = *forvec__start, true); \
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

size_t list_length(ob arg, size_t acc = 0)
{
	if (!arg || arg->tag != otcons)
		return acc;
	else
		return list_length(arg->cdr, acc + 1);
}

ob prim_APPLY(ob, ob args)
{
	ob proc = args->obs[1];
	ob arglist = args->obs[2];
	size_t nargs =  list_length(arglist);
	args = obnew(otvec, 1 + nargs, nargs);
	ob* p = &args->obs[1];
	while (arglist)
	{
		assert(arglist->tag == otcons);
		*p++ = arglist->car;
		arglist = arglist->cdr;
	}
	return proc->proc(proc->env, args);
}

ob prim_LIST(ob, ob args)
{
	ob ret = NULL;
	ob* p = &ret;
	forvec(arg, args)
	{
		*p = obnew(otcons, 2, arg, (ob)0);
		p = &(*p)->cdr;
	}
	return ret;
}

ob prim_EQ(ob, ob args)
{
	ob a = args->obs[1];
	ob b = args->obs[2];

	if (a == b) goto eq;
	if (!a || !b) return NULL;
	if (a->tag != b->tag) return 0;
	switch (a->tag)
	{
	case otint:
		if (a->val == b->val) goto eq;
	default:
		;
	}
	return NULL;
eq:
	return T::reified;
}

ob prim_DISPLAY(ob, ob args)
{
	printob(args->obs[1]);
	printf("\n");
	return NULL;
}

ob prim_PUTC(ob, ob args)
{
	putchar(args->obs[1]->val);
	return NULL;
}
ob prim_GETC(ob, ob args)
{
	int c = getchar();
	printf("getc: %d!\n", c);
	if (c == -1) return NULL;
	return obnew(otint, 1, (uintptr_t)c);
}

ob prim_LIST_TO_STRING(ob, ob args)
{
	ob arg = args->obs[1];
	size_t length = (list_length(arg) + sizeof(void*) - 1) / sizeof(void*);
	ob res = obnew(otstring, 1 + length, (uintptr_t)0);
	char* p = (char*)&res->ptrs[1];
	res->str = p;
	while (arg)
	{
		assert(arg->car && arg->car->tag == otint);
		*p++ = arg->car->val;
		arg = arg->cdr;
	}
	return res;
}

ob prim_LIST_TO_SYM(ob, ob args)
{
	ob str = prim_LIST_TO_STRING(NULL, args);
	str->tag = otsymbol;
	return getsym(str);
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
reg_prim(CONS),
reg_prim(null),
reg_prim(NUMBER),
reg_prim(APPLY),
reg_prim(LIST),
reg_prim(EQ),
reg_prim(DISPLAY),
reg_prim(PUTC),
reg_prim(GETC),
reg_prim(STRING),
reg_prim(SYMBOL),
reg_prim(PAIR),
reg_prim(LIST_TO_SYM),
reg_prim(LIST_TO_STRING)
		>::value>::value::reified;

	return analyze<T>().ret(env);
}

