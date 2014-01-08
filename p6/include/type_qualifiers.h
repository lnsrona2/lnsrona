#pragma once

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
		RESTRICT = 0x2,
		VOLATILE = 0x4,
	};

	enum RecordKeywordEnum
	{
		STRUCT = 1,
		UNION = 2,
		ENUM = 3,
	};

}