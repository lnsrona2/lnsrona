#include "stdafx.h"
#include "include\ast.h"
//#include <cstdarg>
using namespace C1;
using namespace C1::AST;
using namespace std;



void BinaryExpr::Dump(ostream& os) const
{
	os << *LeftSubExpr() <<' ' << Operator() << ' ' << *RightSubExpr();
}

C1::AST::BinaryExpr::BinaryExpr(OperatorsEnum op, Expr* lhs, Expr* rhs)
: m_Operator(op)
{
	lhs->SetParent(this);
	rhs->SetParent(this);
	SetLocation(lhs->Location() + rhs->Location());
	m_SubExprs[0].reset(lhs); m_SubExprs[1].reset(rhs);
}

C1::AST::QualType C1::AST::BinaryExpr::ReturnType() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

C1::AST::ExprValueType C1::AST::BinaryExpr::ValueType() const
{
	return RValue;
}

bool C1::AST::BinaryExpr::HasSideEffect() const
{
	return LeftSubExpr()->HasSideEffect() && RightSubExpr()->HasSideEffect();
}

Expr::ComposedValue C1::AST::BinaryExpr::Evaluate() const
{
	ComposedValue val;
	val.Int = 0;
	return val;
}

bool C1::AST::BinaryExpr::Evaluatable() const
{
	return LeftSubExpr()->Evaluatable() && RightSubExpr()->Evaluatable();
}

void C1::AST::PosfixExpr::Dump(std::ostream& os) const
{
	os << *SubExpr() << Operator();
}

C1::AST::PosfixExpr::PosfixExpr(OperatorsEnum op, Expr* Base)
: UnaryExpr(op,Base)
{
	assert(op == OP_POSFIX_ADDADD || op == OP_POSFIX_SUBSUB);
}

PosfixExpr* C1::AST::PosfixExpr::MakePosfixExpr(OperatorsEnum op, Expr* sub_expr)
{
	auto expr = new PosfixExpr(op, sub_expr);
	auto sub_type = sub_expr->ReturnType();
	if (!sub_type->IsIntegerType() && !sub_type->IsPointerType())
	{
		error(expr, "can not find overload of postfix operator (++/--) which takes arguments type (<sub-expr-type>)");
	}
	if (sub_type.IsConst())
	{
		error(expr, "operand of postfix operator (++/--) can not be an const type");
	}
	if (sub_expr->ValueType())
	{
		error(expr, "operand of postfix operator (++/--) must be a L-Value expression");
	}
	return expr;
}

C1::AST::CallExpr::CallExpr(Expr* func, std::list<Expr*>* args)
: m_FuncExpr(func), m_Arguments(std::move(*args))
{

}

void C1::AST::CallExpr::Dump(std::ostream& os) const
{
	os << *m_FuncExpr;
	if (m_Arguments.empty()) {
		os << "()";
		return;
	}

	os << '(';
	for (auto expr : m_Arguments)
	{
		os << *expr << ", ";
	}
	os << "\b\b)";
}

C1::AST::CallExpr::~CallExpr()
{
	for (auto expr : m_Arguments)
	{
		if (expr) delete expr;
	}
}

QualType C1::AST::CallExpr::ReturnType() const
{
	return m_FuncExpr->ReturnType().As<FunctionType>()->ReturnType();
}

C1::AST::ExprValueType C1::AST::CallExpr::ValueType() const
{
	return RValue;
}

bool C1::AST::CallExpr::HasSideEffect() const
{
	return true;
}

bool C1::AST::CallExpr::Evaluatable() const
{
	return false;
}

CallExpr* C1::AST::CallExpr::MakeCallExpr(Expr* callee, std::list<Expr*>* args)
{
	auto expr = new CallExpr(callee, args);

	return expr;
}

C1::AST::DeclRefExpr::DeclRefExpr(DeclContext* lookup_context, TypeContext* type_context, const std::string &name)
: m_Name(name), m_RefContext(lookup_context), m_TypeContext(type_context)
{
	ResoloveReference();
}

void C1::AST::DeclRefExpr::Dump(std::ostream& os) const
{
	if (m_RefDecl)
		os << m_RefDecl->Name();
	else
		os << "<err-ref>(" << m_Name <<")";
}

ValueDeclaration* C1::AST::DeclRefExpr::ResoloveReference()
{
	auto decl = m_RefContext->lookup(m_Name);
	m_RefDecl = dynamic_cast<ValueDeclaration*>(decl);
	if (!m_RefDecl)
	{
		error(this, "Can not find the declaration of name ""<name>"", assuming int");
		m_RefDecl = new VariableDeclaration(QualType(m_TypeContext->Int()),m_Name);
		m_RefContext->add(m_RefDecl);
	}
	return m_RefDecl;
}

void C1::AST::DeclRefExpr::SetRefDeclContext(DeclContext* reference_context) 
{
	m_RefContext = reference_context;
	ResoloveReference();
}

void C1::AST::DeclRefExpr::SetIdentifier(const std::string val)
{
	m_Name = val;
	ResoloveReference();
}

C1::AST::QualType C1::AST::DeclRefExpr::ReturnType() const
{
	return RefDecl()->DeclType();
}

C1::AST::ExprValueType C1::AST::DeclRefExpr::ValueType() const
{
	if (!ReturnType()->IsArrayType())
		return LValue;
	else
		return RValue;
}

bool C1::AST::DeclRefExpr::HasSideEffect() const
{
	return false;
}

bool C1::AST::DeclRefExpr::Evaluatable() const
{
	auto decl = dynamic_cast<const VariableDeclaration*>(RefDecl());
	return decl && decl->InitializeExpr()->Evaluatable();
}

DeclRefExpr* C1::AST::DeclRefExpr::MakeDeclRefExpr(DeclContext* lookup_context, TypeContext* type_context, const std::string &name)
{
	return new DeclRefExpr(lookup_context,type_context, name);
}

C1::AST::UnaryExpr::UnaryExpr(OperatorsEnum op, Expr* sub_expr)
: m_Operator(op), m_SubExpr(sub_expr)
{
	sub_expr->SetParent(this);
	this->SetLocation(sub_expr->Location());
}

void C1::AST::UnaryExpr::Dump(std::ostream& os) const
{
	os << Operator() << *SubExpr();
}

// Basically , most unary operator returns the same type of its base
C1::AST::QualType C1::AST::UnaryExpr::ReturnType() const
{
	return m_SubExpr->ReturnType();
}

C1::AST::ExprValueType C1::AST::UnaryExpr::ValueType() const
{
	return RValue;
}

bool C1::AST::UnaryExpr::HasSideEffect() const
{
	return false;
}

bool C1::AST::UnaryExpr::Evaluatable() const
{
	return m_SubExpr->Evaluatable();
}

UnaryExpr* C1::AST::UnaryExpr::MakeUnaryExpr(OperatorsEnum op, Expr* sub_expr)
{
	auto expr = new UnaryExpr(op, sub_expr);
	auto sub_type = sub_expr->ReturnType();
	switch (op)
	{
	case C1::OP_NEG:
	case C1::OP_POS:
		if (!sub_type->IsArithmeticType())
		{
			error(expr, "can not find overload of unary operator (+/-) which takes arguments type (<sub-expr-type>)");
		}
		break;
	case C1::OP_ADDADD:
	case C1::OP_SUBSUB:
	case C1::OP_POSFIX_ADDADD:
	case C1::OP_POSFIX_SUBSUB:
		if (!sub_type->IsIntegerType() && !sub_type->IsPointerType())
		{
			error(expr, "can not find overload of operator (++/--) which takes arguments type (<sub-expr-type>)");
		}
		if (sub_type.IsConst())
		{
			error(expr, "operand of operator (++/--) can not be an const type");
		}
		if (sub_expr->ValueType())
		{
			error(expr, "operand of operator (++/--) must be a L-Value expression");
		}
		break;
	case C1::OP_NOT:
	case C1::OP_REVERSE:
		if (!sub_type->IsInitializerListType())
		{
			error(expr, "can not find overload of unary operator (!/~) which takes arguments type (<sub-expr-type>)");
		}
		break;
	default:
		break;
	}
	return expr;
}

C1::AST::AssignExpr::AssignExpr(Expr* lhs, Expr* rhs)
: BinaryExpr(OP_ASSIGN,lhs,rhs)
{
}

C1::AST::AssignExpr::AssignExpr(OperatorsEnum op, Expr* lhs, Expr* rhs)
: BinaryExpr(op,lhs,rhs)
{
}

const OperatorsEnum C1::AST::AssignExpr::PrefixedOperator() const
{
	return OperatorsEnum(m_Operator - OP_ADD_ASSIGN + OP_ADD);
}

C1::AST::QualType C1::AST::AssignExpr::ReturnType() const
{
	return LeftSubExpr()->ReturnType();
}

C1::AST::ExprValueType C1::AST::AssignExpr::ValueType() const
{
	return LValue;
	//int a;
	//(a = 3) = 5;
}

bool C1::AST::AssignExpr::HasSideEffect() const
{
	return true;
}

bool C1::AST::AssignExpr::Evaluatable() const
{
	if (IsComposed())
		return false;
	else
		return RightSubExpr()->Evaluatable();
}

Expr::ComposedValue C1::AST::AssignExpr::Evaluate() const
{
	if (IsComposed() || !RightSubExpr()->Evaluatable())
	{
		ComposedValue val;
		val.Int = 0;
		return val;
	}else
	{
		return RightSubExpr()->Evaluate();
	}
}

AssignExpr* C1::AST::AssignExpr::MakeAssignExpr(OperatorsEnum op, Expr* lhs, Expr* rhs)
{
	auto ltype = lhs->ReturnType();
	auto rtype = rhs->ReturnType();
	ltype.AddConst();
	rhs = CastExpr::MakeImplictitCastExpr(ltype, rhs);
	ltype = lhs->ReturnType();
	if (op != OP_ASSIGN)
		rhs = ArithmeticExpr::MakeArithmeticExpr(GetComposedOperator(op), lhs, rhs);
	auto expr = new AssignExpr(op, lhs, rhs);
	if (lhs->ValueType() != LValue)
	{
		error(expr, "Assigned expression must have L-Value");
	}
	if (ltype.IsConst())
	{
		error(expr, "Assigned expression must have an non-const qualified type.");
	}
	return expr;
}

C1::OperatorsEnum C1::AST::AssignExpr::GetComposedOperator(OperatorsEnum op)
{
	return OperatorsEnum(op - OP_ADD_ASSIGN + OP_ADD);
}

void C1::AST::AssignExpr::Dump(std::ostream& ostr) const
{
	if (m_Operator == OP_ASSIGN)
		BinaryExpr::Dump(ostr);
	else
		ostr << *LeftSubExpr() << Operator() << *dynamic_cast<const BinaryExpr*>(RightSubExpr())->RightSubExpr();
}

C1::AST::IndexExpr::IndexExpr(Expr* host_expr, Expr* index_expr)
: BinaryExpr(OP_INDEX,host_expr,index_expr)
{
}

void C1::AST::IndexExpr::Dump(std::ostream& os) const
{
	os << *Host() << '[' << *Index() << ']';
}

C1::AST::QualType C1::AST::IndexExpr::ReturnType() const
{
	return LeftSubExpr()->ReturnType().As<DereferencableType>()->Base();
}

C1::AST::ExprValueType C1::AST::IndexExpr::ValueType() const
{
	return LValue;
}

Expr::ComposedValue C1::AST::IndexExpr::Evaluate() const
{
	Expr::ComposedValue val;
	val.Int = 0;
	return val;
}

bool C1::AST::IndexExpr::Evaluatable() const
{
	return false;
}

IndexExpr* C1::AST::IndexExpr::MakeIndexExpr(Expr* lhs, Expr* rhs)
{
	auto ltype = lhs->ReturnType();
	auto rtype = rhs->ReturnType();
	auto expr = new IndexExpr(lhs, rhs);
	if (!ltype->IsAddressType())
	{
		error(expr, "Left operand of Index operator must have a dereference able type");
	}
	if (!rtype->IsIntegerType())
	{
		error(expr, "Right operand of Index operator must have a integer type");
	}
	return expr;
}

C1::AST::ArithmeticExpr::ArithmeticExpr(OperatorsEnum op, Expr* lhs, Expr* rhs)
: BinaryExpr(op,lhs,rhs)
{
	//IsArithmeticOperator(op);
}

ArithmeticExpr* C1::AST::ArithmeticExpr::MakeArithmeticExpr(OperatorsEnum op, Expr* lhs, Expr* rhs)
{
	auto ltype = lhs->ReturnType();
	auto rtype = rhs->ReturnType();
	// Well , you always need to build it.
	
	if (!ltype->IsArithmeticType() || !rtype->IsArithmeticType())
	{
		auto expr = new ArithmeticExpr(op, lhs, rhs);
		// C-style pointer arithmetic
		if (op == OP_ADD)
		{
			// int + pointer || pointer + int
			if (ltype->IsAddressType() && rtype->IsIntegerType() || ltype->IsIntegerType() && rtype->IsAddressType())
				return expr;
		} else if (op == OP_SUB)
		{
			// pointer - int || pointer(base) - pointer(base)
			if (ltype->IsAddressType() && rtype->IsIntegerType())
				return expr;
			else if (ltype->IsAddressType() && rtype->IsAddressType())
			if (ltype.As<DereferencableType>()->Base() == rtype.As<DereferencableType>()->Base())
			{
				return expr;
			}
		} else if (op == OP_EQL || op == OP_NEQ)
		{
			// Only support address type equal/not-equal comparison
			if (ltype->IsAddressType() && rtype->IsAddressType() && ltype.As<DereferencableType>()->Base() == rtype.As<DereferencableType>()->Base())
				return expr;
		}
		error(expr, "no overload for operator <op> with argument type (<lhs-type> , <rhs-type>)");
		return expr;
	}
	else {
		switch (op)
		{
		// Integer & Float Type Operators
		case C1::OP_ADD:
		case C1::OP_SUB:
		case C1::OP_MUL:
		case C1::OP_DIV:

		case C1::OP_EQL:
		case C1::OP_NEQ:
		case C1::OP_LSS:
		case C1::OP_GEQ:
		case C1::OP_GTR:
		case C1::OP_LEQ:
			{
				auto generic_type = get_most_generic_arithmetic_type(ltype, rtype);
				lhs = CastExpr::MakeImplictitCastExpr(generic_type, lhs);
				rhs = CastExpr::MakeImplictitCastExpr(generic_type, rhs);
				return new ArithmeticExpr(op, lhs, rhs);
			}
			break;
		// Integer Type Operators
		case C1::OP_MOD:
		case C1::OP_AND:
		case C1::OP_OR:
		case C1::OP_XOR:
		case C1::OP_LSH:
		case C1::OP_RSH:
			{
				auto expr = new ArithmeticExpr(op, lhs, rhs);
				if (!ltype->IsIntegerType() || !rtype->IsIntegerType())
				{
					error(expr, "no overload for operator <op> with argument type (<lhs-type> , <rhs-type>)");
					return expr;
				}
				return expr;
			}
			break;
		default:
			return new ArithmeticExpr(op, lhs, rhs);
			break;
		}
	}
}

C1::AST::QualType C1::AST::ArithmeticExpr::ReturnType() const
{
	auto lhs = LeftSubExpr();
	auto rhs = RightSubExpr();
	auto ltype = lhs->ReturnType();
	auto rtype = rhs->ReturnType();
	auto op = m_Operator;
	if (ltype == rtype)
		return ltype;
	if (ltype->IsErrorType()) return ltype;
	if (rtype->IsErrorType()) return rtype;

	if (!ltype->IsArithmeticType() || !rtype->IsArithmeticType())
	{
		// C-style pointer arithmetic
		if (op == OP_ADD)
		{
			// int + pointer || pointer + int
			if (ltype->IsAddressType() && rtype->IsIntegerType())
				return MakeConst(ltype.As<DereferencableType>()->GetPointerType());
			else if (ltype->IsIntegerType() && rtype->IsAddressType())
				return MakeConst(rtype.As<DereferencableType>()->GetPointerType());
		}
		else if (op == OP_SUB)
		{
			// pointer - int || pointer(base) - pointer(base)
			if (ltype->IsAddressType() && rtype->IsIntegerType())
				return MakeConst(ltype.As<DereferencableType>()->GetPointerType());
			else if (ltype->IsAddressType() && rtype->IsAddressType() && (ltype.As<DereferencableType>()->Base() == rtype.As<DereferencableType>()->Base()))
				return ltype->AffiliatedContext()->Int();
			else // Int - Pointer or Different pointer minus
				return ltype->AffiliatedContext()->Error();
		}
		else if (op == OP_EQL || op == OP_NEQ)
		{
			// Only support address type equal/not-equal comparison
			if (ltype->IsAddressType() && rtype->IsAddressType() && ltype.As<DereferencableType>()->Base() == rtype.As<DereferencableType>()->Base())
				return ltype->AffiliatedContext()->Bool();
			else
				return ltype->AffiliatedContext()->Bool();
				// It's part of the recovery
				//return ltype->AffiliatedContext()->Error();
		}
		//error(expr, "no overload for operator <op> with argument type (<lhs-type> , <rhs-type>)");
	}
	return ltype->AffiliatedContext()->Error();
}

C1::AST::LogicExpr::LogicExpr(OperatorsEnum op, Expr* lhs, Expr* rhs)
: BinaryExpr(op,lhs,rhs)
{
}

LogicExpr* C1::AST::LogicExpr::MakeLogicExpr(OperatorsEnum op, Expr* lhs, Expr* rhs)
{
	auto ltype = lhs->ReturnType();
	auto rtype = rhs->ReturnType();
	auto expr = new LogicExpr(op, lhs, rhs);
	if (!ltype->IsIntegerType() || !rtype->IsIntegerType())
	{
		error(expr, "no overload for operator <logic-op> with argument type (<lhs-type> , <rhs-type>)");
	}
	return expr;
}

C1::AST::QualType C1::AST::LogicExpr::ReturnType() const
{
	return LeftSubExpr()->ReturnType()->AffiliatedContext()->Bool();
}

C1::AST::ConditionalExpr::ConditionalExpr(Expr* condition, Expr* true_expr, Expr* false_expr)
: m_Condition(condition), m_TrueExpr(true_expr), m_FalseExpr(false_expr)
{}

void C1::AST::ConditionalExpr::Dump(std::ostream& os) const
{
	os << *m_Condition << " ? " << *m_TrueExpr << " : " << *m_FalseExpr;
	//bool boo = true;
	//auto c = boo ? "ac" : 1.5f;
}

C1::AST::QualType C1::AST::ConditionalExpr::ReturnType() const
{
	if (m_TrueExpr->ReturnType() == m_FalseExpr->ReturnType())
	{
		return m_TrueExpr->ReturnType();
	}
	else return QualType(nullptr); // Should throw here
}

C1::AST::ExprValueType C1::AST::ConditionalExpr::ValueType() const
{
	return RValue;
}

bool C1::AST::ConditionalExpr::HasSideEffect() const
{
	return m_Condition->HasSideEffect() || m_TrueExpr->HasSideEffect() || m_FalseExpr->HasSideEffect();
}

ComposedValue C1::AST::ConditionalExpr::Evaluate() const
{
	assert(m_Condition->Evaluatable());
	auto cond = m_Condition->Evaluate();
	if (cond.Bool)
		return m_TrueExpr->Evaluate();
	else
		return m_FalseExpr->Evaluate();
}

bool C1::AST::ConditionalExpr::Evaluatable() const
{
	if (!m_Condition->Evaluatable()) return false;
	auto cond = m_Condition->Evaluate();
	if (cond.Bool)
		return m_TrueExpr->Evaluatable();
	else
		return m_FalseExpr->Evaluatable();
}


C1::AST::TypeExpr::TypeExpr(QualifiedTypeSpecifier* qual_type_specifier, Declarator* declarator)
: m_Specifier(qual_type_specifier), m_Declarator(declarator)
{
	m_Specifier->SetParent(this);

	m_Declarator->SetParent(this);
	SetLocation(m_Specifier->Location() + m_Declarator->Location());
	m_Type = m_Declarator->DecorateType(m_Specifier->RepresentType());
}

void C1::AST::TypeExpr::Dump(std::ostream& os) const
{
	os << *m_Specifier << *m_Declarator;
}

C1::AST::MemberExpr::MemberExpr(Expr* host, const std::string &member_name, OperatorsEnum op)
: m_HostExpr(host), m_MemberName(member_name), m_Operator(op)
{
	host->SetParent(this);
	QualType hostType = host->ReturnType();
	if (op == OP_ARROW)
	{
		assert(hostType->IsPointerType());
		hostType = hostType.As<PointerType>()->Base();
	}
	if (!hostType->IsStructType())
	{
		error(this,"Left operand of member operator must have record types.");
	}
	else
	{
		auto def = hostType.As<StructType>()->Members();
		auto decl = def->lookup_local(m_MemberName);
		m_MemberDeclaration = dynamic_cast<FieldDeclaration*>(decl);
		if (!decl)
		{
			//char[100] buf;
			//sprintf(buf, "%s is not a member of type %s", m_MemberName, host->ReturnType()->ToString());
			error(this,"Right operand is not an member of left operand.");
		}
	}
}

bool C1::AST::MemberExpr::IsDirect() const
{
	return m_Operator == OP_DOT;
}

void C1::AST::MemberExpr::Dump(std::ostream& os) const
{
	os << *Host() << m_Operator << m_MemberName;
}

C1::AST::QualType C1::AST::MemberExpr::ReturnType() const
{
	return m_MemberDeclaration->DeclType();
}

C1::AST::ExprValueType C1::AST::MemberExpr::ValueType() const
{
	return LValue;
}

bool C1::AST::MemberExpr::HasSideEffect() const
{
	return false;
}

bool C1::AST::MemberExpr::Evaluatable() const
{
	return false;
}

MemberExpr* C1::AST::MemberExpr::MakeMemberExpr(Expr* host, const std::string &member_name, OperatorsEnum op)
{
	return new MemberExpr(host, member_name, op);
}

C1::AST::ExplicitCastExpr::ExplicitCastExpr(TypeExpr* target_type_expr, Expr* source_expr)
: m_TargetTypeExpr(target_type_expr), CastExpr(source_expr, target_type_expr->DeclType())
{
}

void C1::AST::ExplicitCastExpr::Dump(std::ostream& os) const
{
	os << "cast<" << *m_TargetTypeExpr << ">(" << *m_SourceExpr << ")";
}

C1::AST::StringLiteral::StringLiteral(TypeContext* type_context, const char* raw_str)
: ConstantLiteral(type_context,raw_str)
{
	m_DecodedString = DecodeString(m_RawLiteral);
	m_Value.String = m_DecodedString.data();
}

C1::AST::QualType C1::AST::StringLiteral::ReturnType() const
{
	return MakeConst(m_TypeContext->String());
}

std::string C1::AST::StringLiteral::DecodeString(const std::string & raw_str)
{
	return raw_str;
}

std::string C1::AST::StringLiteral::EncodeString(const std::string & raw_str)
{
	return raw_str;
}

void C1::AST::InitializerList::Dump(std::ostream& os) const
{
	if (empty())
	{
		os << "{}";
		return;
	}
	os << "{";
	for (auto initializer : *this)
	{
		os << *initializer << ", ";
	}
	os << "\b\b}";
}

C1::AST::QualType C1::AST::InitializerList::ReturnType() const
{
	if (m_ReturnType.get())
		return m_ReturnType;
	InitializerListType* type = m_TypeContext->NewInitializerListType();
	const_cast<QualType&>(m_ReturnType) = QualType(type);
	for (auto expr : *this)
	{
		type->ElementTypes().push_back(expr->ReturnType());
	}
	return m_ReturnType;
}

C1::AST::ExprValueType C1::AST::InitializerList::ValueType() const
{
	return RValue;
}


bool C1::AST::InitializerList::HasSideEffect() const
{
	for (auto initailizer : *this)
	{
		if (initailizer->HasSideEffect())
			return true;
	}
	return false;
}

C1::AST::Expr::ComposedValue C1::AST::InitializerList::Evaluate() const
{
	Expr::ComposedValue val;
	val.ValueList = new std::vector<ComposedValue>(size());
	auto &valueList = *val.ValueList;
	for (auto expr : *this)
	{
		assert(expr->Evaluatable());
		valueList.emplace_back(expr->Evaluate());
	}
	return val;
}

bool C1::AST::InitializerList::Evaluatable() const
{
	for (auto initailizer : *this)
	{
		if (initailizer->Evaluatable())
			return true;
	}
	return false;
}

C1::AST::InitializerList::InitializerList(TypeContext *context)
: m_TypeContext(context)
{
}

C1::AST::CommaExpr::CommaExpr(Expr* lhs, Expr* rhs)
: BinaryExpr(OP_COMMA,lhs,rhs)
{

}

C1::AST::CompoundStmt::CompoundStmt()
{

}

void C1::AST::CompoundStmt::Dump(std::ostream& os) const
{
	os << "{" << endl;
	for (auto stmt : Children())
	{
		os << *stmt;
	}
	os << "}" << endl;
}

C1::AST::WhileStmt::WhileStmt(Expr* condition, Stmt* action)
: IterationStmt(action, condition)
{
}

void C1::AST::WhileStmt::Dump(std::ostream& os) const
{
	os << "while (" << *m_Condition << ")" << endl;
	os << *m_Action;
}

C1::AST::ForStmt::ForStmt(Stmt* initialize_stmt, Expr* condition, Expr* post_action, Stmt* action)
: IterationStmt(action, condition), m_Initializer(initialize_stmt), m_PostAction(new ExprStmt(post_action))
{
	m_Initializer->SetParent(this);
	m_PostAction->SetParent(this);
}

void C1::AST::ForStmt::Dump(std::ostream& os) const
{
	os << "for (" << *m_Initializer << *m_Condition << ";" << *m_PostAction->Expression() << ")" << endl;
	os << *m_Action;
}

C1::AST::DoWhileStmt::DoWhileStmt(Stmt* action, Expr* condition)
: IterationStmt(action,condition)
{
}

void C1::AST::DoWhileStmt::Dump(std::ostream& os) const
{
	os << "do" << endl;
	os << *m_Action;
	os << "while (" << *m_Condition << ")" << endl;
}

C1::AST::IfStmt::IfStmt(Expr* condition, Stmt* then_action, Stmt* else_action /*= nullptr*/)
: m_Condition(condition), m_ThenAction(then_action), m_ElseAction(else_action)
{
	m_Condition->SetParent(this);
	m_ThenAction->SetParent(this);
	if (m_ElseAction)
		m_ElseAction->SetParent(this);
}

void C1::AST::IfStmt::Dump(std::ostream& os) const
{
	os << "if (" << *m_Condition << ")" << endl << *m_ThenAction;
	if (m_ElseAction)
		os << "else" << endl << *m_ElseAction;
}

void C1::AST::PrimaryTypeSpecifier::Dump(std::ostream& os) const
{
	os << RepresentType()->ToString() << " ";
}

void C1::AST::StructBody::Dump(std::ostream& os) const
{
	os << "{" << endl;
	for (auto child : Children())
	{
		os << *child;
	}
	os << "}";
}

C1::AST::StructBody::StructBody()
{

}

C1::AST::StructDeclaration::StructDeclaration(const std::string& name, StructBody* definition)
: m_Name(name), m_Definition(definition)
{
	definition->SetParent(this);
}

C1::AST::StructDeclaration::StructDeclaration(const std::string& name)
: m_Name(name), m_Definition(nullptr)
{
}

C1::AST::StructDeclaration::StructDeclaration(StructBody* definition)
: m_Name("%anonymous"), m_Definition(definition)
{
	definition->SetParent(this);
}

C1::AST::StructDeclaration::StructDeclaration()
{
	SetKind(DECL_STRUCT);
}

StructBody* C1::AST::StructDeclaration::LatestDefinition()
{
	auto decl = this;
	while (decl->Definition() == nullptr)
	{
		decl = decl->prev();
	}
	return decl->Definition();
}

const StructBody* C1::AST::StructDeclaration::LatestDefinition() const
{
	auto decl = this;
	while (decl->Definition() == nullptr)
	{
		decl = decl->prev();
	}
	return decl->Definition();
}

void C1::AST::StructDeclaration::Dump(std::ostream& os) const
{
	os << "struct ";
	if (!IsAnonymous()) os << m_Name << " ";
	if (Definition()) os << *Definition() << " ";
}

DeclContext::InsertionResult C1::AST::StructDeclaration::AddToContext(DeclContext& context)
{
	using InsertionResult = DeclContext::InsertionResult;
	using NameCollisonPolicy = DeclContext::NameCollisonPolicy;
	const auto name_policy = NameCollisonPolicy::CompatibleRedefinable;

	InsertionResult result = InsertionResult::Success;
	auto decl = context.lookup_local<StructDeclaration>(this->Name());
	if (decl)
	{
		if (Declaration::CheckCompatible(decl, this))
		{
			result = InsertionResult::Success_CompatibleRedefinition;
			decl->add_last(this);
			this->SetDeclType(decl->DeclType());
		}
		else
		{
			result = InsertionResult::Success_IncompatibleRedefinition;
			auto prev_decl = dynamic_cast<StructDeclaration*>(decl);
			if (prev_decl) prev_decl->add_last(this);
		}
	}
	else
	{

	}

	context.force_add(this);
	return result;
}

void C1::AST::StructDeclaration::SetDefinition(StructBody* val)
{
	m_Definition.reset(val);
	val->SetParent(this);
	if (DeclType())
	{
		dynamic_cast<StructType*>(DeclType())->SetDefinition(val);
	}
}

void C1::AST::TypedefNameSpecifier::Dump(std::ostream& os) const
{
	auto decl_type = dynamic_cast<AliasType*>(m_RefDecl->DeclType());
	assert(decl_type);
	os << decl_type->Name() << " ";
	//os << m_Name << " ";
}

C1::AST::TypedefNameSpecifier::TypedefNameSpecifier(DeclContext* pContext, const std::string& name) : m_Name(name)
{
	TypeDeclaration* decl = dynamic_cast<TypeDeclaration*>(pContext->lookup(name));
	m_RefDecl = decl;
	if (decl)
	{
		SetRepresentType(decl->DeclType());
	}
}

C1::AST::QualifiedTypeSpecifier::QualifiedTypeSpecifier(int qm, TypeSpecifier* ts)
: m_Qualifiers(qm), m_TypeSpecifier(ts)
{
	ts->SetParent(this);
}

void C1::AST::QualifiedTypeSpecifier::Dump(std::ostream& os) const
{
	os << QualType::QulifierMaskToString(m_Qualifiers) << *m_TypeSpecifier;
}

C1::AST::VarDeclStmt::VarDeclStmt(StorageClassSpecifierEnum scs, QualifiedTypeSpecifier* qts, std::list<Declarator*>* dlist)
: m_StorageSpecifier(scs), CompoundDeclaration(qts, std::move(*dlist))
{
}

void C1::AST::VarDeclStmt::Dump(std::ostream& os) const
{
	os << m_StorageSpecifier;
	CompoundDeclaration::Dump(os);
}

C1::AST::TypedefStmt::TypedefStmt(QualifiedTypeSpecifier* qts, std::list<Declarator*>* dlist)
: CompoundDeclaration(qts, std::move(*dlist))
{
}

void C1::AST::TypedefStmt::Dump(std::ostream& os) const
{
	os << "typedef ";
	CompoundDeclaration::Dump(os);
}

void C1::AST::FieldDeclStmt::Dump(std::ostream& os) const
{
	CompoundDeclaration::Dump(os);
}

C1::AST::FieldDeclStmt::FieldDeclStmt(QualifiedTypeSpecifier* qts, std::list<Declarator*>* dlist)
: CompoundDeclaration(qts, std::move(*dlist))
{
}

void C1::AST::ParameterList::Dump(std::ostream& os) const
{
	if (empty()){
		os << "()";
		return;
	}
	os << "(";
	for (auto param : *this)
	{
		os << *static_cast<ParameterDeclaration*>(param) << ", ";
	}
	os << "\b\b)";
}

C1::AST::ParameterList::ParameterList()
{
}

//ParameterList::InsertionResult ParameterList::add(ParameterDeclaration* param)
//{
//	param->SetParent(this);
//	if (param->IsNamed())
//		DeclContext::add(param, DeclContext::NameCollisonPolicy::AlwaysRedefinable);
//	else
//		DeclContext::add(param);
//}

Declarator* C1::AST::Declarator::Atom()
{
	auto declarator = this;
	while (declarator->Base())
	{
		declarator = declarator->Base();
	}
	return declarator;
}

C1::AST::Declarator::Declarator(Declarator* base)
	: m_Base(base)
{
	if (base) {
		base->SetParent(this);
		SetLocation(base->Location());
	}
}

C1::AST::InitDeclarator::InitDeclarator(Declarator* declarator, Initializer* initializer)
: Declarator(declarator), m_Initializer(initializer)
{
}

void C1::AST::InitDeclarator::Dump(std::ostream& os) const
{
	os << *m_Base;
	if (InitializeExpr())
	{
		os <<" = " << *m_Initializer;
	}
}

C1::AST::QualType C1::AST::InitDeclarator::DecorateType(QualType base_type)
{
	if (m_Initializer->IsList())
	{
		auto list = dynamic_cast<InitializerList*>(m_Initializer.get());
		//auto arr_base = dynamic_cast<ArrayDeclarator*>(Base());

		auto res_type = Base()->DecorateType(base_type);
		auto arr_type = res_type.As<ArrayType>();
		if (arr_type) {
			if (arr_type->Length() == ArrayType::ArrayLengthAuto)
				arr_type->SetLength(list->size());
			if (arr_type->Length() < list->size())
				error(this, "Initializer-list size don't match the object-array size");
		}
		else
		{
			error(this,"Initializer-list can only initialize array type object");
		}
		return res_type;
	}
	return Base()->DecorateType(base_type);
}

C1::AST::ParenDeclarator::ParenDeclarator(Declarator* base)
: Declarator(base)
{
}

void C1::AST::ParenDeclarator::Dump(std::ostream& os) const
{
	os << "(" << *m_Base << ")";
}

C1::AST::IdentifierDeclarator::IdentifierDeclarator(const std::string& name)
: Declarator(nullptr), m_Name(name)
{

}

void C1::AST::IdentifierDeclarator::Dump(std::ostream& os) const
{
	os << m_Name;
}

void C1::AST::IdentifierDeclarator::SetIdentifier(const std::string& val)
{
	m_Name = val;
}

C1::AST::QualType C1::AST::IdentifierDeclarator::DecorateType(QualType base_type)
{
	return base_type;
}

C1::AST::PointerDeclarator::PointerDeclarator(int qualfier_mask, Declarator* base)
: Declarator(base), m_Qualifiers(qualfier_mask)
{
}

void C1::AST::PointerDeclarator::Dump(std::ostream& os) const
{
	os << "*" << QualType::QulifierMaskToString(m_Qualifiers) << *m_Base;
}

C1::AST::QualType C1::AST::PointerDeclarator::DecorateType(QualType base_type)
{
	auto decoratedType = QualType(MakePointer(base_type), QualifierMask());
	if (m_Base)
		decoratedType = m_Base->DecorateType(decoratedType);
	return decoratedType;
}

C1::AST::ArrayDeclarator::ArrayDeclarator(Declarator* base, Expr* size)
: Declarator(base), m_SizeExpr(size)
{
	//assert(size->Evaluatable());
}

void C1::AST::ArrayDeclarator::Dump(std::ostream& os) const
{
	os << *m_Base;
	if (m_SizeExpr)
		os << "[" << *m_SizeExpr << "]";
	else
		os << "[]";
}

C1::AST::QualType C1::AST::ArrayDeclarator::DecorateType(QualType base_type)
{
	size_t length = ArrayType::ArrayLengthAuto;
	if (SizeExpr() && SizeExpr()->Evaluatable()) {
		length = SizeExpr()->Evaluate().Int;
	}
	auto decoratedType = QualType(MakeArray(base_type, length));
	if (m_Base)
		decoratedType = m_Base->DecorateType(decoratedType);
	return decoratedType;
}

C1::AST::FunctionalDeclarator::FunctionalDeclarator(Declarator* base, ParameterList* param_list)
: Declarator(base), m_Parameters(param_list)
{

}

void C1::AST::FunctionalDeclarator::Dump(std::ostream& os) const
{
	os << *Base() << *m_Parameters;
}

C1::AST::QualType C1::AST::FunctionalDeclarator::DecorateType(QualType base_type)
{
	auto Context = base_type->AffiliatedContext();
	std::list<QualType> parameterTypes;
	for (auto decl : Parameters())
	{
		auto param = dynamic_cast<ParameterDeclaration*>(decl);
		parameterTypes.push_back(param->DeclType());
	}
	auto decoratedType = QualType(Context->NewFunctionType(base_type,std::move(parameterTypes)));
	if (m_Base)
		decoratedType = m_Base->DecorateType(decoratedType);
	return decoratedType;
}

C1::AST::FieldDeclarator::FieldDeclarator(Declarator* base, Expr* offset /*= nullptr*/)
: Declarator(base), m_OffsetExpr(offset)
{
	if (m_OffsetExpr)
	assert(m_OffsetExpr->Evaluatable() && m_OffsetExpr->ReturnType()->IsIntegerType());
}

C1::AST::FieldDeclarator::FieldDeclarator(Expr* offset)
: Declarator(nullptr), m_OffsetExpr(offset)
{
	assert(m_OffsetExpr);
	assert(m_OffsetExpr->Evaluatable() && m_OffsetExpr->ReturnType()->IsIntegerType());
}

void C1::AST::FieldDeclarator::Dump(std::ostream& os) const
{
	os << *Base();
	if (m_OffsetExpr)
		os << " : " << *m_OffsetExpr;
}

C1::AST::QualType C1::AST::FieldDeclarator::DecorateType(QualType base_type)
{
	return Base()->DecorateType(base_type);
}

C1::AST::Enumerator::Enumerator(const std::string &name, Expr* value_expr /*= nullptr*/)
: m_Name(name), m_ValueExpr(value_expr)
{
}

void C1::AST::Enumerator::Dump(std::ostream& os) const
{
	os << m_Name;
	if (m_ValueExpr)
		os <<" = " << *m_ValueExpr;
}

QualType C1::AST::ParenExpr::ReturnType() const
{
	return m_Base->ReturnType();
}

C1::AST::ExprValueType C1::AST::ParenExpr::ValueType() const
{
	return m_Base->ValueType();
}

bool C1::AST::ParenExpr::HasSideEffect() const
{
	return m_Base->HasSideEffect();
}

bool C1::AST::ParenExpr::Evaluatable() const
{
	return m_Base->Evaluatable();
}

C1::AST::ParenExpr::ParenExpr(Expr* base)
: m_Base(base)
{

}

void C1::AST::ParenExpr::Dump(std::ostream& os) const
{
	os << "(" << *m_Base << ")";
}

C1::AST::ExprValueType C1::AST::DereferenceExpr::ValueType() const
{
	return LValue;
}

bool C1::AST::DereferenceExpr::Evaluatable() const
{
	return false;
}

C1::AST::QualType C1::AST::DereferenceExpr::ReturnType() const
{
	auto def_type = SubExpr()->ReturnType().As<PointerType>();
	if (def_type)
		return def_type->Base();
	else
	{
		return SubExpr()->ReturnType()->AffiliatedContext()->Error();
	}
}

C1::AST::DereferenceExpr::DereferenceExpr(Expr* base)
: UnaryExpr(OP_DEREF, base)
{

}

DereferenceExpr* C1::AST::DereferenceExpr::MakeDereferenceExpr(Expr* base)
{
	auto expr = new DereferenceExpr(base);
	if (!base->ReturnType()->IsAddressType())
	{
		error(expr, "operand of Address operator (*) must be dereference-able.");
	}
	return expr;
}

C1::AST::ExprValueType C1::AST::AddressOfExpr::ValueType() const
{
	return RValue;
}

bool C1::AST::AddressOfExpr::Evaluatable() const
{
	return false;
}

C1::AST::QualType C1::AST::AddressOfExpr::ReturnType() const
{
	return MakeConst(MakePointer(SubExpr()->ReturnType()));
}

C1::AST::AddressOfExpr::AddressOfExpr(Expr* base) : UnaryExpr(OP_ADDRESS, base)
{

}

AddressOfExpr* C1::AST::AddressOfExpr::MakeAddressOfExpr(Expr* base)
{
	auto expr = new AddressOfExpr(base);
	if (base->ValueType() != LValue)
	{
		error(expr, "operand of Address operator (&) must be an L-Value expression.");
	}
	return expr;
}

C1::AST::ExprValueType C1::AST::IncrementExpr::ValueType() const
{
	return LValue;
}

bool C1::AST::IncrementExpr::HasSideEffect() const
{
	return true;
}

bool C1::AST::IncrementExpr::Evaluatable() const
{
	return false;
}

C1::AST::QualType C1::AST::SizeofExpr::ReturnType() const
{
	return m_SubExpr->ReturnType()->AffiliatedContext()->Int();
}

C1::AST::ExprValueType C1::AST::SizeofExpr::ValueType() const
{
	return RValue;
}

bool C1::AST::SizeofExpr::Evaluatable() const
{
	return true;
}

C1::AST::Expr::ComposedValue C1::AST::SizeofExpr::Evaluate() const
{
	ComposedValue val;
	val.Int = m_SubExpr->ReturnType()->Size();
	return val;
}

C1::AST::SizeofExpr::SizeofExpr(Expr* expr)
: UnaryExpr(OP_SIZEOF, expr)
{

}

C1::AST::QualType C1::AST::SizeofTypeExpr::ReturnType() const
{
	return m_TypeExpr->DeclType()->AffiliatedContext()->Int();
}

C1::AST::ExprValueType C1::AST::SizeofTypeExpr::ValueType() const
{
	return RValue;
}

bool C1::AST::SizeofTypeExpr::HasSideEffect() const
{
	return false;
}

C1::AST::Expr::ComposedValue C1::AST::SizeofTypeExpr::Evaluate() const
{
	ComposedValue val;
	val.Int = m_TypeExpr->DeclType()->Size();
	return val;
}

bool C1::AST::SizeofTypeExpr::Evaluatable() const
{
	return true;
}

C1::AST::SizeofTypeExpr::SizeofTypeExpr(TypeExpr* type_expr)
: m_TypeExpr(type_expr)
{
	type_expr->SetParent(this);
}

void C1::AST::SizeofTypeExpr::Dump(std::ostream& os) const
{
	os << "sizeof (" << *m_TypeExpr << ")";
}

C1::AST::ExprValueType C1::AST::ConstantLiteral::ValueType() const
{
	return RValue;
}

bool C1::AST::ConstantLiteral::HasSideEffect() const
{
	return false;
}

C1::AST::Expr::ComposedValue C1::AST::ConstantLiteral::Evaluate() const
{
	return m_Value;
}

bool C1::AST::ConstantLiteral::Evaluatable() const
{
	return true;
}

void C1::AST::ConstantLiteral::Dump(std::ostream& ostr) const
{
	ostr << m_RawLiteral;
}

C1::AST::ConstantLiteral::ConstantLiteral(TypeContext* type_context, const std::string& raw_str)
: m_TypeContext(type_context), m_RawLiteral(raw_str)
{
}

C1::AST::IntegerLiteral::IntegerLiteral(TypeContext* type_context, const char* raw_str, int value, int dec)
: ConstantLiteral(type_context, raw_str), m_decimal(dec)
{
	m_Value.Int = value;
}

C1::AST::QualType C1::AST::IntegerLiteral::ReturnType() const
{
	return MakeConst(m_TypeContext->Int());
}

C1::AST::FloatLiteral::FloatLiteral(TypeContext* type_context, const char* raw_str, float value)
: ConstantLiteral(type_context, raw_str)
{
	m_Value.Float = value;
}

C1::AST::QualType C1::AST::FloatLiteral::ReturnType() const
{
	return MakeConst(m_TypeContext->Float());
}

C1::AST::CharacterLiteral::CharacterLiteral(TypeContext* type_context, const char* raw_str, char value)
: ConstantLiteral(type_context, raw_str)
{
	m_Value.Char = value;
}

C1::AST::QualType C1::AST::CharacterLiteral::ReturnType() const
{
	return MakeConst(m_TypeContext->Char());
}

C1::AST::QualType C1::AST::AtomInitializer::ReturnType() const
{
	return m_ValueExpr->ReturnType();
}

C1::AST::ExprValueType C1::AST::AtomInitializer::ValueType() const
{
	return m_ValueExpr->ValueType();
}

bool C1::AST::AtomInitializer::HasSideEffect() const
{
	return m_ValueExpr->HasSideEffect();
}

C1::AST::Expr::ComposedValue C1::AST::AtomInitializer::Evaluate() const
{
	return m_ValueExpr->Evaluate();
}

bool C1::AST::AtomInitializer::Evaluatable() const
{
	return m_ValueExpr->Evaluatable();
}

void C1::AST::AtomInitializer::Dump(std::ostream& os) const
{
	os << *m_ValueExpr;
}

C1::AST::AtomInitializer::AtomInitializer(Expr* value_expr)
: m_ValueExpr(value_expr)
{

}

C1::AST::QualType C1::AST::CastExpr::ReturnType() const
{
	return m_TargetType;
}

C1::AST::ExprValueType C1::AST::CastExpr::ValueType() const
{
	return RValue;
}

bool C1::AST::CastExpr::HasSideEffect() const
{
	return m_SourceExpr->HasSideEffect();
}

Expr::ComposedValue C1::AST::CastExpr::Evaluate() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

bool C1::AST::CastExpr::Evaluatable() const
{
	return m_SourceExpr->Evaluatable();
}

void C1::AST::CastExpr::Dump(std::ostream& ostr) const
{
	ostr << *m_SourceExpr;
}

C1::AST::CastExpr::CastExpr(Expr* source_expr, QualType target_type, FunctionDeclaration* conversion_func /*= nullptr*/)
: m_SourceExpr(source_expr), m_TargetType(target_type), m_ConversionFunction(conversion_func)
{
	source_expr->SetParent(this);
	SetLocation(source_expr->Location());
}

Expr* C1::AST::CastExpr::MakeImplictitCastExpr(QualType target_type, Expr* source_expr)
{
	auto expr_type = source_expr->ReturnType();
	if (target_type.IsConst())
		expr_type.AddConst(); // non-const to const cast, perfect fine
	if (expr_type->IsErrorType() || target_type->IsErrorType())
		return source_expr;
	if (expr_type == target_type)
		return source_expr; // source type matches target type
	else 
		return new ImplicitCastExpr(target_type, source_expr);
}

void C1::AST::ImplicitCastExpr::Dump(std::ostream& os) const
{
	os << "cast<" << m_TargetType << ">(" << *m_SourceExpr << ")";
}

C1::AST::ImplicitCastExpr::ImplicitCastExpr(QualType target_type, Expr* source_expr)
: CastExpr(source_expr,target_type)
{
}

template <typename T>
void C1::AST::CompoundDeclaration<T>::Dump(std::ostream& os) const
{
	os << *m_QTSpecifier ;
	for (auto declarator : DeclaratorList())
	{
		os << *declarator << ", ";
	}
	if (!DeclaratorList().empty())
		os << "\b\b;" << endl;
	else
		os << ";" << endl;
}

template <typename T>
C1::AST::CompoundDeclaration<T>::CompoundDeclaration(QualifiedTypeSpecifier* QTSpecifier, std::list<Declarator*> &&declarator_list) : m_DeclaratorList(std::move(declarator_list)), m_QTSpecifier(QTSpecifier)
{
	m_QTSpecifier->SetParent(this);
	for (auto dr : m_DeclaratorList)
	{
		dr->SetParent(this);
	}
}

C1::AST::Expr::ComposedValue C1::AST::Expr::Evaluate() const
{
	ComposedValue val;
	val.Int = 0;
	return val;
}

C1::AST::Initializer::Initializer()
{

}

C1::AST::ExprStmt::ExprStmt(Expr* expr)
: m_Expr(expr)
{
	expr->SetParent(this);
}

void C1::AST::ExprStmt::Dump(std::ostream& os) const
{
	os << *m_Expr << ";" << endl;
}

C1::AST::ReturnStmt::ReturnStmt(Expr* return_expr /*= nullptr*/)
: ExprStmt(return_expr)
{
}

void C1::AST::ReturnStmt::Dump(std::ostream& os) const
{
	os << "return " << *m_Expr << ";" << endl;
}

C1::AST::ContinueStmt::ContinueStmt()
{

}

void C1::AST::ContinueStmt::Dump(std::ostream& os) const
{
	os << "continue;" << endl;
}

C1::AST::BreakStmt::BreakStmt()
{

}

void C1::AST::BreakStmt::Dump(std::ostream& os) const
{
	os << "break;" << endl;
}

C1::AST::IterationStmt::IterationStmt(Stmt* action /*= nullptr*/, Expr* condtion /*= nullptr*/)
: m_Action(action), m_Condition(condtion)
{
	m_Action->SetParent(this);
	m_Condition->SetParent(this);
}

void C1::AST::NullStmt::Dump(std::ostream& os) const
{
	os << ";" << endl;
}

void C1::AST::TranslationUnit::Dump(std::ostream& ostr) const
{
	for (auto node : Children())
	{
		ostr << * node;
	}
}

C1::AST::TranslationUnit::TranslationUnit(std::string filename)
:m_FileName(filename)
{
}

C1::AST::TranslationUnit::~TranslationUnit()
{
	set_parent(nullptr);
}

void C1::AST::TypeSpecifier::Dump(std::ostream& ostr) const
{
	ostr << "type_specifier";
}

C1::AST::QualType C1::AST::EmptyDeclarator::DecorateType(QualType base_type)
{
	return base_type;
}

void C1::AST::EmptyDeclarator::Dump(std::ostream& ostr) const
{
}

C1::AST::EmptyDeclarator::EmptyDeclarator()
{

}
