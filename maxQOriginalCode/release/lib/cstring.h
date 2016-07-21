//======================================================================
//  File:        string.h
//  Author:      Timothy A. Budd
//  Description: This file contains the interface of the string
//               class.
//
//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose
//======================================================================

#ifndef STRING_H
#define STRING_H

#include <iostream.h>


class substring;			// forward declaration



//----------------------------------------------------------------------
//	class string
//		better bounds checking, assignments which result
//		in copies, comparisons using relational operators,
//		high level operations such as substrings
//----------------------------------------------------------------------

class string
{
public:
    // constructors
    string();
    string(char);
    string(int);
    string(const char *);
    string(const string &);

    // destructor
    ~string();

    // assignment an catenation
    void         operator =  (const string & right);
    void         operator += (const string & right);

    // substring access
    substring    operator () (unsigned int start, unsigned int length);

    // input of an entire line of text
    istream &    getline(istream &);

    // number of characters in the string
    unsigned int length() const;

    // access to a single character
    char &       operator [](unsigned int) const;

    // compare - used in relational operators
    int          compare(const string &) const;

    // conversion to ordinary C string
    operator     const char *() const;

    // allow access by substring class
    friend class substring;
private:
    // data fields - character buffer and length
    unsigned short int bufferlength;
    char *             buffer;
};



//----------------------------------------------------------------------
//	class substring
//		facilitator class to permit access to
//		portions of a string
//----------------------------------------------------------------------

class substring
{
public:
    // constructors
    substring(string & base, int start, int length);
    substring(const substring & source);

    // assignment of string to substring
    void operator = (const string &) const;

    // conversion of substring to string
    operator string () const;

private:
    // data areas
    string &        base;
    const short int index;
    const short int count;
};



// Function Prototypes: relational operators

int    operator <  (const string &, const string &);
int    operator <= (const string &, const string &);
int    operator != (const string &, const string &);
int    operator == (const string &, const string &);
int    operator >= (const string &, const string &);
int    operator >  (const string &, const string &);



// Function Prototype: concatenate operator

string operator +  (const string &, const string &);



// Function Prototypes: character classification

int isUpperCase(char c);
int isLowerCase(char c);



// Function Prototypes: string conversions

void toUpper(string & word);
void toLower(string & word);


// Function Prototype: stream input

istream & operator >> (istream & in, string & str);



#endif
