#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#ifdef COMPILER
#include <set>
#endif
#include <variant>

enum obtype
{
	otnil = 0,
	otsymbol,
	otstring,
	otcons,
	otproc,
	otint,
	otvec
};

typedef struct ob_ *ob;

const int NUMINLINEP = 2;
struct ob_
{
    constexpr ob_(std::nullptr_t = nullptr): tag(otnil), ptr(nullptr) {}

    constexpr ob_(obtype type, ob ob0): tag(type), ob0(ob0) {
    }

    constexpr ob_(obtype type, ob ob0, ob ob1): tag(type), obs{ob0, ob1} {
    }

    constexpr ob_(obtype type, const char* s): tag(type), str(s) {
        assert(type == otsymbol || type == otstring);
    }

    constexpr ob_(int val): tag(otint), val(val) {
    }

    constexpr ob_(ob (*proc)(ob env, ob args), ob env): tag(otproc), proc(proc), env(env) {
    }

    // Lazy: some things have irregular alignment so there aren't tag bits.
    // Just doing the simplest thing here :)
    obtype tag;
    union
    {
        void *ptr;
        void *ptrs[NUMINLINEP];
        ob ob0;
        ob obs[NUMINLINEP];
        const char* sym;
        const char* str;
        int val;
        struct
        {
            ob car;
            ob cdr;
        };
        struct
        {
            ob (*proc)(ob env, ob args);
            ob env;
        };
    };
};

static bool eqsym(ob o, const char* sym)
{
	return o->tag == otsymbol && o->sym == sym;
}

struct less_sym
{
	bool operator()(ob a, ob b)
	{
		assert(a->tag == otsymbol && b->tag == otsymbol);
		return a != b && strcmp(a->sym, b->sym) < 0;
	}
};

#ifdef COMPILER
static std::set<ob, less_sym> symset;

static ob regsym(ob sym)
{
    const auto [it, inserted] = symset.insert(sym);
    if (!inserted) {
        // Perhaps free the symbol since we're returning another one? Or wait for a GC.
    }
    return *it;
}

static ob getsym(ob sym)
{
	auto it = symset.find(sym);
	if (it == symset.end())
	{
		return regsym(sym);
	}
	return *it;
}
#else
static ob regsym(ob sym)
{
	return sym;
}
#endif

static ob oballoc(obtype type, size_t size)
{
	// TODO Implement a real RTS :)
	ob r = (ob)malloc(sizeof(*r) + (size - NUMINLINEP) * sizeof(ob));
	r->tag = type;
	return r;
}

static inline void init(void**p)
{}
template <typename T, typename... U>
static inline void init(void**p, T value, U... values)
{
	*p++ = (void*)(uintptr_t)value;
	init(p, values...);
}
template <typename... T>
static inline ob obnew(obtype type, size_t size, T... values)
{
	ob r = oballoc(type, size);
	void** p = r->ptrs;
	init(p, values...);
	return r;
}

static void rtsAbort(const char* format, ...) __attribute__((noreturn));
static void rtsAbort(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	abort();
}

void printob(ob val);

void printobcons(ob val)
{
	// Got a cdr that wasn't a cons, do a dotted pair
	if (val->tag != otcons)
	{
		printf(". ");
		printob(val);
		return;
	}
	printob(val->car);
	if (val->cdr)
	{
		printf(" ");
		printobcons(val->cdr);
	}
}

#define forvec(entry, vec) \
	for (ob forvec__vec = (vec), \
			*forvec__start = forvec__vec->obs + 1, \
			*forvec__end = forvec__start + forvec__vec->val, \
			entry; \
		forvec__start < forvec__end && (entry = *forvec__start, true); \
		forvec__start++)

void printob(ob val)
{
	if (!val)
	{
		printf("nil");
		return;
	}

	switch (val->tag)
	{
	case otcons:
		printf("(");
		printobcons(val);
		printf(")");
		break;
	case otint:
		printf("%d", val->val);
		break;
	case otsymbol:
		printf("%s", val->sym);
		break;
	case otstring:
		printf("%s", val->str);
		break;
	case otproc:
		printf("<procedure>");
		break;
	case otvec:
		printf("{ ");
		forvec(entry, val)
		{
			printob(entry);
			printf(" ");
		}
		printf("}");
		break;
	default:
		rtsAbort("printob: unhandled type %d in %p", val->tag, val);
	}
}

ob& rtsGetBinding(ob env, ob symob)
{
	const char* sym = symob->sym;
	while (env)
	{
		assert(env->tag == otcons);
		ob frame = env->car;
		while (frame)
		{
			ob bind = frame->car;
			assert(bind->tag == otcons);
			if (eqsym(bind->car, sym))
			{
				return bind->cdr;
			}
			frame = frame->cdr;
		}
		env = env->cdr;
	}
	rtsAbort("No binding found for \"%s\"!", sym);
}

void rtsSetBinding(ob env, ob sym, ob value)
{
	rtsGetBinding(env, sym) = value;
}

void rtsAddBinding(ob env, ob binding)
{
	env->car = obnew(otcons, 2, binding, env->car);
}
