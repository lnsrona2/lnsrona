#pragma once
#include <string>
#include <iostream>
#include "../src/location.hh"
#include "pcode.h"

namespace C1
{
	namespace AST
	{
		class DeclContext;
		class Declaration;
		class Node;
		class Expr;
		class Initializer;
		class QualifiedTypeSpecifier;
		class Declarator;
		class Stmt;
		class CompoundStmt;

		class Node
		{
		public:
			typedef C1::location LocationType;

			Node();
			Node(const LocationType& location);

			const LocationType& Location() const { return m_location; }
			LocationType& Location() { return m_location; }
			void SetLocation(const LocationType& val) { m_location = val; }

			const Node* Parent() const { return m_parent; }
			Node* Parent() { return m_parent; }
			void SetParent(Node* val) { m_parent = val; }

			const Node* NextNode() const;
			const Node* PrevNode() const;

			//std::string ToString() const/* = 0*/;

			virtual void Dump(std::ostream& ostr) const = 0;
			virtual void Generate(C1::PCode::CodeDome& dome); // Do nothing

			virtual ~Node();
		private:
			LocationType m_location;
			Node* m_parent;
		};

		inline std::ostream& operator<<(std::ostream& os, const Node& node)
		{
			node.Dump(os);
			return os;
		}

		inline C1::PCode::CodeDome& operator<<(C1::PCode::CodeDome& dome, Node& node)
		{
			node.Generate(dome);
			return dome;
		}

		void error(const C1::AST::Node *node, const std::string &msg);
		void error(const C1::location &loc, const std::string &msg);
	}
}