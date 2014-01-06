#pragma once
#ifndef _AST_H
#define _AST_H

//#include "util.h"
//#include "symtab.h"
#include <string>
#include <list>
#include <memory>
#include <array>
#include "location.hh"
#include "declaration.h"
#include "operators.h"

namespace C1
{
	class Type;
	class DeclContext;


	namespace AST
	{
		template <typename TargetType, typename SourceType>
		bool isa(typename SourceType* var)
		{
			static_assert(std::is_pointer<TargetType>::value);
			TargetType ptr = dynamic_cast<TargetType> var;
			return ptr != nullptr;
		};

		class TranslationUnit;
		// Storage nesseary "global" context value for building AST
		struct ASTContext
		{
		public:
			// The current translation unit
			TranslationUnit* CurrentTranslationUnit;
			// Storage the current working scope , the comming declaration is stored in it
			DeclContext*	CurrentDeclContext;
			// The type context object for creating and retriving predefined types
			TypeContext*	TypeContext;
			// The name of the current working source file
			std::string		FileName;
			// The input stream for source file
			std::istream&	SourceFile;
			// Storage the qualified type for declarations.
			QualType		CurrentQualifiedType;
			
		};
		// Represent a literal entity in the code document's AST
		// We have TypeSpecifier Here but not the true underhood Type here.
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

			//virtual const std::list<const Node*> Children() const = 0;
			//virtual std::list<Node*> Children() = 0;

			virtual std::string ToString() const/* = 0*/;

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

		class TranslationUnit : public Node , public DeclContext
		{
		public:
			const std::string& Name() const;
			std::list<Declaration*>& Declarations();
			const std::list<Declaration*>& Declarations() const;
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

			template <typename value_type>
			value_type Evaluate() const;
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

			CallExpr(Node* , std::list<std::unique_ptr<Expr>>&&);
		private:
			Expr* m_Callee;
			std::list<std::unique_ptr<Expr>> m_ArgumentList;
		};

		class DeclRefExpr : public Expr
		{
		public:
			const Declaration* Declaration() const;
		};

		class UnaryExpr : public Expr
		{
		public:
			const Expr* SubExpr() const;
			OperatorsEnum Operator() const;
		private:
			std::unique_ptr<Expr> m_SubExpr;
		};

		class BinaryExpr : public Expr
		{
		public:
			const std::array<Expr*, 2>& SubExprs() const;
			const Expr* LeftSubExpr() const;
			const Expr* RightSubExpr() const;
			const OperatorsEnum Operator() const;
		private:
			std::array<Expr*, 2> m_SubExprs;
		};

		class AssignExpr : public Expr
		{
		public:
			const Expr* Value() const;
			const Expr* Assignee() const;
		};

		class IndexExpr : public BinaryExpr
		{
		public:

		};

		class CompoundAssignExpr : public AssignExpr
		{
		public:
			const OperatorsEnum Operator() const;
		};

		class ArithmeticExpr : public BinaryExpr
		{
		public:

		};

		class LogicExpr : public BinaryExpr
		{
		public:

		};

		class ConditionalExpr : public Expr
		{
		public:
			const Expr* Condition() const;
			const Expr* TrueExpr() const;
			const Expr* FalseExpr() const;

			ConditionalExpr(Expr* condition, Expr* true_expr, Expr* flase_expr);
		};

		class MemberExpr : public Expr
		{
		public:
			const Expr* Host() const;
			const Node* Member() const; //Incomplete

			MemberExpr(Expr* host,Node* Member);
		};
		class CastExpr : public Expr
		{
		public:
			const Expr* SourceExpr() const;
			const Type* TargetType() const;
			const Node* ConversionFunction() const;
		};
		class ImplicitCastExpr : CastExpr
		{
		public:
			ImplicitCastExpr(Expr* source_expr, const Type* target_type);
		};
		class ExplicitCastExpr : CastExpr
		{
		public:
			ExplicitCastExpr(Expr* source_expr, const Type* target_type, const Node* conversion_function = nullptr);
		};

		class StringLiteral : public Expr
		{
		public:
			const std::string& Value() const;
		};
		class IntegerLiteral : public Expr
		{
		public:
			const int Value() const;
		};
		class FloatLiteral : public Expr
		{
		public:
			const float Value() const;
		};
		class CharacterLiteral : public Expr
		{
		public:
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
			explicit InitializerList(std::list<Initializer*>&& list);
			explicit InitializerList(const std::list<Initializer*>& list);
		};

		class Stmt : public Node
		{
		public:
		};
		class CompoundStmt : public Stmt
		{
		public:
			const std::list<Stmt*>& SubStmts() const;
			const DeclContext* DeclarationContext() const;
		};
		class ExprStmt : public Stmt
		{
		public:
			const Expr* Expression() const;
		};
		class ReturnStmt : public Stmt
		{
		public:
			const Expr* ReturnExpr() const;
		};

		class IterationStmt : public Stmt
		{
		public:
			const Stmt* Action() const;
		};

		class WhileStmt : public IterationStmt
		{
		public:
			const Expr* Condition() const;
		};

		class ForStmt : public IterationStmt
		{
		public:
			const Stmt* Initializer() const;
			const Expr* Condition() const;
			const Expr* PostAction() const;
		};

		class IfStmt : public Stmt
		{
		public:
			const Expr* Condition() const;
			const Stmt* Then() const;
			const Stmt* Else() const;
		};

		class VarDeclStmt : public Stmt
		{
		public:
		};

		class FunctionDefination : public Node
		{
		public:
			FunctionDefination(StorageClassSpecifierEnum scs, QualType qual_type, Declarator* declarator, Stmt* body);
			const Stmt* Body() const;
		};
		//class Variable : Node
		//{};

		//class Functional : Node
		//{
		//	virtual std::list<Node*> ParameterList() const;
		//};

		class Enumerator : public Node
		{
		public:
			explicit Enumerator(const std::string &name, Expr* value_expr = nullptr);
			const std::string& Name() const;
			const Expr* Value() const;
		};

		class Declarator : public Node
		{
		public:
			bool IsAbstract() const;
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
			PointerDeclarator(int qualfier_mask, Declarator* base);
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
			FunctionalDeclarator(Declarator* base, std::list<Declaration*>* param_list);
			const Declarator* Base() const;
			const std::list<ParameterDeclaration*>& ParameterList() const;
		};

	}

}
//typedef struct location {
//	int first_line;
//	int first_column;
//	int last_line;
//	int last_column;
//} *Loc;
//
//typedef struct {
//	int 	op;
//	int		type;
//	int 	val;
//	struct astnode	*oprands[2];// kids of the AST node
//} *Expr;
//
//typedef struct{
//	Symbol sym; //no ownership, symbol's owner's ship belongs to defination nodes
//	List arglist;
//} *CallExpr;
//
//typedef struct{
//	//SymbolTable localTable;
//	List stmts;
//} *ComposeStmt;
//
//typedef struct{
//	struct astnode *condition;
//	struct astnode *thenAction;
//	struct astnode *elseAction;
//} *IfStmt;
//
//typedef struct{
//	char *name;
//	int num;
//} *AssignStmt;
//
//typedef struct{
//	Symbol sym;
//	struct astnode *body;
//} *Function;
//
//typedef struct{
//	Type type;
//	List vars;
//} *VarDeclStmt;
//
//typedef struct{
//	//SymbolTable globalTable;
//	List decls;
//	struct astnode * main;
//} *Program;
//
////------------------------------------------------------------------
//typedef struct{
//	struct astnode *condition;
//	struct astnode *action;
//} *WhileStmt;
//
//typedef struct astnode{
//	enum AST_NODE_KINDS{
//		KNumber = 0x200,		// numerial value:
//		KVarExpr,
//		KParenExpr,		// parentheses expression
//		KInfixExpr,		// infix expression
//		KPrefixExpr,
//		KRelationExper,
//
//		KProgram,	// Program is decls
//		KFunction,
//		KMainFunction,
//		KVariable,
//		KConstant,
//
//		KVarDeclStmt,
//		KConstDeclStmt,
//
//		KComposeStmt,
//		KIfStmt,	//If statement
//		KWhileStmt,
//		KCallExper,
//		KAssignExpr,		// assignment expression
//	} kind;	// kind of the AST node
//
//	// information of various kinds of AST node 
//	union {		
//		int  val;		// KValue
//		Symbol sym;		// KVariable , KConstant , KVarExpr
//		Expr   expr;		// KPrefixExpr,KInfexExpr,KparenExpr,KAssignExpr,KRelationExpr
//		CallExpr callexpr;	//KCallExpr
//
//		Program	program;	//KProgram
//		VarDeclStmt vardeclstmt;	//KVariableList,KConstantList
//		Function function;	//KFunction
//
//		ComposeStmt compstmt;	//KComposeStmt
//		WhileStmt whilestmt;	//KWhileStmt
//		IfStmt ifstmt;	//KIfStmt
//	};
//	Loc 	loc;			// locations
//} *ASTNode;
//
//typedef struct astree {
//	ASTNode root;
//	SymbolTable symTab;
//} *ASTree;
//
//// functions for creating various kinds of ASTnodes
//bool IsStatement(ASTNode node);
//bool IsExpersion(ASTNode node);
//bool IsDeclartion(ASTNode node);
//// Symbols and declarations
//ASTNode newVariable(SymbolTable pTab, const char* name, ASTNode initExpr);
//ASTNode newConstant(SymbolTable pTab, const char* name, ASTNode initExpr);
//ASTNode newFunction(SymbolTable pTab, const char* name, ASTNode body);
////void destroyVariable();
//void destroyFunction(Function *pFunc);
//
//ASTNode newVarDeclStmt(Type type);
//ASTNode newConstDeclStmt(Type type);
//void	destroyVarDeclStmt(VarDeclStmt *pvarlist);
//
//// Expressions
//ASTNode newNumber(int value);
//ASTNode newVarExpr(SymbolTable pTab, const char* name);
//ASTNode newPrefixExpr(int op, ASTNode exp);
//ASTNode newParenExpr(ASTNode exp);
//ASTNode newInfixExpr(int op, ASTNode left, ASTNode right);
//ASTNode newAssignExpr(int op, ASTNode left, ASTNode right);
//ASTNode newRelationExpr(int relop, ASTNode lkid, ASTNode rkid);
//void	destroyExpr(Expr *pexp);
//
//ASTNode newCallExpr(SymbolTable pTab, char* name);
//void	destroyCallExpr(CallExpr *loop);
//
//ASTNode newProgram();
//void	destroyProgram(Program *prog);
//
////Statments
//ASTNode newComposeStmt();
//void	destroyComposeStmt(ComposeStmt *loop);
//ASTNode newIfStmt(ASTNode condition, ASTNode action);
//void	destroyIfStmt(IfStmt *loop);
//ASTNode newWhileStmt(ASTNode condition, ASTNode action);
//void	destroyWhileStmt(WhileStmt *loop);
//
//void	destroyASTNode(ASTNode *pnode);
//
//ASTree newAST();
//void	destroyAST(ASTree *pTree);
//
//void 	dumpASTNode(ASTNode node, int indent);
//
//Loc		setLoc(ASTNode node, Loc loc);

#endif // !_AST_H
