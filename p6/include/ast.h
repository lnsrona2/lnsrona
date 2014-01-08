#pragma once
#ifndef _AST_H
#define _AST_H

//#include "util.h"
//#include "symtab.h"
#include <string>
#include <list>
#include <memory>
#include <array>
#include "../src/location.hh"
#include "ast_context.h"
#include "type.h"
#include "declaration.h"
#include "operators.h"

namespace C1
{

	namespace AST
	{
		class Type;
		class DeclContext;
		class Enumerator;
		class QualifiedTypeSpecifier;

		//template <typename TargetType, typename SourceType>
		//bool isa(typename SourceType* var)
		//{
		//	TargetType ptr = dynamic_cast<TargetType>(var);
		//	return ptr != nullptr;
		//};

		class TranslationUnit;
		// Storage nesseary "global" context value for building AST
		// Represent a literal entity in the code document's AST
		// example 
		// TypeSpecifier is a Node , but not the true under hood Type here.
		class Node
		{
		public:
			typedef C1::location LocationType;

			Node();
			Node(const LocationType& location);

			const LocationType& Location() const;
			LocationType& Location();
			void SetLocation(const LocationType&);

			const Node* Parent() const;
			Node* Parent();
			void SetParent(Node*);

			const Node* NextNode() const;
			const Node* PrevNode() const;

			std::string ToString() const/* = 0*/;

			virtual void Dump(std::ostream& ostr) = 0;
			virtual void Generate(CodeGenerator& generator);

			virtual ~Node() = 0;
		private:
			LocationType m_location;
			Node* m_parent;
		};

		class Comment : public Node
		{
		public:
			const std::string &Content() const;
			std::string &Content();
		};

		// Represent the node which containts a collection of child but not some specifid number of children
		class ScopeNode : public Node
		{
		public:
			std::list<Node*>& Children() { return m_Chilren; }
			const std::list<Node*>& Children() const { return m_Chilren; }

		protected:
			std::list<Node*> m_Chilren;
		};

		class TranslationUnit : public ScopeNode, public DeclContext
		{
		public:
			const std::string& Name() const;
			~TranslationUnit();
		};

		enum ExprValueType
		{
			RValue,
			LValue,
			XValue,
		};

		class Expr : public Node
		{
		public:
			const Type* ReturnType() const;
			ExprValueType ValueType() const;
			bool HasSideEffect() const;

			template <typename value_type>
			value_type Evaluate() const;
		};

		class PosfixExpr : public Expr
		{
		public:
			PosfixExpr(OperatorsEnum op, Expr* Base);
		};

		class CallExpr : public Expr
		{
		public:
			const std::list<std::unique_ptr<Expr>>& ArgumentsList() const
			{
				return m_ArgumentList;
			}

			const Expr* Callee() const //Incompelete
			{
				return m_Callee;
			}

			CallExpr(Node* , std::list<Expr*>*);
		private:
			Expr* m_Callee;
			std::list<std::unique_ptr<Expr>> m_ArgumentList;
		};

		class DeclRefExpr : public Expr
		{
		public:
			DeclRefExpr(DeclContext* lookup_context,const std::string &name);
			const Declaration* Declaration() const;
		};

		class ParenExpr : public Expr
		{
		public:
			ParenExpr(Expr* base);
			Expr* Base();
		};

		class UnaryExpr : public Expr
		{
		public:
			UnaryExpr(OperatorsEnum op, Expr* sub_expr);
			const Expr* SubExpr() const;
			OperatorsEnum Operator() const;
		private:
			std::unique_ptr<Expr> m_SubExpr;
		};

		class BinaryExpr : public Expr
		{
		public:
			BinaryExpr(OperatorsEnum op, Expr* lhs, Expr* rhs);
			const std::array<Expr*, 2>& SubExprs() const;
			const Expr* LeftSubExpr() const;
			const Expr* RightSubExpr() const;
			const OperatorsEnum Operator() const;
		private:
			std::array<Expr*, 2> m_SubExprs;
		};

		class AssignExpr : public BinaryExpr
		{
		public:
			AssignExpr(Expr* lhs, Expr* rhs);
			AssignExpr(OperatorsEnum op, Expr* lhs, Expr* rhs);
			// return OP_ASGN if is OP_ASGN itself
			const OperatorsEnum PrefixedOperator() const;
		};

		class IndexExpr : public BinaryExpr
		{
		public:
			IndexExpr(Expr* host_expr, Expr* index_expr);
		};


		class ArithmeticExpr : public BinaryExpr
		{
		public:
			ArithmeticExpr(OperatorsEnum op, Expr* lhs, Expr* rhs);

		};

		class LogicExpr : public BinaryExpr
		{
		public:
			LogicExpr(OperatorsEnum op, Expr* lhs, Expr* rhs);
		};

		class ConditionalExpr : public Expr
		{
		public:
			ConditionalExpr(Expr* condition, Expr* true_expr, Expr* false_expr);
			const Expr* Condition() const;
			const Expr* TrueExpr() const;
			const Expr* FalseExpr() const;
		};

		class TypeExpr : public Node
		{
		public:
			TypeExpr(QualifiedTypeSpecifier* qual_type_specifier, Declarator* declarator);
			QualifiedTypeSpecifier* QualifiedTypeSpecifier();
			Declarator*	Declarator();
			QualType DeclType() const;
		};

		class SizeofExpr : public Expr
		{
		public:
			SizeofExpr(Expr* expr);
		};

		class SizeofTypeExpr : public Expr
		{
		public:
			SizeofTypeExpr(TypeExpr* type_expr);
		};

		class MemberExpr : public Expr
		{
		public:
			MemberExpr(Expr* host, const std::string &member_name, OperatorsEnum op);
			const Expr* Host() const;
			const FieldDeclaration* Member() const; //Incomplete
			// "." is direct member operator
			// "->" is indirect member operator
			bool IsDirect() const;
		};

		class CastExpr : public Expr
		{
		public:
			const Expr* SourceExpr() const;
			const QualType TargetType() const;
			const FunctionDeclaration* ConversionFunction() const;
		};
		class ImplicitCastExpr : public CastExpr
		{
		public:
			ImplicitCastExpr(const Type* target_type, Expr* source_expr);
		};

		class ExplicitCastExpr : public CastExpr
		{
		public:
			ExplicitCastExpr(TypeExpr* target_type_expr, Expr* source_expr);
		};

		class ConstantLiteralExpr : public Expr
		{};

		class StringLiteral : public ConstantLiteralExpr
		{
		public:
			explicit StringLiteral(const char* raw_str);
			const std::string& Value() const;
		};
		class IntegerLiteral : public ConstantLiteralExpr
		{
		public:
			explicit IntegerLiteral(int value, size_t decimal = 10, bool is_signed = true);
			explicit IntegerLiteral(const char* raw_str);
			const int Value() const;
		};
		class FloatLiteral : public ConstantLiteralExpr
		{
		public:
			explicit FloatLiteral(float value, bool is_double = false);
			explicit FloatLiteral(const char* raw_str);
			const float Value() const;
		};
		class CharacterLiteral : public ConstantLiteralExpr
		{
		public:
			explicit CharacterLiteral(const char* raw_str);
			const char Value() const;
		};

		class Initializer : public Expr
		{
		public:
			explicit Initializer(Expr* expr);
			const Expr* Value() const;
			const bool IsList() const;
			const bool IsValue() const;
		};
		class InitializerList : public Initializer ,public std::list<Initializer*>
		{
		public:
			explicit InitializerList(std::list<Initializer*>* list);
			explicit InitializerList(const std::list<Initializer*>* list);
		};

		class CommaExpr : public BinaryExpr
		{
		public:
			CommaExpr(Expr* lhs, Expr* rhs);
		};

		class Stmt : public Node
		{
		public:
		};

		// it's a simple ";"
		class NullStmt : public Stmt
		{
		public :
			NullStmt();
		};

		// wrap an declaration into an statement
		class DeclStmt : public Stmt
		{

		};

		class CompoundStmt : public Stmt , public DeclContext
		{
		public:
			explicit CompoundStmt(std::list<Stmt*>*);
			const std::list<Stmt*>& SubStmts() const;
			const DeclContext* DeclarationContext() const;
		};
		class ExprStmt : public Stmt
		{
		public:
			explicit ExprStmt(Expr* expr);
			const Expr* Expression() const;
			Expr* Expression();
			void SetExpression(Expr* expr);
		};
		class ReturnStmt : public Stmt
		{
		public:
			explicit ReturnStmt(Expr* return_expr = nullptr);
			const Expr* ReturnExpr() const;
		};

		class ContinueStmt : public Stmt
		{
		public:
			ContinueStmt();
		};

		class BreakStmt : public Stmt
		{
		public:
			BreakStmt();

		};

		class CaseLabel : public Stmt
		{
		};

		class IterationStmt : public Stmt
		{
		public:
			const Stmt* Action() const;
		};

		class WhileStmt : public IterationStmt
		{
		public:
			WhileStmt(Expr* condition, Stmt* action);
			const Expr* Condition() const;
		};

		class ForStmt : public IterationStmt
		{
		public:
			ForStmt(Stmt* initialize_stmt, Expr* condition, Stmt* post_action, Stmt* action);
			const Stmt* Initializer() const;
			const Expr* Condition() const;
			const Expr* PostAction() const;
		};

		class DoWhileStmt : public IterationStmt
		{
		public:
			DoWhileStmt(Stmt* action, Expr* condition);
		};

		class IfStmt : public Stmt
		{
		public:
			IfStmt(Expr* condition, Stmt* then_action, Stmt* else_action = nullptr);
			const Expr* Condition() const;
			const Stmt* Then() const;
			const Stmt* Else() const;
		};

		class TypeSpecifier : public Node
		{
		public:
			Type* RepresentType();
			const Type* RepresentType() const;
			void SetRepresentType(Type* type);
		};

		class PrimaryTypeSpecifier : public TypeSpecifier
		{
		public:
			PrimaryTypeSpecifier(Type*);
			std::string ToString() const;
		};

		class StructBody : public ScopeNode, public DeclContext
		{
		public:
			StructBody();
		};

		// represent a struct specifier
		// anything like "strcut foo" or "struct foo {..}" or "struct {...}"
		class StructDeclaration : public TypeSpecifier, public TypeDeclaration, public Redeclarable<StructDeclaration>
		{
		public:
			StructDeclaration(const std::string& name, StructBody* definition);
			StructDeclaration(const std::string& name);
			StructDeclaration(StructBody* definition);
			StructDeclaration()
			{
				SetKind(DECL_STRUCT);
			}

			const std::string & Name() const;
			StructBody* Definition();
			void SetDefinition(StructBody*);
			StructBody* LatestDefinition();
		};


		class EnumSpecifier : public TypeSpecifier
		{
		public:
			EnumSpecifier(const std::string& name, std::list<Enumerator*>* definition);
			EnumSpecifier(std::list<Enumerator*>* definition);
			EnumSpecifier(const std::string& name);
		};

		class TypedefNameSpecifier : public TypeSpecifier
		{
		public:
			TypedefNameSpecifier(const std::string& name);
		};

		class QualifiedTypeSpecifier : Node
		{
		public:
			QualifiedTypeSpecifier(int, TypeSpecifier*);
			int Qualifiers() const;
			TypeSpecifier* TypeSpecifier() const;
			QualType RepresentType() const;
		};

		template <typename T>
		class CompoundDeclaration
		{
		public:
			const QualifiedTypeSpecifier* DeclQualifiedTypeSpecifier() const;
			const std::list<Declarator*>& DeclaratorList() const;
			const std::list<T*>& Declarations() const;
			std::list<T*>& Declarations();
		};

		class VarDeclStmt : public DeclStmt, public CompoundDeclaration<ValueDeclaration>
		{
		public:
			VarDeclStmt(StorageClassSpecifierEnum, QualifiedTypeSpecifier*, std::list<Declarator*>*);
			StorageClassSpecifierEnum StorageClassSpecifier() const;
		};

		class TypedefStmt : public DeclStmt, public CompoundDeclaration<TypeDeclaration>
		{
		public:
			TypedefStmt(QualifiedTypeSpecifier*, std::list<Declarator*>*);
		};

		class FieldDeclStmt : public DeclStmt, public CompoundDeclaration<FieldDeclaration>
		{
		public:
			FieldDeclStmt(QualifiedTypeSpecifier*, std::list<Declarator*>*);
		};

		// A simple declaration container for parameters
		class ParameterList : public Node, public DeclContext
		{
		public:
			ParameterList()
			{}
		};

		class FunctionDeclaration : public Node, public ValueDeclaration, public Redeclarable<FunctionDeclaration>
		{
		public:
			FunctionDeclaration(StorageClassSpecifierEnum scs, QualifiedTypeSpecifier *qualified_type_specifier, Declarator* declarator)
				: m_storage_specifier(scs), m_type_specifier(qualified_type_specifier), m_declarator(declarator)
			{

			}

			const QualifiedTypeSpecifier* DeclTypeSpecifier() const { return m_type_specifier.get(); }
			Declarator* GetDeclarator() const { return m_declarator.get(); }

			Stmt* Definition()  { return m_definition.get(); }
			void SetDefinition(Stmt* def) { m_definition.reset(def); }
			Stmt* LatestDefinition()
			{
				auto func = this;
				while (!func->Definition())
				{
					func = func->prev();
				}
				return func->Definition();
			}

			ParameterList& ParameterList();
			QualType& ReturnType();
		protected:
			//Literal entities
			StorageClassSpecifierEnum				m_storage_specifier;
			std::unique_ptr<QualifiedTypeSpecifier>	m_type_specifier;
			std::unique_ptr<Declarator>				m_declarator;
			std::unique_ptr<Stmt>					m_definition;
		};

		// Represent an Parameter Declaration
		// Literal & Semantic Entity
		// Parameter's Declaration is not ambiguous , we can unify the literal and semantic entity
		class ParameterDeclaration : public Node , public Declaration
		{
		public:
			ParameterDeclaration(QualifiedTypeSpecifier*, Declarator*);
			QualifiedTypeSpecifier* QualifiedTypeSpecifier();
			Declarator*	Declarator();
		};

		class Declarator : public Node
		{
		public:
			// The most "basic" declarator of this declarator
			Declarator* Atom();
			// The base declarator of this declarator
			// return nullptr if it's already the deepest layer
			Declarator* Base();
		protected:
			Declarator(Declarator* base);
			Declarator* m_base;
		};

		class InitDeclarator : public Declarator
		{
		public:
			InitDeclarator(Declarator* declarator, Initializer* initializer);
			const Initializer* Initializer() const;
		};

		class ParenDeclarator : public Declarator
		{
		public:
			ParenDeclarator(Declarator* base);
			const Declarator* Base() const;
		};

		class IdentifierDeclarator : public Declarator
		{
		public:
			IdentifierDeclarator(const std::string& name);
			const std::string& Identifier() const;
		};

		class PointerDeclarator : public Declarator
		{
		public:
			PointerDeclarator(std::list<int>* qualfier_mask_list, Declarator* base);
			const Declarator* Base() const;
			unsigned QualifierMask() const;
		};

		class ArrayDeclarator : public Declarator
		{
		public:
			ArrayDeclarator(Declarator* base,Expr* size);
			const Declarator* Base() const;
			const Expr* SizeExpr() const;
		};

		class FunctionalDeclarator : public Declarator
		{
		public:
			FunctionalDeclarator(Declarator* base, ParameterList* param_list);
			const Declarator* Base() const;
			const ParameterList& ParameterList() const;
		};

		class FieldDeclarator : public Declarator
		{
		public:
			FieldDeclarator(Declarator* base, Expr* offset = nullptr);
			FieldDeclarator(Expr* offset);
		};

		class Enumerator : public Declarator
		{
		public:
			explicit Enumerator(const std::string &name, Expr* value_expr = nullptr);
			const std::string& Name() const;
			const Expr* Value() const;
		};


	}

}

#endif // !_AST_H
