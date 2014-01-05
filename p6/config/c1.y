%require "3.0"
%skeleton "lalr1.cc"
%defines 
%define api.namespace {C1}
%define parser_class_name {BisonParser}
%parse-param { C1::FlexScanner &scanner }
%lex-param   { C1::FlexScanner &scanner }

%define parse.assert
%define parse.trace
%define parse.error verbose


%code requires {
	#include "operators.h"
	#include "type.h"
	#include "declaration.h"
	#include "ast.h"
	// Forward-declare the Scanner class; the Parser needs to be assigned a 
	// Scanner, but the Scanner can't be declared without the Parser
	namespace C1 {
		class FlexScanner;
	}

}


%union{
	int					ival;
	OperatorsEnum		operator;
	Expr*				expr;
	Stmt*				stmt;
	QualType*			qual_type;
	Type*				type;
	Node*				node;
	Declarator*			declarator;
	std::list<void*>*	list;
	std::list<Declarator*>*		declarator_list;
	std::list<Declaration*>*	decl_list;
	std::list<Stmt*>*	stmt_list;
	std::list<Expr*>*	expr_list;
	std::list<int>*		int_list;
	IdentifierInfo*		identifier;
}


%code {
	// Prototype for the yylex function
	static int yylex(C1::BisonParser::semantic_type * yylval,C1::BisonParser::location_type * yylloc, C1::FlexScanner &scanner);
	static int debug(const char* message)
	{
		return printf(message);
	}
}

%locations
%token EOF
%token WHILE IF ELSE FOR DO SWITCH GOTO
%token BREAK RETURN CONTINUE CASE DEFAULT
%token CONST VOLATILE RESTRICT
%token STATIC EXTERN AUTO REGISTER
%token INT VOID FLOAT DOUBLE SIGNED UNSIGNED LONG CHAR SHORT
%token STRUCT UNION ENUM
%token TYPEDEF
%token INT_LITERAL FLOAT_LITERAL STRING_LITERAL
%token NewIdentifier ObjectIdentifier TypeIdentifier
%token COMMA SEMICOLON COLON QUESTION

%token ASSIGN MUL_ASSIGN ADD_ASSIGN DIV_ASSIGN SUB_ASSIGN AND_ASSIGN OR_ASSIGN LSH_ASSIGN RSH_ASSIGN MOD_ASSIGN XOR_ASSIGN ANDAND_ASSIGN OROR_ASSIGN
%token ADD SUB MUL DIV MOD
%token ADDADD SUBSUB
%token EQL NEQ LSS GTR LEQ GEQ
%token NOT ANDAND OROR
%token LSH RSH REVERSE AND OR XOR
%token DOT ARROW
%token SIZEOF CAST
%token LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE

%left NOELSE
%left ELSE
%left COMMA
%right ASSIGN MUL_ASSIGN ADD_ASSIGN DIV_ASSIGN SUB_ASSIGN AND_ASSIGN OR_ASSIGN LSH_ASSIGN RSH_ASSIGN MOD_ASSIGN XOR_ASSIGN
%left OROR
%left ANDAND
%left OR
%left XOR
%left AND
%left EQL NEQ
%left LSS GTR LEQ GEQ
%left LSH RSH
%left ADD SUB
%left MUL DIV MOD
%right POSITIVE NEGTIVE NOT REVERSE ADDRESS DEREF SIZEOF ADDADD SUBSUB CAST
%left MEMBER RBRACKET RPAREN

%type <int> CONST RESTRICT VOLATILE TypeQualifier TypeQualifierList
%type <int> STATIC EXTERN AUTO REGISTER StorageClassSpecifier
%type <int> STRUCT UNION RecordKeyword 
%type <int> DeclaratorPointer

%type <expr> INT_LITERAL FLOAT_LITERAL STRING_LITERAL
%type <expr> Expr AssignExpr ConstantExpr ConditionalExpr ArithmeticExpr CastExpr BitwiseExpr UnaryExpr PosfixExpr PrimaryExpr RelationExpr EqualityExpr LogicAndExpr LogicOrExpr
%type <expr> Initializer

%type <type> TypeSpecifier RecordSpecifier EnumSpecifier
%type <qual_type> TypeExpr QualifiedTypeSpecifier

%type <stmt> Stmt ExprStmt IterationStmt SelectionStmt CompoundStmt JumpStmt DeclStmt Label

%type <node> TranslationUnit 
%type <decl> ExtendDeclaration ObjectDeclaration TypeDefination FunctionDefination
%type <decl> ParameterDeclaration FieldDeclaration

%type <declarator> Declarator DirectDeclarator AbstractDeclarator DirectAbstractDeclarator InitDeclarator FieldDeclarator Enumerator

%type <idenifier> TypeIdentifier ObjectIdentifier NewIdentifier Identifier 

%type <list> DeclaratorList EnumeratorList
%type <initializer_list> InitializerList 
%type <declarator_list> InitDeclaratorList FieldDeclaratorList
%type <decl_list> ParameterList FieldDeclarationList
%type <stmt_list> StmtList
%type <expr_list> ArgumentList

%type <operator> AssignOperator UnaryOperator
%type <operator> ASSIGN MUL_ASSIGN ADD_ASSIGN DIV_ASSIGN SUB_ASSIGN AND_ASSIGN OR_ASSIGN LSH_ASSIGN RSH_ASSIGN MOD_ASSIGN XOR_ASSIGN
%type <operator> ADD SUB MUL DIV MOD
%type <operator> ADDADD SUBSUB
%type <operator> EQL NEQ LSS GTR LEQ GEQ
%type <operator> NOT ANDAND OROR
%type <operator> LSH RSH REVERSE AND OR XOR
%type <operator> DOT ARROW
%type <operator> SIZEOF




%%
%start TranslationUnit;
TranslationUnit
	: TranslationUnit ExtendDeclaration
	{
		$$ = $1;
		$$->AddDeclaration($2);
	}
	| ExtendDeclaration
	{
		$$ = context->TranslationUnit();
		$$->AddDeclaration($1);
	}
	;

ExtendDeclaration
	: FunctionDefination
	{
		$$ = $1;
	}
	| ObjectDeclaration
	{
		$$ = $1;
	}
	| TypeDefination
	{
		$$ = $1;
	}
	;

ObjectDeclaration
	: StorageClassSpecifier QualifiedTypeSpecifier InitDeclaratorList SEMICOLON
	{
		for (declarator : $3)
		{
			Declaration* decl;
			if (isa<FunctionDeclarator>(declarator))
				decl = new FunctionDeclaration($1,$2,declarator);
			else
				decl = new VariableDeclaration($1,$2,declarator);
			current_context.add(decl);
		}		
	}
	;

TypeDefination
	: TYPEDEF QualifiedTypeSpecifier DeclaratorList SEMICOLON
	{
		for (declarator : $3)
		{
			auto decl = new TypedefDeclaration($2,declarator);
			current_context.add(decl);
		}
	}
	;

DeclaratorList
	: DeclaratorList COMMA Declarator
	{
		$$ = $1;
		auto list = static_cast<std::list<Declarator*>*>($$);
		list->push_back($3);
	}
	| Declarator
	{
		auto list = new std::list<Declarator*>();
		lst->push_back($1);
		$$ = list;
	}
	;

Declarator
	: DeclaratorPointer DirectDeclarator
	{
		$$ = new PointerDeclarator($1,$2);
		$$->SetLocation(@$);
	}
	| DirectDeclarator
	{
		$$ = $1;
	}
	;

DirectDeclarator
	: Identifier
	{
		$$ = new IdentifierDeclarator($1);
		$$->SetLocation(@$);
	}
	| LPAREN Declarator RPAREN
	{
		$$ = new ParenDeclarator($2);
		$$->SetLocation(@$);
	}
	| DirectDeclarator LBRACKET ConstantExpr RBRACKET
	{
		$$ = new ArrayDeclarator($1,$3);
		$$->SetLocation(@$);
	}
	| DirectDeclarator LBRACKET RBRACKET
	{
		$$ = new ArratDeclarator($1,nullptr);
		$$->SetLocation(@$);
	}
	| DirectDeclarator LPAREN ParameterList RPAREN
	{
		$$ = new FunctionalDeclarator($1,$3);
		$$->SetLocation(@$);
	}
	| DirectDeclarator LPAREN RPAREN
	{
		$$ = new FunctionalDeclarator($1,nullptr);
		$$->SetLocation(@$);
	}
	;

InitDeclaratorList
	: InitDeclaratorList COMMA InitDeclarator
	{
		$$ = $1;
		$$->push_back($3);
	}
	| InitDeclarator
	{
		$$ = new std::list<Declarator*>();
		$$->push_back($1);
	}
	;

InitDeclarator
	: Declarator
	{
		$$ = $1;
	}
	| Declarator ASSIGN Initializer
	{
		$$ = new InitDeclarator($1,$3);
		$$->SetLocation(@$);
	}
	;

Initializer
	: ConstantExpr
	{
		$$ = new Initializer($1);
	}
	| LBRACE InitializerList RBRACE
	{
		$$ = new Initializer($2);
		$$->SetLocation(@$);
	}
	| LBRACE InitializerList COMMA RBRACE
	{
		$$ = new Initializer($2);
		$$->SetLocation(@$);
	}
	;

InitializerList
	: InitializerList COMMA Initializer
	{
		$$ = $1;
		$$->push_back($1);
	}
	| Initializer
	{
		$$ = new std::list<Initializer*>();
		$$->push_back($1);
	}
	;

AbstractDeclarator
	: DeclaratorPointer DirectAbstractDeclarator
	{
		$$ = new PointerDeclarator($1,$2);
		$$->SetLocation(@$);
	}
	| DirectAbstractDeclarator
	{
		$$ = $1;
	}
	;

DirectAbstractDeclarator
	: LPAREN AbstractDeclarator RPAREN
	{
		$$ = new ParenDeclarator($2);
		$$->SetLocation(@$);
	}
	| LBRACKET ConstantExpr RBRACKET 
	{
		$$ = new ArrayDeclarator(nullptr,$2);
		$$->SetLocation(@$);
	}
	| LBRACKET RBRACKET
	{
		$$ = new ArratDeclarator(nullptr,nullptr);
		$$->SetLocation(@$);
	}
	| DirectAbstractDeclarator LBRACKET ConstantExpr RBRACKET 
	{
		$$ = new ArrayDeclarator($1,$3);
		$$->SetLocation(@$);
	}
	| DirectAbstractDeclarator LBRACKET RBRACKET
	{
		$$ = new ArratDeclarator($1,nullptr);
		$$->SetLocation(@$);
	}
	| LPAREN ParameterList RPAREN
	{
		$$ = new FunctionalDeclarator(nullptr,$2);
		$$->SetLocation(@$);
	}
	| LPAREN RPAREN
	{
		$$ = new FunctionalDeclarator(nullptr,nullptr);
		$$->SetLocation(@$);
	}
	| DirectAbstractDeclarator LPAREN ParameterList RPAREN
	{
		$$ = new FunctionalDeclarator($1,$3);
		$$->SetLocation(@$);
	}
	| DirectAbstractDeclarator LPAREN RPAREN
	{
		$$ = new FunctionalDeclarator($1,nullptr);
		$$->SetLocation(@$);
	}
	;

ParameterList
	: ParameterList COMMA ParameterDeclaration
	{
		$$ = $1;
		$$->push_back($1);
	}
	| ParameterDeclaration
	{
		$$ = new std::list<Declaration*>();
		$$->push_back($1);
	}
	;

ParameterDeclaration
	: QualifiedTypeSpecifier Declarator
	{
		$$ = new ParameterDeclaration($1,$2);
	}
	| QualifiedTypeSpecifier AbstractDeclarator
	{
		$$ = new ParameterDeclaration($1,$2);
	}
	;

DeclaratorPointer
	: MUL TypeQualifierList
	{
		$$ = $2
	}
	| DeclaratorPointer MUL TypeQualifierList
	{
		$$ <<= 3;
		$$ |= $3;
	}
	;

QualifiedTypeSpecifier
	: TypeQualifierList TypeSpecifier
	{
		$$ = new QualType($2,$1);
	}
	;

TypeQualifierList
	: TypeQualifierList TypeQualifier
	{
		$$ |= $1;
	}
	| %empty
	{
		$$ = 0;
	}
	;

FunctionDefination
	: StorageClassSpecifier QualifiedTypeSpecifier Declarator CompoundStmt
	{
		$$ = new FunctionDefination($1,$2,$3,$4);
	}
	;

TypeSpecifier
	: VOID
	{
		$$ = type_context->Void();
	}
	| INT
	{
		$$ = type_context->Int();
	}
	| FLOAT
	{
		$$ = type_context->Float();
	}
	| RecordSpecifier
	{
		$$ = $1;
	}
	| EnumSpecifier
	{
		$$ = $1;
	}
	| TypeIdentifier
	{
		auto typeid = as<TypeDeclaration*>(current_context.Lookup($1));
		$$ = typeid->DeclType();
	}
	;

TypeQualifier
	: CONST
	{
		$$ = $1;
	}
	| VOLATILE 
	{
		$$ = $1;
	}
	| RESTRICT
	{
		$$ = $1;
	}
	;

StorageClassSpecifier
	: EXTERN
	{
		$$ = $1;
	}
	| STATIC
	{
		$$ = $1;
	}
	| AUTO
	{
		$$ = $1;
	}
	| REGISTER
	{
		$$ = $1;
	}
	;

RecordSpecifier
	: RecordKeyword Identifier LBRACE FieldDeclarationList RBRACE
	{
		$$ = type_context->NewRecordType($1,$2,$4);
	}
	| RecordKeyword LBRACE FieldDeclarationList RBRACE
	{
		$$ = type_context->NewRecordType($1,$3);
	}
	| RecordKeyword Identifier
	{
		$$ = type_context->NewRecordType($1,$2);
	}
	;

RecordKeyword
	: STRUCT
	{
		$$ = $1;
	}
	| UNION
	{
		$$ = $1;
	}
	;

EnumSpecifier
	: ENUM Identifier LBRACE EnumeratorList RBRACE
	{
		$$ = type_context->NewEnumType($2,$4);
	}
	| ENUM Identifier LBRACE EnumeratorList COMMA RBRACE
	{
		$$ = type_context->NewEnumType($2,$4);
	}
	| ENUM LBRACE EnumeratorList RBRACE
	{
		$$ = type_context->NewEnumType($3);
	}
	| ENUM LBRACE EnumeratorList COMMA RBRACE
	{
		$$ = type_context->NewEnumType($3);
	}
	| ENUM Identifier
	{
		$$ = type_context->NewEnumType($2);
	}
	;

EnumeratorList
	: EnumeratorList COMMA Enumerator
	{
		$$ = $1;
		$$->push_back($3);
	}
	| Enumerator
	{
		$$ = new std::list<Declarator*>();
		$$->push_back($1);
	}
	;

Enumerator
	: Identifier
	{
		$$ = new Enumerator($1);
		$$->SetLocation(@$);
	}
	| Identifier ASSIGN ConstantExpr
	{
		$$ = new Enumerator($1,$3);
		$$->SetLocation(@$);
	}
	;

FieldDeclarationList
	: FieldDeclaration
	{
		$$ = new std::list<Declaration*>();
		$$->push_back($1);
	}
	| FieldDeclarationList FieldDeclaration
	{
		$$ = $1;
		$$->push_back($2);
	}
	;

FieldDeclaration
	: QualifiedTypeSpecifier FieldDeclaratorList SEMICOLON
	{
		for (declarator : $2)
		{
			Declaration* decl = new FieldDeclaration($1,$2);
			current_context.add(decl);
		}		
	}
	;

FieldDeclaratorList
	: FieldDeclaratorList COMMA FieldDeclarator
	{
		$$ = $1;
		$$->push_back($3);
	}
	| FieldDeclarator
	{
		$$ = new std::list<Declarator*>();
		$$->push_back($1);
	}
	;

FieldDeclarator
	: Declarator COLON ConstantExpr
	{
		$$ = new FieldDeclarator($1,$3);
	}
	| COLON ConstantExpr
	{
		$$ = new FieldDeclarator(nullptr,$2);
	}
	| Declarator
	{
		$$ = new FieldDeclarator($1);
	}
	;

Identifier
	: NewIdentifier
	{
		$$ = $1;
	}
	| ObjectIdentifier
	{
		$$ = $1;
	}
	;

PrimaryExpr
	: INT_LITERAL
	{
		$$ = $1;
		$$->SetLocation(@$);
	}
	| STRING_LITERAL
	{
		$$ = $1;
		$$->SetLocation(@$);
	}
	| FLOAT_LITERAL
	{
		$$ = $1;
		$$->SetLocation(@$);
	}
	| Identifier
	{
		$$ = new DeclRefExpr($1);
		$$->SetLocation(@$);
	}
	| LPAREN Expr RPAREN
	{
		$$ = new ParenExpr($2);
		$$->SetLocation(@$);
	}
	;

PosfixExpr
	: PrimaryExpr
	{
		$$ = $1;
	}
	| PosfixExpr ADDADD
	{
		$$ = new UnaryExpr(OP_SUFIX_SELF_PLUS,$2);
		$$->SetLocation(@$);
	}
	| PosfixExpr SUBSUB
	{
		$$ = new UnaryExpr(OP_SUFIX_SELF_SUB,$2);
		$$->SetLocation(@$);
	}
	| PosfixExpr LBRACKET Expr RBRACKET
	{
		$$ = new IndexExpr($1,$3);
		$$->SetLocation(@$);
	}
	| PosfixExpr LPAREN ArgumentList RPAREN
	{
		$$ = new CallExpr($1,$3);
		$$->SetLocation(@$);
	}
	| PosfixExpr LPAREN RPAREN
	{
		$$ = new CallExpr($1,nullptr);
		$$->SetLocation(@$);
	}
	| PosfixExpr DOT Identifier
	{
		$$ = new DirectMemberExpr($1,$3);
		$$->SetLocation(@$);
	}
	| PosfixExpr ARROW Identifier
	{
		$$ = new IndirectMemberExpr($1,$3);
		$$->SetLocation(@$);
	}
	;

UnaryExpr
	: PosfixExpr
	{
		$$ = $1;
	}
	| ADDADD UnaryExpr
	{
		$$ = new UnaryExpr($1,$2)
		$$->SetLocation(@$);
	}
	| SUBSUB UnaryExpr
	{
		$$ = new UnaryExpr($1,$2)
		$$->SetLocation(@$);
	}
	| UnaryOperator CastExpr
	{
		$$ = new UnaryExpr($1,$2)
		$$->SetLocation(@$);
	}
	| SIZEOF UnaryExpr
	{
		$$ = new SizeofExpr($2);
		$$->SetLocation(@$);
	}
	| SIZEOF LPAREN TypeExpr RPAREN
	{
		$$ = new SizeofTypeExpr($3);
		$$->SetLocation(@$);
	}
	;

UnaryOperator
	: AND
	{
		$$ = $1;
	}
	| MUL
	{
		$$ = $1;
	}
	| ADD
	{
		$$ = $1;
	}
	| SUB
	{
		$$ = $1;
	}
	| REVERSE
	{
		$$ = $1;
	}
	| NOT
	{
		$$ = $1;
	}
	;

CastExpr
	: UnaryExpr
	{
		$$ = $1;
	}
	| CAST LSS TypeExpr GTR LPAREN Expr RPAREN
	{
		$$ = new ExplicitCastExpr($3,$6);
		$$ = SetLocation(@$);
	}
	| LPAREN TypeExpr RPAREN CastExpr
	{
		$$ = new ExplicitCastExpr($2,$4);
		$$ = SetLocation(@$);
	}
	;

ArithmeticExpr
	: CastExpr
	{ 
		$$ = $1;
	}
	| ArithmeticExpr ADD CastExpr
	{
		$$ = new ArithmeticExpr($2,$1,$3);
		$$->SetLocation(@$);
	}
	| ArithmeticExpr SUB CastExpr
	{
		$$ = new ArithmeticExpr($2,$1,$3);
		$$->SetLocation(@$);
	}
	| ArithmeticExpr MUL CastExpr
	{
		$$ = new ArithmeticExpr($2,$1,$3);
		$$->SetLocation(@$);
	}
	| ArithmeticExpr DIV CastExpr
	{
		$$ = new ArithmeticExpr($2,$1,$3);
		$$->SetLocation(@$);
	}
	| ArithmeticExpr MOD CastExpr
	{
		$$ = new ArithmeticExpr($2,$1,$3);
		$$->SetLocation(@$);
	}
	| ArithmeticExpr LSH CastExpr
	{
		$$ = new ArithmeticExpr($2,$1,$3);
		$$->SetLocation(@$);
	}
	| ArithmeticExpr RSH CastExpr
	{
		$$ = new ArithmeticExpr($2,$1,$3);
		$$->SetLocation(@$);
	}
	;

RelationExpr
	: ArithmeticExpr
	{
		$$ = $1;
	}
	| RelationExpr LEQ ArithmeticExpr
	{
		$$ = new ArithmeticExpr($2,$1,$3);
		$$->SetLocation(@$);
	}
	| RelationExpr GEQ ArithmeticExpr
	{
		$$ = new ArithmeticExpr($2,$1,$3);
		$$->SetLocation(@$);
	}
	| RelationExpr GTR ArithmeticExpr
	{
		$$ = new ArithmeticExpr($2,$1,$3);
		$$->SetLocation(@$);
	}
	| RelationExpr LSS ArithmeticExpr
	{
		$$ = new ArithmeticExpr($2,$1,$3);
		$$->SetLocation(@$);
	}
	;

EqualityExpr
	: RelationExpr
	{
		$$ = $1;
	}
	| EqualityExpr EQL RelationExpr
	{
		$$ = new ArithmeticExpr($2,$1,$3);
		$$->SetLocation(@$);
	}
	| EqualityExpr NEQ RelationExpr
	{
		$$ = new ArithmeticExpr($2,$1,$3);
		$$->SetLocation(@$);
	}
	;


BitwiseExpr
	: EqualityExpr
	{
		$$ = $1;
	}
	| BitwiseExpr AND EqualityExpr
	{
		$$ = new ArithmeticExpr($2,$1,$3);
		$$->SetLocation(@$);
	}
	| BitwiseExpr OR  EqualityExpr
	{
		$$ = new ArithmeticExpr($2,$1,$3);
		$$->SetLocation(@$);
	}
	| BitwiseExpr XOR EqualityExpr
	{
		$$ = new ArithmeticExpr($2,$1,$3);
		$$->SetLocation(@$);
	}
	;

LogicAndExpr
	: BitwiseExpr
	{
		$$ = $1;
	}
	| LogicAndExpr ANDAND BitwiseExpr
	{
		$$ = new LogicExpr($2,$1,$3);
		$$->SetLocation(@$);
	}
	;

LogicOrExpr
	: LogicAndExpr
	{
		$$ = $1;
	}
	| LogicOrExpr OROR LogicAndExpr
	{
		$$ = new LogicExpr($2,$1,$3);
		$$->SetLocation(@$);
	}
	;

ConditionalExpr
	: LogicOrExpr
	{
		$$ = $1;
	};

AssignExpr
	: BitwiseExpr
	{
		$$ = $1;
	}
	| UnaryExpr AssignOperator AssignExpr
	{
		//Unary operator here is because binary operator don't have L-value
		$$ = new AssignExpr($1,$3);
		$$ = SetLocation(@$);
	}
	;

AssignOperator
	: ASSIGN
	{ $$ = $1 }
	| MUL_ASSIGN
	{ $$ = $1 }
	| DIV_ASSIGN
	{ $$ = $1 }
	| MOD_ASSIGN
	{ $$ = $1 }
	| ADD_ASSIGN
	{ $$ = $1 }
	| SUB_ASSIGN
	{ $$ = $1 }
	| LSH_ASSIGN
	{ $$ = $1 }
	| RSH_ASSIGN
	{ $$ = $1 }
	| AND_ASSIGN
	{ $$ = $1 }
	| XOR_ASSIGN
	{ $$ = $1 }
	| OR_ASSIGN
	{ $$ = $1 }
	;

Expr
	: AssignExpr
	{
		$$ = $1;
	}
	| Expr COMMA AssignExpr
	{
		// Comma expression is evil!!!
		$$ = new CommaExpr($1,$3);
	}
	;

ConstantExpr
	: ConditionalExpr
	{
		// Constant expression don't support assign expression
		// nessary for initializers and compile time constant
		$$ = $1;
	}

ArgumentList
	: ArgumentList COMMA AssignExpr
	{
		$$ = $1;
		$$->push_back($3);
	}
	| AssignExpr
	{
		$$ = new std::list<Expr*>();
	}
	;

TypeExpr
	: QualifiedTypeSpecifier AbstractDeclarator
	{
		$$ = new TypeExpr($1,$2);
		$$->SetLocation(@$);
	}
	;

Stmt
	: ExprStmt
	{
		$$ = $1;
	}
	| DeclStmt
	{
		$$ = $1;
	}
	| IterationStmt
	{
		$$ = $1;
	}
	| SelectionStmt
	{
		$$ = $1;
	}
	| JumpStmt
	{
		$$ = $1;
	}
	| Label
	{
		$$ = $1;
	}
	| CompoundStmt
	{
		$$ = $1;
	}
	;

CompoundStmt
	: LBRACE RBRACE
	{
		$$ = new CompoundStmt(nullptr);
		$$->SetLocation(@$);
	}
	| LBRACE StmtList RBRACE
	{
		$$ = new CompoundStmt($2);		
		$$->SetLocation(@$);
	}
	;

StmtList
	: StmtList Stmt
	{
		$$ = $1;
		$$->push_back($2);
	}
	| Stmt
	{
		$$ = new std::list<Stmt*>();
		$$->push_back($1);
	}
	;

ExprStmt
	: Expr SEMICOLON
	{
		$$ = new ExprStmt($1);
		$$->SetLocation(@$);
	}
	| SEMICOLON
	{
		$$ = new NullStmt();
		$$->SetLocation(@$);
	}
	;

DeclStmt
	: ObjectDeclaration
	{
		$$ = new DeclStmt($1);
		$$->SetLocation(@$);
	}
	| TypeDefination
	{
		$$ = new DeclStmt($1);
		$$->SetLocation(@$);
	}
	;

IterationStmt
	: FOR LPAREN Stmt ExprStmt Expr RPAREN Stmt
	{
		$$ = new ForStmt($3,$4,$5,$7);
		$$->SetLocation(@$);
	}
	| FOR LPAREN Stmt ExprStmt RPAREN Stmt
	{
		$$ = new ForStmt($3,$4,nullptr,$6);
		$$->SetLocation(@$);
	}
	| WHILE LPAREN Expr RPAREN Stmt
	{
		$$ = new WhileStmt($3,$5);
		$$->SetLocation(@$);
	}
	| DO Stmt WHILE LPAREN Expr RPAREN SEMICOLON
	{
		$$ = new DoWhileStmt($5,$2);
		$$->SetLocation(@$);
	}
	;

SelectionStmt
	: IF LPAREN Expr RPAREN Stmt %prec NOELSE
	{
		$$ = new IfStmt($3,%5);
		$$->SetLocation(@$);
	}
	| IF LPAREN Expr RPAREN Stmt ELSE Stmt
	{
		$$ = new IfStmt($3,%5,$7);
		$$->SetLocation(@$);
	}
	| SWITCH LPAREN Expr RPAREN Stmt
	{
		$$ = new SwitchStmt($3);
		$$->SetLocation(@$);
		diag_context->NewDiagMsg(@$,UnsupportedFeature,"'switch statement'");
	}
	;

JumpStmt
	: CONTINUE SEMICOLON
	{
		$$ = new ContinueStmt();
		$$->SetLocation(@$);
	}
	| BREAK SEMICOLON
	{
		$$ = new BreakStmt();
		$$->SetLocation(@$);
	}
	| RETURN SEMICOLON
	{
		$$ = new ReturnStmt();
		$$->SetLocation(@$);
	}
	| RETURN Expr SEMICOLON
	{
		$$ = new ReturnStmt($2);
		$$->SetLocation(@$);
	}
	;

Label
	: CASE Expr COLON
	{
		$$ = new CaseLabel($2);
		$$->SetLocation(@$);
	}
	| DEFAULT COLON
	{
		$$ = new DefaultLabel();
		$$->SetLocation(@$);
	}
	;

%%

// We have to implement the error function
void C1::BisonParser::error(const C1::BisonParser::location_type &loc, const std::string &msg) {
	std::cerr << "Error: " << msg << std::endl;
}

// Now that we have the Parser declared, we can declare the Scanner and implement
// the yylex function
#include "cscanner.h"
static int yylex(C1::BisonParser::semantic_type * yylval,C1::BisonParser::location_type * yylloc, C1::FlexScanner &scanner) {
	return scanner.yylex(yylval,yylloc);
}

/*
int yyerror(char *message)
{
	printf("%s\n",message);
	return 0;
}

ASTree parse()
{
	ASTree tree = newAST();
	ast = tree;
	yyparse();
	return tree;
} */