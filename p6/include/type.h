#pragma once
#ifndef _TYPE_H_
#define _TYPE_H_

#include <list>
#include <string>

namespace C1
{
	class DeclContext;

	enum TypeQualifierEnum
	{
		CONST = 0x1,
		RESTRIC = 0x2,
		VOLATILE = 0x4,
	};

	class Type
	{
	public:
		virtual std::string ToString() const;
		virtual bool IsCompleted() const;
		virtual size_t Size() const;
		virtual size_t Alignment() const;
		virtual ~Type();
	};

	class QualType
	{
		bool IsConst() const
		{
			return m_qulifier_mask & TypeQualifierEnum::CONST;
		}
		bool IsRestrict() const
		{
			return m_qulifier_mask & TypeQualifierEnum::RESTRIC;
		}
		bool IsVolatile() const
		{
			return m_qulifier_mask & TypeQualifierEnum::VOLATILE;
		}

		std::string ToString() const
		{
			return QulifierMaskToString(m_qulifier_mask) + m_type->ToString();
		}

		size_t Size() const
		{
			return m_type->Size();
		}

		size_t Alignment() const
		{
			return m_type->Alignment();
		}

		const Type* CanonicalType() const;

		Type* CanonicalType();

		static const std::string& QulifierMaskToString(unsigned int qulifier_mask)
		{
			static std::string qulifier_list_name[] = { "", "const ", "restrict ", "const restrict ", "volatile ", "const volatile ", "restrict volatile ", "const restrict volatile " };
			if (qulifier_mask < 8)
				return qulifier_list_name[qulifier_mask];
			else
				return qulifier_list_name[0];
		}

	private:
		Type* m_type;
		TypeQualifierEnum m_qulifier_mask;
	};

	class TypeContext
	{
		const Type*			NewTypeFromTypeSpecifier();
		const Type*			NewTypeFromSpecifierQualifierList();
		const Type*			NewTypeFromDeclarator();

		const PointerType*	NewPointerType();
		const ArrayType*	NewArrayType();
		const StructType*	NewStructType();
		const UnionType*	NewUnionType();
		const QualType*		NewQualType(Type* base_type, unsigned qualifier_mask);

		const IntegerType*	Char() const;
		const IntegerType*	Short() const;
		const IntegerType*	Int() const;
		const IntegerType*	UnsignedChar() const;
		const IntegerType*	UnsignedShort() const;
		const IntegerType*	UnsignedInt() const;
		const BooleanType*	Bool() const;
		const VoidType*		Void() const;
		const FloatType*	Float() const;
		const FloatType*	Double() const;
	};

	class PointerType : Type
	{
		const Type * Base() const;
	};
	class ArrayType : Type
	{
		const Type * Base() const;
		int Length() const;
	};
	class StructType : Type
	{
		const DeclContext* Context() const;
		const std::list<Type*> Elements() const;
	};
	class UnionType : Type
	{
		const DeclContext* Context() const;
		const std::list<Type*> Elements() const;
	};
	class BasicType : Type
	{};
	class VoidType : BasicType
	{};
	class BooleanType : BasicType
	{};
	class IntegerType : BasicType
	{};
	class FloatType : BasicType
	{};
}

#endif // if !_TYPE_H_