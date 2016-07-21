//==================================================================
//	File: 		labeledVertex.h
//	Author: 	Timothy A. Budd
//	Platform:	Cfront
//	Description: 	Template Description of labeled graph vertex
//	Copyright (c) 1992 by Timothy A. Budd, All Rights Reserved
//	Permission granted for duplication if not for profit
//==================================================================

# ifndef labeldVertexH
# define labeldVertexH

//
//	weighted graph vertices
//

# include <table.h>

//
//	class labeledVertex
//		vertex maintaining arcs with edge weights
//

template <class T, class A> class labeledVertex {
public:
	T value;

	// constructors
	labeledVertex();
	labeledVertex(T init);
	labeledVertex(const labeledVertex & x);

	// collect the set of neighbor arcs
	dictionary<labeledVertex *, A>  arcs;

	// add an arc
	void addArc(labeledVertex * to, A label);
};


template <class T, class A> int operator == 
	(const labeledVertex<T, A> & left, const labeledVertex<T, A> & right);

template <class T, class A> 
labeledVertex<T, A>::labeledVertex() 
{
}

template <class T, class A> 
labeledVertex<T, A>::labeledVertex(T init) : value(init)
{
}

template <class T, class A> 
labeledVertex<T, A>::labeledVertex(const labeledVertex<T, A> & x) 
	: value(x.value)
{
}

template <class T, class A> 
void labeledVertex<T, A>::addArc(labeledVertex<T, A> * to, A label)
{
	arcs[to] = label;
}

template <class T, class A> int operator == 
	(const labeledVertex<T, A> & left, const labeledVertex<T, A> & right)
{
	return left.value == right.value;
}

# endif
