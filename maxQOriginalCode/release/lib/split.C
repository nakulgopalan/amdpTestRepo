//======================================================================
//  File:        split.cpp
//  Author:      Timothy A. Budd
//  Description: This file contains the implementation of the split
//               function, which splits a sentence string into words
//               and returns a vector of these words.
//
//  Copyright (c) 1992 by Timothy A. Budd.  All Rights Reserved.
//	may be reproduced for any non-commercial purpose
//======================================================================

#include <split.h>

// Word Structure

class wordPosition
{
public:
    int    startingPos;
    int    length;
};

unsigned int skipOver(string & text, unsigned int position,
    const charSet & skipchars)
{
    // loop as long as we see characters from charSet
    while ((text[position] != '\0') && skipchars.includes(text[position]))
        position = position + 1;

    // return updated position, which may be null character 
    // if we ran off end of string
    return position;
}



unsigned int skipTo (string & text, unsigned int position,
    const charSet & haltset)
{
    // loop as long as we fail to see characters from halt set
    while ((text[position] != '\0') && !haltset.includes(text[position]))
        position = position + 1;

    // return updated position, which may be null character 
    // if we ran off end of string
    return position;
}


static int findSplitPositions(string & text, const charSet & separators,
    vector<wordPosition> & splits)
{
    // find the start and length of each word in the argument text
    int lastpos = text.length();
    int numberOfWords = 0;

    // now loop over the text, looking for new words
    int position = 0;

    while (position < lastpos)
    {
        // first skip any leading separators
        position = skipOver(text, position, separators);
        
        // either we are at end of text 
        if (position < lastpos)
        { 
            // or at beginning of a word
            if (numberOfWords >= splits.length()) 
                splits.setSize(splits.length() + 5);

            splits[numberOfWords].startingPos = position;

            // now skip over non-separator characters
            position = skipTo(text, position, separators);

            // length is current position - start
            splits[numberOfWords].length = 
                position - splits[numberOfWords].startingPos;
            numberOfWords += 1;
        }
    }

    // return the number of words in words array
    return numberOfWords;
}



void split(string & text, const charSet & separators,
    vector<string> & words)
{
    // invoke procedure to compute word starting positions and lengths.
    // the size of the words array is unimportant, as
    // the vector will grow if necessary
    vector<wordPosition>  splitPos(20);
    int numberOfWords = findSplitPositions(text, separators, splitPos);

    // we now have an array of all the starting positions and lengths
    // simply loop over vector to break string
    words.setSize(numberOfWords, " ");

    for (int i = 0; i < numberOfWords; i++) {
         words[i] = text(splitPos[i].startingPos, splitPos[i].length);
	 }
}
