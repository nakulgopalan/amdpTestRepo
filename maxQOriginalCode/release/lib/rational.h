//==================================================================
//	File: 		rational.h
//	Author: 	Timothy A. Budd
//	Description: 	interface for the rational number 
//			data abstraction
//	Copyright (c) 1992 by Timothy A. Budd, All Rights Reserved
//	Permission Granted for Duplication if not for profit
//==================================================================
# ifndef RATIONAL_H
# define RATIONAL_H

//	forward references

class ostream;
class istream;

//
//	class rational
//		rational number data abstraction
//

class rational {
public:
	// constructors
	rational	();
	rational	(int);
	rational	(int, int);
	rational	(const rational &);

	// accessor functions
	int 	numerator	() const;
	int 	denominator	() const;

	// assignments
	void 	operator =  	(const rational &);
	void 	operator += 	(const rational &);
	void 	operator -= 	(const rational &);
	void 	operator *= 	(const rational &);
	void 	operator /= 	(const rational &);

	// comparison
	int 	compare		(const rational &) const;
	
	// conversions
	operator float	() const;

private:
	// data areas
	int 	top;
	int 	bottom;

	// operations used internally
	void 	normalize	();
};

//
//	function prototypes
//

rational operator + (const rational &, const rational &);
rational operator - (const rational &, const rational &);
rational operator * (const rational &, const rational &);
rational operator / (const rational &, const rational &);
rational operator - (const rational &);

// comparison operations
int operator == (const rational &, const rational &);
int operator != (const rational &, const rational &);
int operator < (const rational &, const rational &);
int operator <= (const rational &, const rational &);
int operator > (const rational &, const rational &);
int operator >= (const rational &, const rational &);

// input and output functions
ostream & operator << (ostream &, const rational &);
istream & operator >> (istream &, rational &);

int floor(const rational &);

//
//	inline functions
//

inline rational::rational() : top(0), bottom(1)
{
	// no further initialization required
}

inline rational::rational(int numerator) : top(numerator), bottom(1)
{
	// no further initialization required
}

inline rational::rational(const rational & value) 
	: top(value.top), bottom(value.bottom)
{
	// no further initialization required
}

inline int rational::numerator() const
{	
	// return numerator field of rational number
	return top; 
}

inline int rational::denominator() const
{	
	// return denominator field of rational number
	return bottom; 
}

#endif
