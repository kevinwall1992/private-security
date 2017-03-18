#ifndef PS_PROMOTE
#define PS_PROMOTE

//http://stackoverflow.com/questions/2426330/uses-of-a-c-arithmetic-promotion-header/2450157#2450157
//Based god Johannas

namespace Utility
{
	// typedef eiher to A or B, depending on what integer is passed
	template<int, typename A, typename B>
	struct cond;

	#define CCASE(N, typed) \
	  template<typename A, typename B> \
	  struct cond<N, A, B> { \
		typedef typed type; \
	  }

	CCASE(1, A); CCASE(2, B);
	CCASE(3, int); CCASE(4, unsigned int);
	CCASE(5, long); CCASE(6, unsigned long);
	CCASE(7, float); CCASE(8, double);
	CCASE(9, long double);

	#undef CCASE

	// for a better syntax...
	template<typename T> struct identity { typedef T type; };

	// different type => figure out common type
	template<typename A, typename B>
	struct Promote {
	private:
	  static A a;
	  static B b;

	  // in case A or B is a promoted arithmetic type, the template
	  // will make it less preferred than the nontemplates below
	  template<typename T>
	  static identity<char[1]>::type &check(A, T);
	  template<typename T>
	  static identity<char[2]>::type &check(B, T);

	  // "promoted arithmetic types"
	  static identity<char[3]>::type &check(int, int);
	  static identity<char[4]>::type &check(unsigned int, int);
	  static identity<char[5]>::type &check(long, int);
	  static identity<char[6]>::type &check(unsigned long, int);
	  static identity<char[7]>::type &check(float, int);
	  static identity<char[8]>::type &check(double, int);
	  static identity<char[9]>::type &check(long double, int);

	public:
	  typedef typename cond<sizeof check(0 ? a : b, 0), A, B>::type
		type;
	};

	// same type => finished
	template<typename A>
	struct Promote<A, A> {
	  typedef A type;
	};
}

#endif
