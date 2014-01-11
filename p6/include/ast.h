#pragma once
#ifndef _AST_H
#define _AST_H

//#include "util.h"
//#include "symtab.h"
#include <string>
#include <list>
#include <memory>
#include <array>
#include <vector>
#include "ast_node.h"
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
		class FunctionalDeclarator;

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


		class Comment : public Node
		{
		public:
			const std::string &Content() const;
			std::string &Content();
		};

		// Represent the node which contains a collection of child but not some specifid number of children
		class ScopeNode
		{
		public:
			std::list<Node*>& Children() { return m_Chilren; }
			const std::list<Node*>& Children() const { return m_Chilren; }
		protected:
			//void Dump(std::ostream&) const;
			std::list<Node*> m_Chilren;
		};

		class TranslationUnit : public Node, public ScopeNode, public DeclContext
		{
		public:
			TranslationUnit(std::string filename); 
			~TranslationUnit();

			const std::string & Name() const { return m_FileName; }
			virtual void Dump(std::ostream& ostr) const;

		protected:
			std::string m_FileName;
		};

		enum ExprValueType
		{
			RValue,
			LValue,
			XValue,
		};

		union ComposedValue
		{
			bool Bool;
			char Char;
			int Int;
			float Float;
			const char* String;
			void* Pointer;
			std::vector<ComposedValue>* ValueList;
			//Well, array is a little tricky, we have initializer list but not this.
			//void* Array;
		};

		class Expr : public Node
		{
		public:
			typedef ComposedValue ComposedValue;
			virtual QualType ReturnType() const = 0;
			virtual ExprValueType ValueType() const = 0;
			virtual bool HasSideEffect() const = 0;

			virtual ComposedValue Evaluate() const;

			virtual bool Evaluatable() const = 0;
		//protected:
		//	value_type m_Value;
		};

		class CallExpr : public Expr
		{
		public:
			const std::list<Expr*>& Arguments() const
			{
				return m_Arguments;
			}

			const Expr* FunctionalExpr() const
			{
				return m_FuncExpr.get();
			}

			~CallExpr();

			CallExpr(Expr* , std::list<Expr*>*);

			void Dump(std::ostream&) const;

			virtual QualType ReturnType() const;

			virtual ExprValueType ValueType() const;

			virtual bool HasSideEffect() const;

			// Always false, ignore the C++11 const-expr specifier
			virtual bool Evaluatable() const;

		private:
			std::unique_ptr<Expr> m_FuncExpr;
			// Call Expr owns all it's arguments
			std::list<Expr*> m_Arguments;
		};

		class DeclRefExpr : public Expr
		{
		public:
			DeclRefExpr(DeclContext* lookup_context,const std::string &name);
			const ValueDeclaration* RefDecl() const { return m_RefDecl; }

			const std::string& Identifier() const { return m_Name; }
			// It's safe.
			void SetIdentifier(const std::string val);

			const DeclContext* RefDeclContext() const { return m_RefContext; }
			// It's safe.
			void SetRefDeclContext(DeclContext* reference_context);

			void Dump(std::ostream&) const;
		protected:
			void ResoloveReference();

			virtual QualType ReturnType() const;

			virtual ExprValueType ValueType() const;

			virtual bool HasSideEffect() const;

			virtual bool Evaluatable() const;

			std::string m_Name;

			DeclContext*	  m_RefContext; //The declaration context which the where this ref should lookup

			// DeclRefExpr don't owning its reference declaration
			// It's a cache
			ValueDeclaration* m_RefDecl;
		};

		class ParenExpr : public Expr
		{
		public:
			ParenExpr(Expr* base);
			Expr* Base() { return m_Base.get(); }
			void Dump(std::ostream&) const;

			virtual QualType ReturnType() const;

			virtual ExprValueType ValueType() const;

			virtual bool HasSideEffect() const;

			virtual bool Evaluatable() const;

		protected:
			std::unique_ptr<Expr> m_Base;
		};

		class UnaryExpr : public Expr
		{
		public:
			UnaryExpr(OperatorsEnum op, Expr* sub_expr);
			const Expr* SubExpr() const { return m_SubExpr.get(); }
			Expr* SubExpr() { return m_SubExpr.get(); }
			OperatorsEnum Operator() const { return m_Operator; }
			void Dump(std::ostream&) const;

			virtual QualType ReturnType() const;

			virtual ExprValueType ValueType() const;

			virtual bool HasSideEffect() const;

			virtual bool Evaluatable() const;

		protected:
			OperatorsEnum m_Operator;
			std::unique_ptr<Expr> m_SubExpr;
		};

		class PosfixExpr : public UnaryExpr
		{
		public:
			PosfixExpr(OperatorsEnum op, Expr* Base);
			void Dump(std::ostream&) const;
		protected:
		};

		class IncrementExpr : public UnaryExpr
		{
			// Always LValue
			virtual ExprValueType ValueType() const;
			// Always True
			virtual bool HasSideEffect() const;
			// Always False ,something like "++3" is always illegal
			virtual bool Evaluatable() const;

		};

		class PosfixIncrementExpr : public IncrementExpr
		{};

		class BinaryExpr : public Expr
		{
		public:
			BinaryExpr(OperatorsEnum op, Expr* lhs, Expr* rhs);
			const Expr* LeftSubExpr() const { return m_SubExprs[0].get(); }
			const Expr* RightSubExpr() const { return m_SubExprs[1].get(); }
			Expr* LeftSubExpr() { return m_SubExprs[0].get(); }
			Expr* RightSubExpr() { return m_SubExprs[1].get(); }
			const OperatorsEnum Operator() const { return m_Operator; }
			void Dump(std::ostream&) const;

			virtual QualType ReturnType() const;

			virtual ExprValueType ValueType() const;

			virtual bool HasSideEffect() const;

			virtual ComposedValue Evaluate() const;

			virtual bool Evaluatable() const;

		protected:
			OperatorsEnum m_Operator;
			std::array<std::unique_ptr<Expr>, 2> m_SubExprs;
		};

		class DeReferenceExpr : public UnaryExpr
		{
			// Always LValue
			virtual ExprValueType ValueType() const;
			// Always False
			virtual bool Evaluatable() const;
			// de-ref of the sub-expr
			virtual QualType ReturnType() const;
			// Side effect : Always False , if there is no operator overload
		};

		class AddressOfExpr : public UnaryExpr
		{
			// Always RValue
			virtual ExprValueType ValueType() const;
			// Always False
			virtual bool Evaluatable() const;
			// const (pointer of the sub-expression's type)
			virtual QualType ReturnType() const;
		};

		class AssignExpr : public BinaryExpr
		{
		public:
			AssignExpr(Expr* lhs, Expr* rhs);
			AssignExpr(OperatorsEnum op, Expr* lhs, Expr* rhs);

			bool IsComposed() const { return Operator() != OP_ASSIGN; }
			// return OP_ASGN if is OP_ASGN itself
			const OperatorsEnum PrefixedOperator() const;

			// The lhs
			virtual QualType ReturnType() const;

			// always LValue
			virtual ExprValueType ValueType() const;

			// always true
			virtual bool HasSideEffect() const;

			// Same as rhs for "=" , always false for ComposedAssignExpr 
			virtual bool Evaluatable() const;

			virtual ComposedValue Evaluate() const;

		};

		class IndexExpr : public BinaryExpr
		{
		public:
			IndexExpr(Expr* host_expr, Expr* index_expr);
			Expr* Host() { return LeftSubExpr(); }
			Expr* Index() { return RightSubExpr(); }
			const Expr* Host() const { return LeftSubExpr(); }
			const Expr* Index() const { return RightSubExpr(); }
			void Dump(std::ostream&) const;

			// De-ref of lhs
			virtual QualType ReturnType() const;
			// Always LValue
			virtual ExprValueType ValueType() const;

			virtual bool HasSideEffect() const;

			virtual ComposedValue Evaluate() const;

			// Always false
			virtual bool Evaluatable() const;

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
			void Dump(std::ostream&) const;

			const Expr* FalseExpr() const { return m_FalseExpr; }
			void SetFalseExpr(Expr* val) { m_FalseExpr = val; }
			const Expr* TrueExpr() const { return m_TrueExpr; }
			void SetTrueExpr(Expr* val) { m_TrueExpr = val; }
			const Expr* Condition() const { return m_Condition; }
			void SetCondition(Expr* val) { m_Condition = val; }

			virtual QualType ReturnType() const;

			virtual ExprValueType ValueType() const;

			virtual bool HasSideEffect() const;

			virtual ComposedValue Evaluate() const;

			virtual bool Evaluatable() const;

		protected:
			Expr* m_Condition, *m_TrueExpr, *m_FalseExpr;
		};

		class TypeExpr : public Node
		{
		public:
			TypeExpr(QualifiedTypeSpecifier* qual_type_specifier, Declarator* declarator);
			QualifiedTypeSpecifier* DeclSpecifier() { return m_Specifier.get(); }
			void SetDeclSpecifier(QualifiedTypeSpecifier*);
			Declarator*	DeclDeclarator() { return m_Declarator.get(); }
			void SetDeclarator(Declarator*);
			QualType DeclType() const { return m_Type; }
			void Dump(std::ostream&) const;
		protected:
			std::unique_ptr<Declarator> m_Declarator;
			std::unique_ptr<QualifiedTypeSpecifier> m_Specifier;
			QualType	m_Type; // It cached the type defined by Specifier and Declarator
		};

		class SizeofExpr : public  UnaryExpr
		{
		public:
			SizeofExpr(Expr* expr);

			//inherit from UnaryExpr is enough
			//void Dump(std::ostream&) const;

			// Always int
			virtual QualType ReturnType() const;

			// Always RValue
			virtual ExprValueType ValueType() const;

			virtual bool Evaluatable() const;

			virtual ComposedValue Evaluate() const;

		};

		class SizeofTypeExpr : public Expr
		{
		public:
			SizeofTypeExpr(TypeExpr* type_expr);
			void Dump(std::ostream&) const;
			const TypeExpr* SubExpr() const { return m_TypeExpr.get(); }

			virtual QualType ReturnType() const;

			virtual ExprValueType ValueType() const;

			virtual bool HasSideEffect() const;

			virtual ComposedValue Evaluate() const;

			virtual bool Evaluatable() const;

		protected:
			std::unique_ptr<TypeExpr> m_TypeExpr;
		};

		class MemberExpr : public Expr
		{
		public:
			MemberExpr(Expr* host, const std::string &member_name, OperatorsEnum op);
			const Expr* Host() const 
			{ return m_HostExpr.get(); }
			const FieldDeclaration* Member() const
			{ return m_MemberDeclaration; }
			// "." is direct member operator
			// "->" is indirect member operator
			bool IsDirect() const;
			void Dump(std::ostream&) const;

			virtual QualType ReturnType() const;

			virtual ExprValueType ValueType() const;

			virtual bool HasSideEffect() const;

			virtual bool Evaluatable() const;

		protected:
			OperatorsEnum			m_Operator;
			std::unique_ptr<Expr>	m_HostExpr;
			std::string				m_MemberName;
			FieldDeclaration*		m_MemberDeclaration;
		};

		class CastExpr : public Expr
		{
		public:
			const Expr* SourceExpr() const { return m_SourceExpr.get(); }
			const QualType TargetType() const { return m_TargetType; }
			const FunctionDeclaration* ConversionFunction() const { return m_ConversionFunction; }
			void SetConversionFunction(FunctionDeclaration* val) { m_ConversionFunction = val; }
		protected:
			CastExpr(Expr* source_expr, QualType target_type, FunctionDeclaration* conversion_func = nullptr);

			virtual QualType ReturnType() const;

			virtual ExprValueType ValueType() const;

			virtual bool HasSideEffect() const;

			virtual ComposedValue Evaluate() const;

			virtual bool Evaluatable() const;

			virtual void Dump(std::ostream& ostr) const;

			FunctionDeclaration* m_ConversionFunction;
			QualType m_TargetType;
			std::unique_ptr<Expr> m_SourceExpr;
		};
		class ImplicitCastExpr : public CastExpr
		{
		public:
			ImplicitCastExpr(QualType target_type, Expr* source_expr);
			void Dump(std::ostream&) const;
		};

		class ExplicitCastExpr : public CastExpr
		{
		public:
			ExplicitCastExpr(TypeExpr* target_type_expr, Expr* source_expr);
			const TypeExpr* TargetCastTypeExpr() const { return m_TargetTypeExpr.get(); }
			void Dump(std::ostream&) const;
		protected:
			std::unique_ptr<TypeExpr> m_TargetTypeExpr;
		};

		class ConstantLiteral : public Expr
		{
		public:
			virtual ExprValueType ValueType() const;

			virtual bool HasSideEffect() const;

			virtual ComposedValue Evaluate() const;

			virtual bool Evaluatable() const;

			virtual void Dump(std::ostream& ostr) const;

			const std::string& RawLiteral() const { return m_RawLiteral; }
	
			void SetValue(ComposedValue val) { m_Value = val; }
		protected:
			ConstantLiteral(TypeContext* type_context, const std::string& raw_str);

			ComposedValue		m_Value;
			TypeContext*	m_TypeContext;
			std::string		m_RawLiteral;


		};

		class StringLiteral : public ConstantLiteral
		{
		public:
			explicit StringLiteral(TypeContext* type_context, const char* raw_str);

			virtual QualType ReturnType() const;

			// Not impleamented yet
			static std::string DecodeString(const std::string & encoded_str);
			static std::string EncodeString(const std::string & raw_str);
		protected:
			std::string m_DecodedString; // The true value of this string literal
		};

		class IntegerLiteral : public ConstantLiteral
		{
		public:
			//explicit IntegerLiteral(int value, );
			IntegerLiteral(TypeContext* type_context, const char* raw_str, int value,int dec);

			virtual QualType ReturnType() const;
		protected:
			int m_decimal;
		};

		class FloatLiteral : public ConstantLiteral
		{
		public:
			//explicit FloatLiteral(float value, bool is_double = false);
			FloatLiteral(TypeContext* type_context, const char* raw_str, float value);

			virtual QualType ReturnType() const;

		};

		class CharacterLiteral : public ConstantLiteral
		{
		public:
			CharacterLiteral(TypeContext* type_context, const char* raw_str, char value);

			virtual QualType ReturnType() const;

		};

		class Initializer : public Expr
		{
		public:
			virtual bool IsList() const = 0;
		protected:
			Initializer();
		};

		class AtomInitializer : public Initializer
		{
		public:
			AtomInitializer(Expr* value_expr);
			bool IsList () const { return false; }

			virtual QualType ReturnType() const;

			virtual ExprValueType ValueType() const;

			virtual bool HasSideEffect() const;

			virtual ComposedValue Evaluate() const;

			virtual bool Evaluatable() const;

			virtual void Dump(std::ostream& ostr) const;

		protected:
			std::unique_ptr<Expr> m_ValueExpr;
		};

		class InitializerList : public Initializer ,public std::list<Initializer*>
		{
		public:
			//explicit InitializerList(std::list<Initializer*>& list);
			//explicit InitializerList(const std::list<Initializer*>& list);
			InitializerList(TypeContext *context);
			bool IsList() const { return true; }
			void Dump(std::ostream&) const;

			virtual QualType ReturnType() const;

			virtual ExprValueType ValueType() const;

			virtual bool HasSideEffect() const;

			virtual Expr::ComposedValue Evaluate() const;

			virtual bool Evaluatable() const;

		protected:
			TypeContext *m_TypeContext;
			QualType m_ReturnType;
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
			NullStmt() {}
			void Dump(std::ostream&) const;
		};

		// wrap an declaration into an statement
		class DeclStmt : public Stmt
		{

		};

		class CompoundStmt :public Stmt, public ScopeNode, public DeclContext
		{
		public:
			CompoundStmt();
			void Dump(std::ostream&) const;
		};

		class ExprStmt : public Stmt
		{
		public:
			explicit ExprStmt(Expr* expr);
			const Expr* Expression() const { return m_Expr.get(); }
			Expr* Expression() { return m_Expr.get(); }
			void SetExpression(Expr* expr) { m_Expr.reset(expr); }
			void Dump(std::ostream&) const;
		protected:
			std::unique_ptr<Expr> m_Expr;
		};

		class ReturnStmt : public ExprStmt
		{
		public:
			explicit ReturnStmt(Expr* return_expr = nullptr);
			void Dump(std::ostream&) const;
		};

		class ContinueStmt : public Stmt
		{
		public:
			ContinueStmt();
			void Dump(std::ostream&) const;
		};

		class BreakStmt : public Stmt
		{
		public:
			BreakStmt();
			void Dump(std::ostream&) const;
		};

		class CaseLabel : public Stmt
		{
		};

		class IterationStmt : public Stmt
		{
		public:
			const Stmt* Action() const { return m_Action.get(); }
			const Expr* Condition() const { return m_Condition.get(); }
			void SetAction(Stmt* val) { m_Action.reset(val); }
			void SetCondition(Expr* val) { m_Condition.reset(val); }
		protected:
			IterationStmt(Stmt* action = nullptr, Expr* condtion = nullptr);
			std::unique_ptr<Expr> m_Condition;
			std::unique_ptr<Stmt> m_Action;
		};

		class WhileStmt : public IterationStmt
		{
		public:
			WhileStmt(Expr* condition, Stmt* action);
			void Dump(std::ostream&) const;
		};

		class ForStmt : public IterationStmt
		{
		public:
			ForStmt(Stmt* initialize_stmt, Expr* condition, Expr* post_action, Stmt* action);
			const Stmt* Initializer() const { return m_Initializer.get(); }
			const ExprStmt* PostAction() const { return m_PostAction.get(); }
			void Dump(std::ostream&) const;
		protected:
			std::unique_ptr<Stmt> m_Initializer;
			std::unique_ptr<ExprStmt> m_PostAction;
		};

		class DoWhileStmt : public IterationStmt
		{
		public:
			DoWhileStmt(Stmt* action, Expr* condition);
			void Dump(std::ostream&) const;
		};

		class IfStmt : public Stmt
		{
		public:
			IfStmt(Expr* condition, Stmt* then_action, Stmt* else_action = nullptr);
			const Expr* Condition() const { return m_Condition.get(); }
			const Stmt* Then() const { return m_ThenAction.get(); }
			const Stmt* Else() const { return m_ElseAction.get(); }
			void Dump(std::ostream&) const;

		protected:
			std::unique_ptr<Expr> m_Condition;
			std::unique_ptr<Stmt> m_ThenAction;
			std::unique_ptr<Stmt> m_ElseAction;
		};

		class TypeSpecifier : public Node
		{
		public:
			Type* RepresentType() { return m_RepresentType; }
			const Type* RepresentType() const { return m_RepresentType; }
			void SetRepresentType(Type* type) { m_RepresentType = type; }
		protected:
			TypeSpecifier(Type* represent_type)
				: m_RepresentType(represent_type)
			{}
			TypeSpecifier()
				: m_RepresentType(nullptr)
			{}

			virtual void Dump(std::ostream& ostr) const;

			Type* m_RepresentType;
		};

		class PrimaryTypeSpecifier : public TypeSpecifier
		{
		public:
			PrimaryTypeSpecifier(Type* type)
				: TypeSpecifier(type)
			{}
			void Dump(std::ostream&) const;
		};

		class StructBody :public Node, public ScopeNode, public DeclContext
		{
		public:
			StructBody();

			virtual void Dump(std::ostream& ostr) const;

		};

		// represent a struct specifier
		// anything like "struct foo" or "struct foo {..}" or "struct {...}"
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

			const std::string & Name() const { return m_Name; }
			StructBody* Definition() { return m_Definition.get(); }
			const StructBody* Definition() const { return m_Definition.get(); }
			void SetDefinition(StructBody* val);
			bool IsAnonymous() const { return m_Name == "%anonymous"; }
			// Retrieve the latest previous definition before this declaration
			StructBody* LatestDefinition();
			const StructBody* LatestDefinition() const;
			void Dump(std::ostream&) const;

			virtual DeclContext::InsertionResult AddToContext(DeclContext& context);

		protected:
			std::string m_Name;
			std::unique_ptr<StructBody> m_Definition;
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
			TypedefNameSpecifier(DeclContext* pContext, const std::string& name)
				: m_Name(name)
			{
				TypeDeclaration* decl = dynamic_cast<TypeDeclaration*>(pContext->lookup(name));
				if (decl)
				{
					SetRepresentType(decl->DeclType());
				}
			}
			void Dump(std::ostream&) const;
		protected:
			std::string m_Name;
		};

		class QualifiedTypeSpecifier : public Node
		{
		protected:
			int m_Qualifiers;
			std::unique_ptr<TypeSpecifier> m_TypeSpecifier;
		public:
			QualifiedTypeSpecifier(int, TypeSpecifier*);
			int Qualifiers() const { return m_Qualifiers; }
			TypeSpecifier* DeclTypeSpecifier() const { return m_TypeSpecifier.get(); }
			QualType RepresentType() const { return QualType(m_TypeSpecifier->RepresentType(),m_Qualifiers); }
			void Dump(std::ostream&) const;
		};

		template <typename T>
		class CompoundDeclaration : public DeclStmt
		{
		public:
			const QualifiedTypeSpecifier* DeclSpecifier() const { return m_QTSpecifier.get(); }
			const std::list<Declarator*>& DeclaratorList() const { return m_DeclaratorList; }
			const std::list<T*>& Declarations() const { return m_DeclarationList; }
			std::list<T*>& Declarations() { return m_DeclarationList; }
		protected:
			CompoundDeclaration(QualifiedTypeSpecifier* QTSpecifier, std::list<Declarator*> &&declarator_list);

			void Dump(std::ostream& os) const;

			std::unique_ptr<QualifiedTypeSpecifier> m_QTSpecifier;
			// Compound Declaration don't owns any declaration , this is a lazy access list.
			std::list<T*> m_DeclarationList;
			// Compound Declaration owns all the declarators inside it.
			std::list<Declarator*>	m_DeclaratorList;
		};

		class VarDeclStmt : public CompoundDeclaration<ValueDeclaration>
		{
		public:
			VarDeclStmt(StorageClassSpecifierEnum, QualifiedTypeSpecifier*, std::list<Declarator*>*);
			StorageClassSpecifierEnum StorageClassSpecifier() const { return m_StorageSpecifier; }
			void SetStorageClassSpecifier(StorageClassSpecifierEnum val) { m_StorageSpecifier = val; }
			void Dump(std::ostream&) const;

		protected:
			StorageClassSpecifierEnum m_StorageSpecifier;
		};

		class TypedefStmt : public CompoundDeclaration<TypeDeclaration>
		{
		public:
			TypedefStmt(QualifiedTypeSpecifier*, std::list<Declarator*>*);
			void Dump(std::ostream&) const;
		};

		class FieldDeclStmt : public CompoundDeclaration<FieldDeclaration>
		{
		public:
			FieldDeclStmt(QualifiedTypeSpecifier*, std::list<Declarator*>*);

			virtual void Dump(std::ostream& ostr) const;

		};

		class Declarator : public Node
		{
		public:
			// The most "basic" declarator of this declarator
			Declarator* Atom();
			const Declarator* Atom() const { return m_Base.get(); }
			// The base declarator of this declarator
			// return nullptr if it's already the deepest layer
			Declarator* Base() { return m_Base.get(); }
			const Declarator* Base() const { return m_Base.get(); }

			virtual QualType DecorateType(QualType base_type) = 0;
		protected:
			Declarator(Declarator* base = nullptr);
			std::unique_ptr<Declarator> m_Base;
		};

		class ParenDeclarator : public Declarator
		{
		public:
			ParenDeclarator(Declarator* base);
			QualType DecorateType(QualType base_type)
			{
				return Base()->DecorateType(base_type);
			}
			void Dump(std::ostream&) const;
		};

		class IdentifierDeclarator : public Declarator
		{
		public:
			IdentifierDeclarator(const std::string& name);
			const std::string& Identifier() const { return m_Name; }
			void SetIdentifier(const std::string& val);
			void Dump(std::ostream&) const;

			virtual QualType DecorateType(QualType base_type);

		protected:
			std::string m_Name;
		};

		class PointerDeclarator : public Declarator
		{
		public:
			PointerDeclarator(int qualfier_mask, Declarator* base);
			int QualifierMask() const { return m_Qualifiers; }
			void Dump(std::ostream&) const;

			virtual QualType DecorateType(QualType base_type);

		protected:
			int m_Qualifiers;
		};

		class ArrayDeclarator : public Declarator
		{
		public:
			ArrayDeclarator(Declarator* base,Expr* size);
			const Expr* SizeExpr() const { return m_SizeExpr.get(); }
			void Dump(std::ostream&) const;

			virtual QualType DecorateType(QualType base_type);

		protected:
			std::unique_ptr<Expr> m_SizeExpr;
		};

		class ParameterDeclaration;

		// A simple declaration container for parameters
		class ParameterList : public Node, public DeclContext
		{
		public:
			ParameterList();
			//override the original add 
			//InsertionResult add(ParameterDeclaration* param);
			void Dump(std::ostream&) const;
		};

		class FunctionalDeclarator : public Declarator
		{
		public:
			FunctionalDeclarator(Declarator* base, ParameterList* param_list);
			const ParameterList& Parameters() const { return *m_Parameters; }
			ParameterList& Parameters() { return *m_Parameters; }
			void Dump(std::ostream&) const;

			virtual QualType DecorateType(QualType base_type);

		protected:
			std::unique_ptr<ParameterList> m_Parameters;
		};

		// It's not really so good to recognize InitDeclarator as a "Declarator"
		class InitDeclarator : public Declarator
		{
		public:
			InitDeclarator(Declarator* declarator, Initializer* initializer);
			const Initializer* InitializeExpr() const { return m_Initializer.get(); }
			Initializer* InitializeExpr() { return m_Initializer.get(); }
			void SetInitializeExpr(Initializer* val) { m_Initializer.reset(val); }
			void Dump(std::ostream&) const;
			QualType DecorateType(QualType base_type)
			{
				return Base()->DecorateType(base_type);
			}
		protected:
			std::unique_ptr<Initializer> m_Initializer;
		};

		class FieldDeclarator : public Declarator
		{
		public:
			FieldDeclarator(Declarator* base, Expr* offset = nullptr);
			FieldDeclarator(Expr* offset);
			Expr* OffsetExpr() { return m_OffsetExpr.get(); }
			const Expr* OffsetExpr() const { return m_OffsetExpr.get(); }
			void Dump(std::ostream&) const;

			virtual QualType DecorateType(QualType base_type);
		protected:
			std::unique_ptr<Expr> m_OffsetExpr;
		};

		// Well , enumerator is not some kind of declarator...
		class Enumerator : public Node
		{
		public:
			explicit Enumerator(const std::string &name, Expr* value_expr = nullptr);
			const std::string& Name() const { return m_Name; }
			const Expr* Value() const { return m_ValueExpr.get(); }
			void Dump(std::ostream&) const;

			virtual QualType DecorateType(QualType base_type)
			{
				throw std::logic_error("The method or operation is not implemented.");
			}

		protected:
			std::string m_Name;
			std::unique_ptr<Expr> m_ValueExpr;
		};


	}

}

#endif // !_AST_H
