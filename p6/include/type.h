#pragma once
#ifndef _TYPE_H_
#define _TYPE_H_

#include <list>
#include <string>
#include "decl_context.h"
#include "type_qualifiers.h"

namespace C1
{
	namespace AST
	{
		class DeclContext;
		//class RecordDeclaration;
		//class StructBody;
		class Declaration;
		class Declarator;

		// Represent an type in the program , it's an Semantic entity
		class Type
		{
		public:
			// register all the sub classes here
			enum TypeKindEnum
			{
				Unknown,
				Basic,
				Void,
				Boolean,
				Character,
				Integer,
				Float,
				String,
				Pointer,
				Array,
				Function,
				Struct,
				Union,
				Enum,
				Typedef,
			};
			virtual std::string ToString() const;
			virtual size_t Size() const;
			virtual size_t Alignment() const;
			virtual ~Type();

		protected:
			Type(TypeKindEnum kind)
				: m_Kind(kind)
			{}

			Type()
				: m_Kind(Unknown)
			{}

			TypeKindEnum m_Kind;
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
				return (m_qulifier_mask & TypeQualifierEnum::RESTRICT) != 0;
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
			static const TypeKindEnum class_kind = Pointer;
			PointerType(QualType base)
				: Type(class_kind), m_base(base)
			{
			}
			const QualType Base() const;
		private:
			QualType m_base;
		};

		class ArrayType : public Type
		{
		public:
			static const TypeKindEnum class_kind = Array;
			ArrayType(QualType base, size_t size)
				: Type(class_kind), m_base(base), m_size(size)
			{}
			const QualType Base() const;
			int Length() const;
		private:
			QualType m_base;
			size_t m_size;
		};

		class RecordType : public Type
		{
		public:
			static const TypeKindEnum class_kind = Unknown;
			RecordType(TypeKindEnum kind, const std::string &name, DeclContext* define = nullptr)
				: Type(kind), m_Name(name), m_Members(define)
			{}
			const std::string& Name() const { return m_Name; }
			bool IsCompleted() const { return m_Members != nullptr; }

			// We don't discard all the member's name detail here becaus we need them
			DeclContext* Members() { return m_Members; }
			const DeclContext* Members() const { return m_Members; }

			void SetDefinition(DeclContext* define) { m_Members = define; }

		protected:
			std::string m_Name;
			DeclContext* m_Members;
		};

		class StructType : public RecordType
		{
		public:
			static const TypeKindEnum class_kind = Struct;
			StructType(const std::string &name = "%anonymous", DeclContext* define = nullptr)
				: RecordType(Struct,name,define)
			{}
		};

		class FunctionType : public Type
		{
		public:
			static const TypeKindEnum class_kind = Function;
			FunctionType(QualType return_type, const std::list<QualType>& parameters)
				: Type(class_kind), m_ReturnType(return_type), m_Parameters(parameters)
			{}
			FunctionType(QualType return_type, std::list<QualType>&& parameters)
				: Type(class_kind), m_ReturnType(return_type), m_Parameters(std::move(parameters))
			{}
			QualType ReturnType();
			const QualType ReturnType() const;
			// Since it's an type, we discard all the parameter's name
			std::list<QualType>& Parameters();
			const std::list<QualType>& Parameters() const;
		private:
			QualType m_ReturnType;
			std::list<QualType> m_Parameters;
		};

		class EnumType : public RecordType
		{
			static const TypeKindEnum class_kind = Enum;

		};

		class UnionType : public RecordType
		{
			static const TypeKindEnum class_kind = Union;
		};

		class BasicType : public Type
		{
		public:
			static const TypeKindEnum class_kind = Basic;
			typedef Type::TypeKindEnum TypeKindEnum;

			size_t Size() const {
				return m_Size;
			}

			size_t Alignment() const {
				return m_Alignment;
			}
		protected:
			BasicType(TypeKindEnum kind, size_t size, size_t alignment)
				: Type(kind), m_Size(size), m_Alignment(alignment)
			{}
			BasicType(TypeKindEnum kind, size_t size)
				: Type(kind), m_Size(size), m_Alignment(0)
			{}

			size_t m_Size;
			size_t m_Alignment;
		};
		class VoidType : public BasicType
		{
		public:
			static const TypeKindEnum class_kind = Void;
			VoidType()
				: BasicType(Void,0,0)
			{}
		};
		class BooleanType : public BasicType
		{
		public:
			static const TypeKindEnum class_kind = Integer;
			BooleanType()
				: BasicType(Boolean, 0, 0)
			{}
		};
		class IntegerType : public BasicType
		{
			static const TypeKindEnum class_kind = Integer;
			IntegerType()
				: BasicType(Integer, 4,4)
			{}
		};
		class CharacterType : public BasicType
		{
			static const TypeKindEnum class_kind = Character;
			CharacterType()
				: BasicType(Character, 1, 1)
			{}
		};
		class FloatType : public BasicType
		{
			static const TypeKindEnum class_kind = Float;
			FloatType()
			: BasicType(Float, 4, 4)
			{}
		};

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