#include "sqal_internal.h"
#include <math.h>
#include <time.h>

extern "C" {

//#define SQUSEDOUBLE

#ifndef round
//int round(SQFloat f) { return (int)f + (((f - (int)f) >= 0.5) ? 1 : 0); }
int round(SQFloat f) { return (int)f + ((fmod(f, (SQFloat)1.0) >= 0.5f) ? 1 : 0); }
#endif

#ifndef sign
int sign(int v) { if (v < 0) return -1; if (v > 0) return +1; return 0; }
#endif
void swap(int &l, int &r) { int t; t = r; r = l; l = t; }

#define DSQA_FUNC_FLOAT_P1(NAME)     DSQA_FUNC(NAME) { SQFloat f = 0.0         ; sq_getfloat(v, -1, &f); sq_pushfloat  (v,      NAME(f)); return 1; }
#define DSQA_FUNC_FLOAT_P1_INT(NAME) DSQA_FUNC(NAME) { SQFloat f = 0.0         ; sq_getfloat(v, -1, &f); sq_pushinteger(v, (int)NAME(f)); return 1; }
#define DSQA_FUNC_FLOAT_P2(NAME)     DSQA_FUNC(NAME) { SQFloat l = 0.0, r = 0.0; sq_getfloat(v, -1, &r); sq_getfloat   (v, -2, &l); sq_pushfloat(v, NAME(l, r)); return 1; }

DSQA_FUNC(min) {
	SQInteger res; SQObjectPtr &r = stack_get(v, -1), &l = stack_get(v, -2);
	v->ObjCmp(l, r, res);
	if (res < 0) v->Pop();
	return 1;
}

DSQA_FUNC(max) {
	SQInteger res; SQObjectPtr &r = stack_get(v, -1), &l = stack_get(v, -2);
	v->ObjCmp(l, r, res);
	if (res > 0) v->Pop();
	return 1;
}

DSQA_FUNC(abs) {
	SQInteger res;
	SQObjectPtr &a = stack_get(v, -1);
	SQObjectPtr b;
	v->ObjCmp(a, 0, res);
	if (res < 0) {
		v->NEG_OP(b, a);
		v->Push(b);
	}
	return 1;
}

DSQA_FUNC(sign) {
	SQInteger res;
	SQObjectPtr &a = stack_get(v, -1);
	v->ObjCmp(a, (SQFloat)0.0, res);
	v->Push(sign(res));
	return 1;
}

DSQA_FUNC(clamp) {
	SQInteger res_min, res_max;

	SQObjectPtr &max = stack_get(v, -1); // max
	SQObjectPtr &min = stack_get(v, -2); // min
	SQObjectPtr &val = stack_get(v, -3); // value

	// Comparisons.
	v->ObjCmp(val, min, res_min);
	v->ObjCmp(val, max, res_max);
	
	int pop_count;
	
	if (res_max > 0) { // max
		pop_count = 0;
	} else if (res_min < 0) { // min
		pop_count = 1;
	} else { // current
		pop_count = 2;
	}

	while (pop_count-- > 0) v->Pop();

	return 1;
}

DSQA_FUNC(interpolate) {
	SQInteger res_min, res_max;

	SQFloat progress = 0.0;
	
	sq_getfloat(v, -1, &progress);     // progress
	SQObjectPtr &b = stack_get(v, -2); // b
	SQObjectPtr &a = stack_get(v, -3); // a

	SQObjectPtr res;
	
	// a + (b - a) * v;
	v->ARITH_OP('-', res, b, a);
	v->ARITH_OP('*', res, res, progress);
	v->ARITH_OP('+', res, res, a);
	
	v->Push(res);
	return 1;
}

DSQA_FUNC_FLOAT_P1(sin);
DSQA_FUNC_FLOAT_P1(cos);
DSQA_FUNC_FLOAT_P1(tan);
DSQA_FUNC_FLOAT_P1(asin);
DSQA_FUNC_FLOAT_P1(acos);
DSQA_FUNC_FLOAT_P1(atan);
DSQA_FUNC_FLOAT_P2(atan2);
DSQA_FUNC_FLOAT_P1(sinh);
DSQA_FUNC_FLOAT_P1(cosh);
DSQA_FUNC_FLOAT_P1(tanh);

DSQA_FUNC_FLOAT_P1_INT(floor);
DSQA_FUNC_FLOAT_P1_INT(ceil);
DSQA_FUNC_FLOAT_P1_INT(round);

DSQA_FUNC_FLOAT_P1(sqrt);
DSQA_FUNC_FLOAT_P1(exp);
DSQA_FUNC_FLOAT_P2(pow);
DSQA_FUNC_FLOAT_P1(log);
DSQA_FUNC_FLOAT_P1(log10);

class MT_Rand { public:
	#define hiBit(u)      ((u) & 0x80000000U)  /* mask all but highest   bit of u */
	#define loBit(u)      ((u) & 0x00000001U)  /* mask all but lowest    bit of u */
	#define loBits(u)     ((u) & 0x7FFFFFFFU)  /* mask     the highest   bit of u */
	#define mixBits(u, v) (hiBit(u)|loBits(v)) /* move hi bit of u to hi bit of v */
	#define twist(m,u,v)  (m ^ (mixBits(u,v)>>1) ^ ((u32)(-(u32)(loBit(u))) & 0x9908b0dfU))

	static const int N = 624; // Length of state vector.
	static const int M = 397; // A period parameter.

	bool seeded;
	u32 state[N + 1]; // state vector + 1 extra to not violate ANSI C
	u32 *next;
	int left;

	MT_Rand() {
		seeded = false;
	}
	
	void initialize(u32 seed) {
		register u32 *s = state, *r = state;
		register int i = 1;

		*s++ = seed & 0xffffffffU;
		for( ; i < N; ++i ) {
			*s++ = ( 1812433253U * ( *r ^ (*r >> 30) ) + i ) & 0xffffffffU;
			r++;
		}
	}
	
	void reload() {
		register u32 *p = state;
		register int i;

		for (i = N - M; i--; ++p) *p = twist(p[M], p[0], p[1]);
		for (i = M    ; --i; ++p) *p = twist(p[M-N], p[0], p[1]);

		*p = twist(p[M-N], p[0], state[0]);
		left = N;
		next = state;
	}
	
	void srand(u32 seed) {
		initialize(seed);
		reload();
		seeded = true;
	}
	
	void srand() {
		srand(time(NULL));
	}
	
	u32 rand() {
		register u32 s1;
		
		if (!seeded) srand();

		if (left == 0) reload();
		--left;
			
		s1 = *next++;
		s1 ^= (s1 >> 11);
		s1 ^= (s1 <<  7) & 0x9d2c5680U;
		s1 ^= (s1 << 15) & 0xefc60000U;
		return ( s1 ^ (s1 >> 18) );
	}
	
	u32 rand(int min, int max) {
		return min + rand() % (max - min + 1);
	}
};

static MT_Rand mtrand;

DSQA_FUNC(rand) {
	SQInteger min = 0, max = 0x7FFFFFFF;
	sq_getinteger(v, -1, &max);
	sq_getinteger(v, -2, &min);
	if (min > max) swap(min, max);
	sq_pushinteger(v, mtrand.rand(min, max));
	//sq_pushinteger(v, rand());
	return 1;
}

DSQA_FUNC(srand) {
	SQInteger seed = 0;
	sq_getinteger(v, -1, &seed);
	mtrand.srand(seed);
	return 1;
}

SQUIRREL_API void sqal_math_register(HSQUIRRELVM v) {
	SQInteger top = sq_gettop(v);
	{
		// Misc.
		sqal_register_simple(abs,   "..");
		sqal_register_simple(clamp, "....");
		sqal_register_simple(sign,  "..");
		sqal_register_simple(min,   "...");
		sqal_register_simple(max,   "...");
		sqal_register_simple(interpolate, "....");

		// Rounding.
		sqal_register_simple(floor, ".n");
		sqal_register_simple(ceil,  ".n");
		sqal_register_simple(round, ".n");

		// Arithmetic.
		sqal_register_simple(sqrt,  ".n");
		sqal_register_simple(pow,   ".nn");
		sqal_register_simple(exp,   ".n");
		sqal_register_simple(log,   ".n");
		sqal_register_simple(log10, ".n");

		// Trigonometric functions.
		sqal_register_simple(sin,   ".n");
		sqal_register_simple(cos,   ".n");
		sqal_register_simple(tan,   ".n");
		sqal_register_simple(asin,  ".n");
		sqal_register_simple(acos,  ".n");
		sqal_register_simple(atan,  ".n");
		sqal_register_simple(atan2, ".nn");
		sqal_register_simple(sinh,  ".n");
		sqal_register_simple(cosh,  ".n");
		sqal_register_simple(tanh,  ".n");

		// Random number generator (MT).
		sqal_register_simple(srand, ".n");
		sqal_register_simple(rand,  ".nn");

		// Constants.
		sqal_register_constant("PI", (SQFloat)3.14159265358979323846);

		//if (0)
		{ // ::Vec
			static const SQChar *class_vec_src = _SC(" \
				class ::Vec { \n\
					x = 0; y = 0; \n\
					constructor(x, y = null) { \n\
						if (y == null) { \n\
							if (!(\"x\" in x)) throw(\"::Vec invalid parameters\"); \n\
							this.y = x.y; this.x = x.x; \n\
						} else { \n\
							this.x = x; this.y = y; \n\
						} \n\
					} \n\
					function _cmp (that)  { return ((this.x == that.x) && (this.y == that.y)) ? 0 : 1; } \n\
					function _add (that)  { return ::Vec(this.x + that.x, this.y + that.y); } \n\
					function _sub (that)  { return ::Vec(this.x - that.x, this.y - that.y); } \n\
					function _mul (v)     { return ::Vec(this.x * v     , this.y * v     ); } \n\
					function _div (v)     { return ::Vec(this.x / v     , this.y / v     ); } \n\
					function _unm ()      { return ::Vec(-this.x, -this.y); } \n\
					function len  ()      { return ::sqrt(this.x * this.x + this.y * this.y); } \n\
					function floor()      { return ::Vec(::floor(this.x), ::floor(this.y)); } \n\
					function abs  ()      { return ::Vec(::abs(this.x), ::abs(this.y)); } \n\
					function normalize()  { local l = this.len(); return ::Vec(this.x / l, this.y / l); } \n\
					function _tostring()  { return ::format(\"vec(%d, %d)\", this.x, this.y) } \n\
					function _cloned()    { } \n\
				} \n\
				 \n\
				class ::Rect { \n\
					p0 = ::Vec(0, 0); \n\
					p1 = ::Vec(0, 0); \n\
					size = ::Vec(0, 0); \n\
					constructor(p0, p1, x2 = null, y2 = null) { \n\
						if (x2 != null) { \n\
							this.p0 = ::Vec(p0, p1); \n\
							this.p1 = ::Vec(x2, y2); \n\
						} else { \n\
							this.p0 = p0; \n\
							this.p1 = p1; \n\
						} \n\
						this.size = (this.p1 - this.p0).abs(); \n\
					} \n\
					function _tostring() { return ::format(\"Rect((%s), (%s))\", this.p0._tostring(), this.p1._tostring()); } \n\
					function inside(p) { \n\
						return ( \n\
							(p.x >= this.p0.x) && (p.y >= this.p0.y) && \n\
							(p.x <  this.p1.x) && (p.y <  this.p1.y) \n\
						); \n\
					} \n\
				} \n\
			");
			SQInteger oldtop = sq_gettop(v);
			if (SQ_SUCCEEDED(sq_compilebuffer(v, class_vec_src, strlen(class_vec_src), _SC("math vec"), SQTrue))) {
				sq_pushroottable(v);
				sq_call(v, 1, 0, SQTrue);
			}
			sq_settop(v, oldtop);
		}
	}
	sq_settop(v, top);
	
	mtrand = MT_Rand();
}

}