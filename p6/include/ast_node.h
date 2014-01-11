#pragma once
#include <string>
#include <iostream>
#include "../src/location.hh"

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
			//virtual void Generate(CodeGenerator& generator);

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

	}
}