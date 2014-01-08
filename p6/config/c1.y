%require "3.0"
%skeleton "lalr1.cc"
%defines 
%define api.namespace {C1}
%define api.token.constructor
%define api.value.type variant
%define parser_class_name {BisonParser}
%param { C1::AST::ASTContext& context }
%param { C1::FlexScanner& scanner }

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

%code {
	// Prototype for the yylex function
	//static int yylex(C1::BisonParser::semantic_type * yylval,C1::BisonParser::location_type * yylloc,C1::AST::ASTContext&, C1::FlexScanner &scanner);
	static C1::BisonParser::symbol_type yylex(C1::AST::ASTContext&, C1::FlexScanner &scanner);
	static int debug(const char* message)
	{
		return printf(message);
	}
	using namespace C1::AST;
	using namespace std;
}

%locations
%token 
	END "eof"
	WHILE "while"
	IF "if"
	ELSE "else"
	FOR "for"
	DO "do"
	SWITCH "switch"
	GOTO "goto"
	BREAK "break"
	RETURN "return"
	CONTINUE "continue"
	CASE "case"
	DEFAULT "defaul"
	TYPEDEF "typedef"
	COMMA ","
	SEMICOLON ";"
	COLON ":"
	QUESTION "?"
	;

%token INT VOID FLOAT DOUBLE SIGNED UNSIGNED LONG CHAR SHORT

%token <StorageClassSpecifierEnum>
	STATIC "static"
	EXTERN "extern"
	AUTO "auto"
	REGISTER "register"
	;
%token <TypeQualifierEnum>
	CONST "const" 
	VOLATILE "volatile"
	RESTRICT "restrict"
	;
%token <RecordKeywordEnum>
	STRUCT "struct" 
	UNION "union"
	ENUM "enum"
	;

%token <AST::Expr*> INT_LITERAL FLOAT_LITERAL STRING_LITERAL
%token <std::string> NewIdentifier ObjectIdentifier TypeIdentifier

%token <OperatorsEnum>
	ASSIGN "="
	MUL_ASSIGN "*="
	ADD_ASSIGN "+="
	DIV_ASSIGN "/="
	SUB_ASSIGN "-="
	AND_ASSIGN "&="
	OR_ASSIGN "|="
	LSH_ASSIGN "<<="
	RSH_ASSIGN ">>="
	MOD_ASSIGN "%="
	XOR_ASSIGN "^="
	ADD "+" 
	SUB "-"
	MUL "*"
	DIV "/"
	MOD "%"
	ADDADD "++"
	SUBSUB "--"
	EQL "=="
	NEQ "!="
	LSS "<"
	GTR ">"
	LEQ "<="
	GEQ "=>"
	NOT "!"
	ANDAND "&&"
	OROR "||"
	LSH "<<"
	RSH ">>"
	REVERSE "~"
	AND "&"
	OR "|"
	XOR "^"
	DOT "."
	ARROW "->"
	SIZEOF "sizeof"
	CAST "cast"
;

%token
	LPAREN "("
	RPAREN ")"
	LBRACKET "["
	RBRACKET "]"
	LBRACE "{"
	RBRACE "}"
;

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

%type <TypeQualifierEnum> TypeQualifier 
%type <int> TypeQualifierList
%type <StorageClassSpecifierEnum> StorageClassSpecifier
%type <RecordKeywordEnum> RecordKeyword 

%type <AST::Expr*> Expr AssignExpr ConstantExpr ConditionalExpr ArithmeticExpr CastExpr BitwiseExpr UnaryExpr PosfixExpr PrimaryExpr RelationExpr EqualityExpr LogicAndExpr LogicOrExpr
%type <AST::Initializer*> Initializer

%type <AST::TypeSpecifier*> TypeSpecifier RecordSpecifier EnumSpecifier
%type <AST::QualifiedTypeSpecifier*> QualifiedTypeSpecifier
%type <AST::TypeExpr*> TypeExpr
%type <AST::Stmt*> Stmt ExprStmt IterationStmt SelectionStmt CompoundStmt JumpStmt DeclStmt Label

%type <AST::Node*> TranslationUnit FunctionDefination ExtendDeclaration
%type <AST::FunctionHeader*> FunctionHeader
%type <AST::StructBody*> StructBody;
%type <AST::VarDeclStmt*> ObjectDeclaration
%type <AST::TypedefStmt*> TypeDefination
%type <AST::ParameterDeclaration*> ParameterDeclaration
%type <AST::FieldDeclStmt*> FieldDeclaration

%type <AST::Declarator*> Declarator DirectDeclarator AbstractDeclarator DirectAbstractDeclarator InitDeclarator FieldDeclarator
%type <AST::Enumerator*> Enumerator

%type <std::string> Identifier 

%type <std::list<int>*> DeclaratorPointer
%type <std::list<AST::Initializer*>*> InitializerList 
%type <std::list<AST::Declarator*>*> DeclaratorList InitDeclaratorList FieldDeclaratorList
%type <std::list<AST::Enumerator*>*> EnumeratorList
%type <std::list<AST::ParameterDeclaration*>*> ParameterList
%type <std::list<AST::FieldDeclStmt*>*> FieldDeclarationList
%type <std::list<AST::Stmt*>*> StmtList
%type <std::list<AST::Expr*>*> ArgumentList

%type <OperatorsEnum> UnaryOperator AssignOperator

%%
%start TranslationUnit;
TranslationUnit
	: TranslationUnit ExtendDeclaration
	{
		$$ = $1;
		//$$->AddDeclaration($2);
	}
	| ExtendDeclaration
	{
		$$ = context.CurrentTranslationUnit;
		//$$ = new TranslationUnit;
		//$$->AddDeclaration($1);
	}
	;

ExtendDeclaration
	: FunctionDefination
	{
		$$ = $1;
	}
	| DeclStmt
	{
		$$ = $1;
	}
	;

FunctionDefination
	: FunctionHeader CompoundStmt
	{
		$$ = new FunctionDefination($1,$2);
	}
	;

FunctionHeader
	: StorageClassSpecifier QualifiedTypeSpecifier Declarator
	{
		if (!isa<FunctionalDeclarator*>($3))
		{
			error(@3,"Expect a function declaration here.");
		}
		$$ = new FunctionHeader($1,$2,$3);
		// We do this here because we FunctionHeader : ValueDeclaration
		context.CurrentDeclContext->AddDeclaration($$);
	}
	;

ObjectDeclaration
	: StorageClassSpecifier QualifiedTypeSpecifier InitDeclaratorList
	{
		$$ = new VarDeclStmt($1,$2,$3);
		/*for (auto declarator : *$3)
		{
			Declaration* decl;
			if (isa<FunctionalDeclarator*>(declarator))
			{
				decl = new FunctionDeclaration($1,*$2,declarator);
			}
			else
			{
				decl = new VariableDeclaration($1,*$2,declarator);
			}
			context.CurrentDeclContext->AddDeclaration(decl);
		}*/
	}
	;

TypeDefination
	: TYPEDEF QualifiedTypeSpecifier DeclaratorList
	{
		$$ = new TypedefStmt($2,$3);
		/*for (auto declarator : *$3)
		{
			auto decl = new TypedefDeclaration(*$2,declarator);
			context.CurrentDeclContext->AddDeclaration(decl);
		}*/
	}
	;

DeclaratorList
	: DeclaratorList COMMA Declarator
	{
		$$ = $1;
		$$->push_back($3);
	}
	| Declarator
	{
		$$ = new std::list<Declarator*>();
		$$->push_back($1);
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
		$$ = new ArrayDeclarator($1,nullptr);
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
		$$ = new InitializerList($2);
		$$->SetLocation(@$);
	}
	| LBRACE InitializerList COMMA RBRACE
	{
		$$ = new InitializerList($2);
		$$->SetLocation(@$);
	}
	;

InitializerList
	: InitializerList COMMA Initializer
	{
		$$ = $1;
		$$->push_back($3);
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
		$$ = new ArrayDeclarator(nullptr,nullptr);
		$$->SetLocation(@$);
	}
	| DirectAbstractDeclarator LBRACKET ConstantExpr RBRACKET 
	{
		$$ = new ArrayDeclarator($1,$3);
		$$->SetLocation(@$);
	}
	| DirectAbstractDeclarator LBRACKET RBRACKET
	{
		$$ = new ArrayDeclarator($1,nullptr);
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
		$$->push_back($3);
	}
	| ParameterDeclaration
	{
		$$ = new std::list<ParameterDeclaration*>();
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
	| QualifiedTypeSpecifier
	{
		$$ = new ParameterDeclaration($1,nullptr);
	}
	;

DeclaratorPointer
	: MUL TypeQualifierList
	{
		$$ = new std::list<int>();
		$$->push_back($2);
	}
	| DeclaratorPointer MUL TypeQualifierList
	{
		$$->push_back($3);
	}
	;

QualifiedTypeSpecifier
	: TypeQualifierList TypeSpecifier
	{
		$$ = new QualifiedTypeSpecifier($1,$2);
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

TypeSpecifier
	: VOID
	{
		$$ = new PrimaryTypeSpecifier(context.TypeContext->Void());
	}
	| INT
	{
		$$ = new PrimaryTypeSpecifier(context.TypeContext->Int());
	}
	| FLOAT
	{
		$$ = new PrimaryTypeSpecifier(context.TypeContext->Float());
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
		$$ = new TypedefNameSpecifier($1);
		/*auto decl = dynamic_cast<TypeDeclaration*>(context.CurrentDeclContext->Lookup(*$1));
		assert(decl != nullptr);
		$$ = decl->DeclType();*/
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
	: RecordHeader StructBody
	{
		//current_context = *context.CurrentDeclContext;
		$$ = $1;
		auto specifier = dynamic_cast<StructSpecifier*>($$);
		specifier->SetBody($2);
		specifier->Decl()->AddBody($2);
	}
	| RecordHeader
	{
		$$ = $1;
	}
	| RecordKeyword StructBody
	{
		auto specifier = new StructSpecifier($2);
		auto decl = new StructDeclaration();
		current_context->AddDeclaration(decl);
		specifier -> SetDecl(decl);
	}
	;

RecordHeader
	: RecordKeyword Identifier
	{
		auto specifier = new StructSpecifier($2);
		$$ = specifier;

		auto decl = current_context->LocalLookup($2);
		if (!decl)
		{
			decl = new StructDeclaration($2);
			current_context->AddDeclaration(decl);
		} else {
			auto struct_decl = dynamic_cast<StructDeclaration*>(decl);
			if (!struct_decl)
			{
				error(@2,"redefination of symbol ""%s"".");
				//Recover : replace the defination with latest one.
				decl = new StructDeclaration($2);
				current_context->AddDeclaration(decl);
			} else {
				struct_decl->AddNode(specifier);
			}
		}
		specifier->SetDecl(decl);
	}
	;

StructBody
	: "{" FieldDeclarationList "}"
	{
		$$ = new StructBody($2);
		CurrentDeclContext = CurrentDeclContext->pop();
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
		$$ = new EnumSpecifier($2,$4);
	}
	| ENUM Identifier LBRACE EnumeratorList COMMA RBRACE
	{
		$$ = new EnumSpecifier($2,$4);
	}
	| ENUM LBRACE EnumeratorList RBRACE
	{
		$$ = new EnumSpecifier($3);
	}
	| ENUM LBRACE EnumeratorList COMMA RBRACE
	{
		$$ = new EnumSpecifier($3);
	}
	| ENUM Identifier
	{
		$$ = new EnumSpecifier($2);
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
		$$ = new std::list<Enumerator*>();
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
	: %empty
	{
		$$ = new std::list<FieldDeclStmt*>();
		CurrentDeclContext = CurrentDeclContext->push(new StructBody());
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
		auto compound_decl = new FieldDeclStmt($1,$2);
		compound_decl -> SetLocation(@$);
		for (auto declarator : $2)
		{
			auto decl = new FieldDeclaration($1,$2);
			decl -> SetNode(compound_decl);
			CurrentDeclContext.AddDeclaration(decl);
			// Handel redefination
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
		$$ = new PosfixExpr(OP_POSFIX_ADDADD,$1);
		$$->SetLocation(@$);
	}
	| PosfixExpr SUBSUB
	{
		$$ = new PosfixExpr(OP_POSFIX_SUBSUB,$1);
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
		$$ = new MemberExpr($1,$3,OP_DOT);
		$$->SetLocation(@$);
	}
	| PosfixExpr ARROW Identifier
	{
		$$ = new MemberExpr($1,$3,OP_ARROW);
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
		$$ = new UnaryExpr($1,$2);
		$$->SetLocation(@$);
	}
	| SUBSUB UnaryExpr
	{
		$$ = new UnaryExpr($1,$2);
		$$->SetLocation(@$);
	}
	| UnaryOperator CastExpr
	{
		$$ = new UnaryExpr($1,$2);
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
		$$ = OP_NEG;
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
		$$->SetLocation(@$);
	}
	| LPAREN TypeExpr RPAREN CastExpr
	{
		$$ = new ExplicitCastExpr($2,$4);
		$$->SetLocation(@$);
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
		//Unary op_enum here is because binary op_enum don't have L-value
		$$ = new AssignExpr($1,$3);
		$$ -> SetLocation(@$);
	}
	;

AssignOperator
	: ASSIGN
	{ $$ = $1; }
	| MUL_ASSIGN
	{ $$ = $1; }
	| DIV_ASSIGN
	{ $$ = $1; }
	| MOD_ASSIGN
	{ $$ = $1; }
	| ADD_ASSIGN
	{ $$ = $1; }
	| SUB_ASSIGN
	{ $$ = $1; }
	| LSH_ASSIGN
	{ $$ = $1; }
	| RSH_ASSIGN
	{ $$ = $1; }
	| AND_ASSIGN
	{ $$ = $1; }
	| XOR_ASSIGN
	{ $$ = $1; }
	| OR_ASSIGN
	{ $$ = $1; }
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
	| QualifiedTypeSpecifier
	{
		$$ = new TypeExpr($1,nullptr);
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
	: ObjectDeclaration SEMICOLON
	{
		$$ = $1;
		$$->SetLocation(@$);
	}
	| TypeDefination SEMICOLON
	{
		$$ = $1;
		$$->SetLocation(@$);
	}
	;

IterationStmt
	: FOR "(" Stmt ExprStmt Expr ")" Stmt
	{
		Stmt* post_action = new ExprStmt($5);
		ExprStmt* expr_stmt = dynamic_cast<ExprStmt*>($4);
		Expr* condition;
		if (expr_stmt != nullptr)
		{
			condition = expr_stmt->Expression();

			// delete this expr-stmt since we are only interested in the expr it self
			expr_stmt->SetExpression(nullptr);
			delete expr_stmt;
		} else // it's a null-statement
		{
			condition = nullptr;
		}

		$$ = new ForStmt($3,condition,post_action,$7);
		$$->SetLocation(@$);
	}
	| FOR "(" Stmt ExprStmt ")" Stmt
	{
		ExprStmt* expr_stmt = dynamic_cast<ExprStmt*>($4);
		Expr* condition;
		if (expr_stmt != nullptr)
		{
			condition = expr_stmt->Expression();

			// delete this expr-stmt since we are only interested in the expr it self
			expr_stmt->SetExpression(nullptr);
			delete expr_stmt;
		} else // it's a null-statement
		{
			condition = nullptr;
		}
		$$ = new ForStmt($3,condition,nullptr,$6);
		$$->SetLocation(@$);
	}
	| WHILE "(" Expr ")" Stmt
	{
		$$ = new WhileStmt($3,$5);
		$$->SetLocation(@$);
	}
	| DO Stmt WHILE "(" Expr ")" ";"
	{
		$$ = new DoWhileStmt($2,$5);
		$$->SetLocation(@$);
	}
	;

SelectionStmt
	: IF LPAREN Expr RPAREN Stmt %prec NOELSE
	{
		$$ = new IfStmt($3,$5);
		$$->SetLocation(@$);
	}
	| IF LPAREN Expr RPAREN Stmt ELSE Stmt
	{
		$$ = new IfStmt($3,$5,$7);
		$$->SetLocation(@$);
	}
	| SWITCH LPAREN Expr RPAREN Stmt
	{
		//$$ = new SwitchStmt($3);
		//$$->SetLocation(@$);
		$$ = new NullStmt();
		//diag_context->NewDiagMsg(@$,UnsupportedFeature,"'switch statement'");
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
		$$ = new NullStmt();
		$$->SetLocation(@$);
	}
	| DEFAULT COLON
	{
		$$ = new NullStmt();
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
inline C1::BisonParser::symbol_type yylex(C1::AST::ASTContext&, C1::FlexScanner &scanner)
{
	return scanner.NextToken();
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