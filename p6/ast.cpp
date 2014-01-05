#include "include\ast.h"

using namespace C1::AST;

Node::Node()
{}

typedef float *IntPtr;

unsigned IntPtr();

typedef int* IntPtr;

Node::Node(const Node::LocationType& location)
: m_location(location)
{
	typedef int *IntPtr;
	int foo();
	int* IntPtr;
	IntPtr k;
	switch (foo())
	{
	default:
		break;
	}
}

