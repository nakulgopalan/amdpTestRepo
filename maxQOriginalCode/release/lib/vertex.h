//==================================================================
//	File: 		vertex.h
//	Author: 	Timothy A. Budd
//	Platform:	Cfront
//	Description: 	Template Description of graph vertex
//	Copyright (c) 1992 by Timothy A. Budd, All Rights Reserved
//	Permission granted for duplication if not for profit
//==================================================================

# ifndef vertexH
# define vertexH

//
//	class vertex
//		a simple unweighted graph vertex
//

# include <setList.h>

template <class T> class vertex {
public:
	//	data value is publically accessible
	T	value;
	setList<vertex<T> &>  arcs;

	// constructor
	vertex();
	vertex(T init);

	// add a new arc
	void addArc(vertex<T> &);
};

template <class T>  
int operator == (const vertex<T> left, const vertex<T> & right);

# endif
