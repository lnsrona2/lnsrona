#include "stdafx.h"
#include "type.h"
#include "declaration.h"
#include "ast.h"

const char C1::AST::RecordType::AnonymousName[] = "%anonymous";

using namespace std;
using namespace C1;
using namespace C1::AST;

size_t C1::AST::InitializerListType::Alignment() const
{
	return 0;
}

size_t C1::AST::InitializerListType::Size() const
{
	size_t size = 0;
	for (auto qt : ElementTypes())
	{
		size += qt->Size();
	}
	return size;
}

C1::AST::InitializerListType::InitializerListType(TypeContext* context)
: Type(class_kind)
{
	SetAffiliatedContext(context);
}

bool C1::AST::InitializerListType::Match(const Type* type) const
{
	if (this == type) return true;
	auto rhs = dynamic_cast<const InitializerListType*>(type);
	if (!rhs) return false;
	if (this->ElementTypes().size() != rhs->ElementTypes().size())
		return false;
	auto litr = this->ElementTypes().cbegin();
	auto lend = this->ElementTypes().cend();
	auto ritr = rhs->ElementTypes().cbegin();
	for (; litr != lend; ++litr,++ritr)
	{
		if (type_match(*litr, *ritr))
			return false;
	}
	return true;
}

C1::AST::InitializerListType::~InitializerListType()
{

}

C1::AST::DereferencableType::DereferencableType()
{

}

C1::AST::DereferencableType::DereferencableType(QualType base)
: m_base(base)
{

}

C1::AST::DereferencableType::DereferencableType(TypeKindEnum kind, QualType base)
: Type(kind), m_base(base)
{

}

const QualType C1::AST::DereferencableType::Base() const
{
	return m_base;
}

C1::AST::QualType C1::AST::DereferencableType::Base()
{
	return m_base;
}

C1::AST::DereferencableType::~DereferencableType()
{

}

C1::AST::Type::Type()
: m_Kind(Unknown)
{

}

C1::AST::Type::Type(TypeKindEnum kind)
: m_Kind(kind)
{

}

void C1::AST::Type::SetAffiliatedContext(TypeContext* val)
{
	m_AffiliatedContext = val;
}

const TypeContext* C1::AST::Type::AffiliatedContext() const
{
	return m_AffiliatedContext;
}

TypeContext* C1::AST::Type::AffiliatedContext()
{
	return m_AffiliatedContext;
}

std::string C1::AST::Type::ToString() const
{
	return "type_place_holder";
}

bool C1::AST::Type::IsArithmeticType() const
{
	return m_Kind >= Boolean && m_Kind <= Float;
}

bool C1::AST::Type::IsInitializerListType() const
{
	return m_Kind == InitializerList;
}

bool C1::AST::Type::IsAliasType() const
{
	return m_Kind == Typedef;
}

bool C1::AST::Type::IsStructType() const
{
	return m_Kind == Struct;
}

bool C1::AST::Type::IsFunctionType() const
{
	return m_Kind == Function;
}

bool C1::AST::Type::IsIntegerType() const
{
	return m_Kind == Integer;
}

bool C1::AST::Type::IsBasicType() const
{
	return m_Kind >= Void && m_Kind <= String;
}

bool C1::AST::Type::IsArrayType() const
{
	return m_Kind == Array;
}

bool C1::AST::Type::IsPointerType() const
{
	return m_Kind == Pointer;
}

C1::AST::Type::~Type()
{

}

C1::AST::RecordType::RecordType(TypeKindEnum kind, const std::string &name, DeclContext* define /*= nullptr*/)
: Type(kind), m_Name(name), m_Members(define)
{

}

C1::AST::RecordType::~RecordType()
{

}

C1::AST::StructType::StructType(const std::string &name /*= "%anonymous"*/, DeclContext* define /*= nullptr*/)
: RecordType(Struct, name, define)
{

}

size_t C1::AST::StructType::Size() const
{
	size_t size = 0;
	for (auto decl : *Definition())
	{
		auto field = dynamic_cast<FieldDeclaration*>(decl);
		size += field->DeclType()->Size();
	}
	return size;
}

size_t C1::AST::StructType::Alignment() const
{
	size_t alignment = 0;
	for (auto decl : *Definition())
	{
		auto field = dynamic_cast<FieldDeclaration*>(decl);
		alignment = std::max(alignment, field->DeclType()->Alignment());
	}
	return alignment;
}

bool C1::AST::StructType::Match(const Type* type) const
{
	if (this == type) return true;
	auto rhs = dynamic_cast<const StructType*>(type);
	if (!rhs) return false;
	if (Name() == AnonymousName || rhs->Name() == AnonymousName)
		return false;
	// Should be fully-qualified name here
	return Name() == rhs->Name();
}

C1::AST::StructType::~StructType()
{

}

std::string C1::AST::StructType::ToString() const
{
	return "struct " + Name();
}

C1::AST::FunctionType::FunctionType(QualType return_type, const std::list<QualType>& parameters)
: Type(class_kind), m_ReturnType(return_type), m_Parameters(parameters)
{
	SetAffiliatedContext(return_type->AffiliatedContext());
}

C1::AST::FunctionType::FunctionType(QualType return_type, std::list<QualType>&& parameters)
: Type(class_kind), m_ReturnType(return_type), m_Parameters(std::move(parameters))
{
	SetAffiliatedContext(return_type->AffiliatedContext());
}

size_t C1::AST::FunctionType::Size() const
{
	return AffiliatedContext()->AddressWidth();
}

size_t C1::AST::FunctionType::Alignment() const
{
	return AffiliatedContext()->AddressAlignment();
}

bool C1::AST::FunctionType::Match(const Type* type) const
{
	if (this == type) return true;
	auto rhs = dynamic_cast<const FunctionType*>(type);
	if (!rhs) return false;
	if (!type_match(ReturnType(), rhs->ReturnType()))
		return false;

	if (this->Parameters().size() != rhs->Parameters().size())
		return false; // parameter count not equal
	auto litr = this->Parameters().cbegin();
	auto lend = this->Parameters().cend();
	auto ritr = rhs->Parameters().cbegin();
	for (; litr != lend; ++litr, ++ritr)
	{
		if (!type_match(*litr, *ritr))
			return false;
	}
	return true;
}

C1::AST::FunctionType::~FunctionType()
{

}

C1::AST::BasicType::BasicType(TypeKindEnum kind, size_t size, size_t alignment)
: Type(kind), m_Size(size), m_Alignment(alignment)
{

}

C1::AST::BasicType::BasicType(TypeKindEnum kind, size_t size)
: Type(kind), m_Size(size), m_Alignment(0)
{

}

bool C1::AST::BasicType::Match(const Type* type) const
{
	if (this == type) return true;
	auto rhs = dynamic_cast<const BasicType*>(type);
	if (!rhs) return false;
	return m_Size == rhs->m_Size && m_Alignment == rhs->m_Alignment;
}

C1::AST::BasicType::~BasicType()
{

}

C1::AST::VoidType::VoidType()
: BasicType(Void, 0, 0)
{

}

std::string C1::AST::VoidType::ToString() const
{
	return "void";
}

C1::AST::BooleanType::BooleanType()
: BasicType(Boolean, 0, 0)
{

}

std::string C1::AST::BooleanType::ToString() const
{
	return "bool";
}

C1::AST::IntegerType::IntegerType()
: BasicType(Integer, 4, 4)
{

}

C1::AST::IntegerType::IntegerType(size_t size)
: BasicType(class_kind, size)
{

}

std::string C1::AST::IntegerType::ToString() const
{
	return "int";
}

C1::AST::CharacterType::CharacterType() : BasicType(Character, 1, 1)
{

}

std::string C1::AST::CharacterType::ToString() const
{
	return "char";
}

C1::AST::FloatType::FloatType() : BasicType(Float, 4, 4)
{

}

std::string C1::AST::FloatType::ToString() const
{
	return "float";
}

//C1::AST::QualType C1::AST::TypeContext::NewTypeFromDeclarator(QualType base_type, Declarator* declarator)
//{
//	//return declarator->DecorateType(base_type);
//}

PointerType* C1::AST::TypeContext::NewPointerType(QualType base)
{
	auto type = new PointerType(base);
	type->SetAffiliatedContext(this);
	m_ResourcesPool.emplace_back(type);
	return type;
}

ArrayType* C1::AST::TypeContext::NewArrayType(QualType base, size_t size)
{
	auto type = new ArrayType(base, size);
	type->SetAffiliatedContext(this);
	m_ResourcesPool.emplace_back(type);
	return type;
}

InitializerListType* C1::AST::TypeContext::NewInitializerListType()
{
	auto type = new InitializerListType(this);
	type->SetAffiliatedContext(this);
	m_ResourcesPool.emplace_back(type);
	return type;
}

FunctionType* C1::AST::TypeContext::NewFunctionType(QualType return_type, std::list<QualType>&& parameter_type_list)
{
	auto type = new FunctionType(return_type, std::move(parameter_type_list));
	type->SetAffiliatedContext(this);
	auto ptr = static_cast<Type*>(type);
	m_ResourcesPool.emplace_back(ptr);
	return type;

}

StructType* C1::AST::TypeContext::NewStructType(const std::string& name)
{
	auto type = new StructType(name);
	type->SetAffiliatedContext(this);
	m_ResourcesPool.emplace_back(type);
	return type;
}

C1::AST::TypeContext::TypeContext()
{
	m_Char = make_unique<IntegerType>(1);
	m_Char->SetAffiliatedContext(this);
	m_Short = make_unique<IntegerType>(2);
	m_Short->SetAffiliatedContext(this);
	m_Int = make_unique<IntegerType>(4);
	m_Int->SetAffiliatedContext(this);
	m_Long = make_unique<IntegerType>(4);
	m_Long->SetAffiliatedContext(this);
	m_Bool = make_unique<BooleanType>();
	m_Bool->SetAffiliatedContext(this);
	m_Void = make_unique<VoidType>();
	m_Void->SetAffiliatedContext(this);
	m_Float = make_unique<FloatType>();
	m_Float->SetAffiliatedContext(this);
	m_String.reset(NewPointerType(MakeConst(Char())));
	m_String->SetAffiliatedContext(this);
}

AliasType* C1::AST::TypeContext::NewAliasType(QualType base, const std::string& alias)
{
	auto type = new AliasType(base, alias);
	type->SetAffiliatedContext(this);
	m_ResourcesPool.emplace_back(type);
	return type;
}

RecordType* C1::AST::TypeContext::NewRecordType(RecordKeywordEnum record_type, const std::string& name /*= "%anonymous"*/)
{
	switch (record_type)
	{
	case C1::STRUCT:
		return NewStructType(name);
		break;
	case C1::UNION:
		break;
	case C1::ENUM:
		break;
	default:
		break;
	}
	return nullptr;
}


size_t C1::AST::ArrayType::Size() const
{
	return m_size*m_base->Size();
}

size_t C1::AST::ArrayType::Alignment() const
{
	return m_base->Alignment();
}

bool C1::AST::ArrayType::Match(const Type* type) const
{
	if (this == type) return true;
	auto rhs = dynamic_cast<const ArrayType*>(type);
	if (!rhs) return false;
	return Length() == rhs->Length() && type_match(Base(), rhs->Base());
}

C1::AST::ArrayType::ArrayType(QualType base, size_t size) : DereferencableType(class_kind, base), m_size(size)
{

}

C1::AST::ArrayType::~ArrayType()
{

}

std::string C1::AST::ArrayType::ToString() const
{
	return m_base->ToString() + "[]";
}

size_t C1::AST::PointerType::Size() const
{
	return m_AffiliatedContext->AddressWidth();
}

size_t C1::AST::PointerType::Alignment() const
{
	return m_AffiliatedContext->AddressAlignment();
}

bool C1::AST::PointerType::Match(const Type* type) const
{
	if (this == type) return true;
	auto rhs = dynamic_cast<const PointerType*>(type);
	if (!rhs) return false;
	return type_match(Base(), rhs->Base());
}

C1::AST::PointerType::PointerType(QualType base) : DereferencableType(class_kind, base)
{

}

C1::AST::PointerType::~PointerType()
{

}

std::string C1::AST::PointerType::ToString() const
{
	return m_base->ToString() + "*";
}

size_t C1::AST::AliasType::Size() const
{
	return m_Base->Size();
}

size_t C1::AST::AliasType::Alignment() const
{
	return m_Base->Alignment();
}

bool C1::AST::AliasType::Match(const Type* type) const
{
	return m_Base->Match(type);
}

C1::AST::AliasType::AliasType(QualType aliasd_type, const std::string& name)
: m_Base(aliasd_type), m_Name(name)
{

}

C1::AST::AliasType::~AliasType()
{

}

std::string C1::AST::AliasType::ToString() const
{
	return m_Name;
}

// Remove the alias type in this type (not recursively)
QualType remove_alias(QualType lhs)
{
	int mask = 0;
	while (lhs->IsAliasType()) {
		mask = lhs.Qualifiers();
		lhs = lhs.As<AliasType>()->Base();
		lhs.AddQualifiers(mask);
	}
	return lhs;
}

namespace C1
{
	namespace AST
	{
		bool type_match(QualType lhs, QualType rhs)
		{
			// Remove alias
			lhs = remove_alias(lhs);
			rhs = remove_alias(rhs);

			// Qualifier different
			if (lhs.Qualifiers() != rhs.Qualifiers())
			{
				return false;
			}
			if (lhs->AffiliatedContext() != rhs->AffiliatedContext())
			{
				return false;
			}

			// Kind different
			if (lhs->Kind() != rhs->Kind())
				return false;

			return lhs->Match(rhs.get());
		}
	}
}
