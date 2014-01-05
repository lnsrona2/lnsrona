#pragma once
#ifndef _TYPE_H_
#define _TYPE_H_

#include <list>
#include <string>

namespace C1
{
	class DeclContext;
	class RecordDeclaration;

	enum StorageClassSpecifierEnum
	{
		SCS_EXTERN,
		SCS_STATIC,
		SCS_AUTO,
		SCS_REGISTER,
		SCS_NONE,
	};

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
		virtual size_t Size() const;
		virtual size_t Alignment() const;
		virtual ~Type();
	};

	class QualType
	{
		QualType(Type *pType,int qualifiers_mask)
		: m_type(pType), m_qulifier_mask(qualifiers_mask)
		{
		}

		int Qualifiers() const
		{
			return m_qulifier_mask;
		}

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


		int AddConst()
		{
			m_qulifier_mask |= TypeQualifierEnum::CONST;
		}

		int RemoveConst()
		{
			m_qulifier_mask &= ~TypeQualifierEnum::CONST;
		}

		inline const Type &operator*() const{
			return *m_type;
		}

		inline const Type *operator->() const{
			return m_type;
		}

		inline Type &operator*() {
			return *m_type;
		}

		inline Type *operator->() {
			return m_type;
		}

	protected:

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
		int m_qulifier_mask;
	};

	class TypeContext
	{
		const Type*			NewTypeFromTypeSpecifier();
		const QualType*		NewTypeFromSpecifierQualifierList();
		//const Type*			NewTypeFromDeclarator();

		const PointerType*	NewPointerType(QualType base);
		const ArrayType*	NewArrayType(QualType base , size_t size);
		const StructType*	NewStructType();
		const UnionType*	NewUnionType();

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
	public:
		PointerType(QualType base)
		: m_base(base)
		{
		}
		const QualType Base() const;
	private:
		QualType m_base;
	};
	class ArrayType : Type
	{
	public:
		ArrayType(QualType base , size_t size)
			: m_base(base), m_size(size)
		{
		}
		const QualType Base() const;
		int Length() const;
	private:
		QualType m_base;
		size_t m_size;
	};

	class RecordType : Type
	{
		const RecordDeclaration* Declaration() const;
		RecordDeclaration* Declaration();

	protected:
		RecordDeclaration* m_declaration;
	};

	class StructType : RecordType
	{
	};

	class UnionType : RecordType
	{
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