#include "stdafx.h"
#include "ast_node.h"
using namespace C1::AST;


Node::Node()
{}

Node::Node(const Node::LocationType& location)
: m_location(location)
{
}

C1::AST::Node::~Node()
{

}
