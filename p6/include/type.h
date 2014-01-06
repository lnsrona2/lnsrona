#pragma once
#ifndef _TYPE_H_
#define _TYPE_H_

#include <list>
#include <string>
#include "decl_context.h"

namespace C1
{
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

	namespace AST
	{
		class DeclContext;
		class RecordDeclaration;
		class Declaration;
		class Declarator;

		// Represent an type in the program , it's an Semantic entity
		class Type
		{
		public:
			virtual std::string ToString() const;
			virtual size_t Size() const;
			virtual size_t Alignment() const;
			virtual ~Type();
		};

		bool type_match(const Type &lhs, const Type &rhs);
		inline bool operator==(const Type &lhs, const Type &rhs)
		{
			return type_match(lhs, rhs);
		}

		// Serve as an pointer of "Type" class, but provide qualifier information
		class QualType
		{
		public:
			QualType(Type *pType, int qualifiers_mask)
				: m_type(pType), m_qulifier_mask(qualifiers_mask)
			{
			}

			QualType()
				: m_type(nullptr), m_qulifier_mask(0)
			{}

			int Qualifiers() const
			{
				return m_qulifier_mask;
			}

			void reset(Type* type)
			{
				m_type = type;
			}

			Type* get()
			{
				return m_type;
			}

			const Type* get() const
			{
				return m_type;
			}

			bool IsConst() const
			{
				return (m_qulifier_mask & TypeQualifierEnum::CONST) != 0;
			}
			bool IsRestrict() const
			{
				return (m_qulifier_mask & TypeQualifierEnum::RESTRIC) != 0;
			}
			bool IsVolatile() const
			{
				return (m_qulifier_mask & TypeQualifierEnum::VOLATILE) != 0;
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
				static std::string qulifier_list_name [] = { "", "const ", "restrict ", "const restrict ", "volatile ", "const volatile ", "restrict volatile ", "const restrict volatile " };
				if (qulifier_mask < 8)
					return qulifier_list_name[qulifier_mask];
				else
					return qulifier_list_name[0];
			}

		private:
			Type* m_type;
			int m_qulifier_mask;
		};

		class PointerType : public Type
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
		class ArrayType : public Type
		{
		public:
			ArrayType(QualType base, size_t size)
				: m_base(base), m_size(size)
			{
			}
			const QualType Base() const;
			int Length() const;
		private:
			QualType m_base;
			size_t m_size;
		};

		class RecordType : public Type
		{
			StructDefination* Defination();
		};

		class StructType : public RecordType
		{
		};

		class EnumType : public RecordType
		{

		};

		class UnionType : public RecordType
		{
		};
		class BasicType : public Type
		{};
		class VoidType : public BasicType
		{};
		class BooleanType : public BasicType
		{};
		class IntegerType : public BasicType
		{};
		class FloatType : public BasicType
		{};

		class TypeContext
		{
		public:
			const Type*			NewTypeFromTypeSpecifier();
			const QualType*		NewTypeFromSpecifierQualifierList();
			//const Type*			NewTypeFromDeclarator();

			PointerType*NewPointerType(QualType base);
			ArrayType*	NewArrayType(QualType base, size_t size);
			StructType*	NewStructType(const std::string& name, std::list<Declaration*>* field_decl_list);
			StructType*	NewStructType(std::list<Declaration*>* field_decl_list);
			StructType*	NewStructType(const std::string& name);
			EnumType*	NewEnumType(const std::string& name, std::list<Declarator*>* enumerator_list);
			EnumType*	NewEnumType(std::list<Declarator*>* enumerator_list);
			EnumType*	NewEnumType(const std::string& name);
			UnionType*	NewUnionType();

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

			IntegerType*	Char();
			IntegerType*	Short();
			IntegerType*	Int();
			IntegerType*	UnsignedChar();
			IntegerType*	UnsignedShort();
			IntegerType*	UnsignedInt();
			BooleanType*	Bool();
			VoidType*		Void();
			FloatType*	Float();
			FloatType*	Double();
		};
	}
}

#endif // if !_TYPE_H_