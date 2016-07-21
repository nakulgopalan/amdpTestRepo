//==================================================================
//	File: 		rational.cc
//	Author: 	Timothy A. Budd
//	Description: 	implementation of the rational number 
//			data abstraction
//	Copyright (c) 1992 by Timothy A. Budd, All Rights Reserved
//	Permission Granted for Duplication if not for profit
//==================================================================
# ifndef RATIONAL_C
# define RATIONAL_C

# include <misc.h>
# include <rational.h>		// rational definition
# include <assert.h>		// assertions
# include <stream.h>		// stream input and output

//
//	constructors
//

rational::rational(int numerator, int denominator)
	: top(numerator), bottom(denominator)
{
	// normalize the numerator and denominator values
	normalize();
}

//
//	normalization
//

void rational::normalize()
{	// normalize rational by:
	// a) move sign to numerator
	// b) make sure numerator and denominator have no common divisors
	int sign = 1;	// sign is 1 if positive, -1 if negative
	if (top < 0) {
		sign = -1;
		top = - top;
		}
	if (bottom < 0) {
		sign = - sign;
		bottom = - bottom;
		}

	// make sure we are not dividing by zero
	assert(bottom != 0);

	// find greatest common divisor 
	int d = gcd(top, bottom);

	// move sign to top and divide by gcd
	top = sign * (top / d);
	bottom = bottom / d;
}

//
//	assignments
//

void rational::operator = (const rational & right)
{	// simply copy values from right hand side of assignment
	top = right.top;
	bottom = right.bottom;
}

void rational::operator += (const rational & right)
{	// modify by adding right hand side
	top = top * right.bottom + bottom * right.top;
	bottom *= right.bottom;

	// normalize the result, insuring lowest denominator form
	normalize();
}

void rational::operator -= (const rational & right)
{	// modify by subtracting right hand side
	top = top * right.bottom - bottom * right.top;
	bottom *= right.bottom;

	// normalize the result, insuring lowest denominator form
	normalize();
}

void rational::operator *= (const rational & right)
{	// modify by multiplying by right hand side
	top *= right.top;
	bottom *= right.bottom;

	// normalize the result, insuring lowest denominator form
	normalize();
}

void rational::operator /= (const rational & right)
{	// modify by dividing by right hand side
	// (same as multiplying by reciprocol)
	top *= right.bottom;
	bottom *= right.top;

	// normalize the result, insuring lowest denominator form
	normalize();
}

//
//	comparison
//

int rational::compare (const rational & right) const
{	// return negative if value is less than right
	// zero if value is equal to right
	// and positive if value is greater than right
	return numerator() * right.denominator() - 
		right.numerator() * denominator();
}

//
//	arithmetic operations
//

rational operator + (const rational & left, const rational & right)
{	// return addition of two rational numbers	
	rational result(
		left.numerator() * right.denominator() + 
		right.numerator() * left.denominator(),
			left.denominator() * right.denominator());
	return result;
}

rational operator - (const rational & left, const rational & right)
{	// return difference of two rational numbers
	rational result(
		left.numerator() * right.denominator() - 
		right.numerator() * left.denominator(),
			left.denominator() * right.denominator());
	return result;
}

rational operator * (const rational & left, const rational & right)
{	// return product of two rational numbers
	rational result(
		left.numerator() * right.numerator(),
		left.denominator() * right.denominator());
	return result;
}

rational operator / (const rational & left, const rational & right)
{	// return quotent of two rational numbers
	rational result(
		left.numerator() * right.denominator(),
		left.denominator() * right.numerator());
	return result;
}

rational operator - (const rational & value)
{
	// return negation of a rational number
	rational result( - value.numerator(), value.denominator());
	return result;
}

//
//	converions
//

rational::operator float() const
{	// convert rational number into single-precision floating point
	return (float) top / (float) bottom;
}

//
//	misc functions
//

int floor(const rational & value) const
{	// return the smallest integer less than rationa number
	// integer division returns the floor automatically
	return value.numerator() / value.denominator();
}

ostream & operator << (ostream & out, const rational & value)
{	// stream output of rational number
	out << value.numerator() << " / " << value.denominator();
	return out;
}

istream & operator >> (istream & in, rational & r)
{	// stream input of rational number
	int t, b;

	// read the top
	in >> t;

	// if there is a slash, read the next number
	char c;
	in >> c;
	if (c == '/')
		in >> b;	// read bottom part
	else {
		in.putback(c);
		b = 1;
		}

	// do the assignment
	rational newValue(t, b);
	r = newValue;

	// return the stream
	return in;
}

# endif
