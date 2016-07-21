//======================================================================
//  File:        string.cpp
//  Author:      Timothy A. Budd
//  Description: This file contains the implementation of the
//               string class.
//
//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose
//======================================================================

# include <assert.h>			// assertions
# include <cstring.h>			// string class



//----------------------------------------------------------------------
//	calculate length of a C string
//----------------------------------------------------------------------

int CstringLength(const char * str)
{
    // compute number of non-null characters in a C string
    int i = 0;

    while (str[i] != '\0')
	i++;

    return i;
}



//----------------------------------------------------------------------
//	class string implementation
//----------------------------------------------------------------------

string::string()
{
    // create a string with no characters
    // length is one, holding just null character
    // allocate the new buffer
    bufferlength = 1;

    // initialize buffer with null character
    buffer = new char[bufferlength];
    assert(buffer != 0);

    // initialize to null
    buffer[0] = '\0';
}



string::string(char c)
{
    // create a string with a single character
    bufferlength = 2;
    buffer = new char[bufferlength];
    assert(buffer != 0);

    // initialize with single character
    buffer[0] = c;
    buffer[1] = '\0';
}



string::string(int size)
{
    // create a new string with a buffer of the given size
    // make sure size is positive
    assert(size >= 0);

    // hold one addition char for null value
    bufferlength = 1 + size;

    // allocate buffer, check allocation was successful
    buffer = new char[bufferlength];
    assert(buffer != 0);

    // initialize to all null characters
    for (int i = 0; i < bufferlength; i++)
	buffer[i] = '\0';
}



string::string(const char * initialtext)
{
    // create a new string and initialize with argument value
    // length is length of text, plus null char
    bufferlength = 1 + CstringLength(initialtext);

    // allocate buffer
    buffer = new char[bufferlength];
    assert(buffer != 0);

    // copy values into place
    {
      int i;
      for (i = 0; initialtext[i] != '\0';  i++)
	buffer[i] = initialtext[i];
      buffer[i] = '\0';
    }
}



string::string(const string & initialstring)
{
    // create a new string, initializing with copy of argument
    // length is length of text, plus null char
    bufferlength = 1 + CstringLength(initialstring.buffer);

    // allocate buffer
    buffer = new char[bufferlength];
    assert(buffer != 0);

    // copy values into place
    {
      int i;
      for (i = 0; initialstring.buffer[i] != '\0'; i++)
	buffer[i] = initialstring.buffer[i];

      buffer[i] = '\0';
    }
}



string::~string()
{
    // called implicitly when a string is about to be deleted
    // free the memory associated with the buffer
    delete [] buffer;

    // set the pointer variable to null
    buffer = 0;
}



void string::operator = (const string & right)
{
    // copy values of right argument into string
    const int rightLength = right.length();

    // if it doesn't fit, make a new buffer
    if (rightLength >= bufferlength)
    {
	// delete current buffer
	delete [] buffer;

	// allocate the new one
	bufferlength = 1 + rightLength;
	buffer = new char[bufferlength];
	assert(buffer != 0);
    }

    // copy into buffer
    {
      int i;
      for (i = 0; right.buffer[i] != '\0'; i++)
	buffer[i] = right.buffer[i];
      
      buffer[i] = '\0';
    }
}



void string::operator += (const string & val)
{
    // append argument to end of current string

    // if there isn't space, make new buffer
    int combinedLength = length() + val.length();

    if (combinedLength > bufferlength)
    {
	char * newbuffer = new char [1 + combinedLength];
	assert(newbuffer != 0);

	// copy over old values
	{
	  int i;
	  for (i = 0; buffer[i] != '\0'; i++)
	    newbuffer[i] = buffer[i];

	  newbuffer[i] = '\0';
	}

	// delete old buffer and reset pointer
	delete [] buffer;
	bufferlength = 1 + combinedLength;
	buffer = newbuffer;
    }

    // catenate val on end of current string
    int i = CstringLength(buffer);

    for (int j = 0; val.buffer[j] != '\0'; j++)
	buffer[i++] = val.buffer[j];

    buffer[i] = '\0';
}



substring string::operator ()(unsigned int index, unsigned int count)
{
    // return substring consisting of count chars starting at index

    // if index out of range return empty string
    if (index >= length())
    {
	index = 0;
	count = 0;
    }

    // convert count, if necessary
    int numberleft = length() - index;

    if (count > numberleft)
	count = numberleft;

    // make the new string, use this to be base string
    return substring(*this, index, count);
}



istream & string::getline(istream & in)
{
    // read an entire line of input into string
    in.getline(buffer, bufferlength, '\n');
    return in;
}



unsigned int string::length() const
{
    // compute the number of nonull characters in a string
    return CstringLength(buffer);
}



char nothing;	// global variable holding null character

char & string::operator [](unsigned int index) const
{
    // return the character referenced by the index value

    // first see if the index is in bounds
    if (index >= CstringLength(buffer))
    {
	// not in bounds, return null character
	nothing = '\0';
	return nothing;
    }

    // otherwise it is a valid index, return character
    return buffer[index];
}



int string::compare(const string & val) const
{
    // compare the buffer to the argument string value
    char * p = buffer;
    char * q = val.buffer;

    // loop as long as both have characters and are equal
    while ((*p != '\0') && (*p == *q))
    {
	// inv: up to *p buffer and val are equal
	p++; q++;
    }

    // by subtracting the values referred to by the pointers,
    // we yield a negative value if the first string is smaller
    // than the second, a zero value if they are equal,
    // and a positive value if the first is larger than the second
    return *p - *q;
}



string::operator const char *() const
{
    // convert string value into char pointer value,
    // by returning access to the underlying C buffer
    return buffer;
}



//----------------------------------------------------------------------
//	relational operators
//----------------------------------------------------------------------

int operator <  (const string & left, const string & right)
{ return left.compare(right) < 0; }


int operator <= (const string & left, const string & right)
{ return left.compare(right) <= 0; }


int operator != (const string & left, const string & right)
{ return left.compare(right) != 0; }


int operator == (const string & left, const string & right)
{ return left.compare(right) == 0; }


int operator >= (const string & left, const string & right)
{ return left.compare(right) >= 0; }


int operator >  (const string & left, const string & right)
{ return left.compare(right) > 0; }



//----------------------------------------------------------------------
//	function: concatenate operator
//----------------------------------------------------------------------

string operator + (const string & left, const string & right)
{
    // concatenate two strings, leaving  arguments unchanged
    string result(left);
    result += right;
    return result;
}



//----------------------------------------------------------------------
//	functions: character classification
//----------------------------------------------------------------------

int isUpperCase(char c)
{
    // return true if c is an upper case letter
    return (c >= 'A') && (c <= 'Z');
}



int isLowerCase(char c)
{
    // return true if c is a lower case letter
    return (c >= 'a') && (c <= 'z');
}



//----------------------------------------------------------------------
//	functions: string conversions
//----------------------------------------------------------------------

void toUpper(string & word)
{
    // convert all lower case letters to upper case
    for (int i = 0; word[i] != '\0'; i++)
	if (isLowerCase(word[i]))
            word[i] = (word[i] - 'a') + 'A'; 
}



void toLower(string & word)
{
    // convert all upper case letters to lower case
    for (int i = 0; word[i] != '\0'; i++)
	if (isUpperCase(word[i]))
            word[i] = (word[i] - 'A') + 'a'; 
}



//----------------------------------------------------------------------
//	function: stream input for string
//----------------------------------------------------------------------

istream & operator >> (istream & in, string & str)
{
    // perform string input,
    // read next white-space separated word into string
    // implemented using existing character input facility
    char inbuffer[1000];

    if (in >> inbuffer)
	str = inbuffer;
    else
	str = "";

    return in;
}



//----------------------------------------------------------------------
//	class substring implementation
//----------------------------------------------------------------------

substring::substring(string & b, int i, int c)
    : base(b), index(i), count(c)
{
    // create and initialize a substring value
    // no further initialization required
}



substring::substring(const substring & source)
    : base(source.base), index(source.index), count(source.count)
{
    // create and initialize a substring value
    // no further initialization required
}



void substring::operator = (const string & rep) const
{
    // copy a string value to a substring position
    // if lengths are equal, then no new allocation necessary
    if (count == rep.length())
    {
	for (int i = 0; i <= count; i++)
	    base[index + i] = rep[i];

	return;
    }

    // otherwise compute the length of the new string
    int baselen = base.length();
    int newlen  = rep.length() + baselen - count;
    char * newdata = new char[newlen + 1];

    // then copy the values into the new area
    {
      int i, j;
      for (i = 0; i < index; i++)
	newdata[i] = base[i];

      // catenate on the new values
      for (j = 0; rep[j] != 0; j++)
	newdata[i++] = rep[j];

      // then catenate on the base
      for (j = index + count; base[j] != 0; j++)
	newdata[i++] = base[j];

      newdata[i] = '\0';
    }

    // then change the base string
    delete [] base.buffer;
    base.bufferlength = newlen;
    base.buffer = newdata;

    return;
}



substring::operator string () const
{
    // convert a substring into a string
    // first copy the text into a temporary buffer
    char * buffer = new char[count + 1];
    assert(buffer != 0);

    for (int i = 0; i < count; i++)
	buffer[i] = base[i + index];

    buffer[count] = '\0';

    // then make it into a string, and delete the buffer
    string result(buffer);
    delete [] buffer;
    return result;
}
