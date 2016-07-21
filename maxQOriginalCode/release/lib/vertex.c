//==================================================================
//	File: 		vertex.c
//	Author: 	Timothy A. Budd
//	Platform:	Cfront
//	Description: 	Template implementation of graph vertex
//	Copyright (c) 1992 by Timothy A. Budd, All Rights Reserved
//	Permission granted for duplication if not for profit
//==================================================================

# include <ds/sets/setList.h>

template <class T>  vertex<T>::vertex() 
{ 
	// do nothing
}

template <class T>  vertex<T>::vertex(T init) : value(init) 
{ 
	// do nothing
}

template <class T>  void vertex<T>::addArc(vertex<T> & x)
{
	arcs.add(x);
}

template <class T>  
int operator == (const vertex<T> left, const vertex<T> & right)
{
	return left.value == right.value;
}

