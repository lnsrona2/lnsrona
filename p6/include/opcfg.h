/* opxx(a,b) OP##a is the enumerate no of operator b */
#ifdef opxx
	opxx(RET,"return","return %s")
	opxx(NEG,"(-)","-%s")
	opxx(ADD,"+","%s + %s")
	opxx(SUB, "-","%s - %s")
	opxx(MUL, "*", "%s * %s")
	opxx(DIV, "/", "%s / %s")
	opxx(MOD, "%", "%s \% %s")
	opxx(AND, "&", "%s & %s")
	opxx(OR, "|", "%s | %s")
	opxx(XOR, "^", "%s ^ %s")
	opxx(LSH, "<<", "%s << %s")
	opxx(RSH, ">>", "%s >> %s")
	opxx(EQL, "==", "%s == %s")
	opxx(NEQ, "!=", "%s != %s")
	opxx(LSS, "<", "%s < %s")
	opxx(GEQ, ">=", "%s >= %s")
	opxx(GTR, ">", "%s > %s")
	opxx(LEQ, "<=", "%s <= %s")
	opxx(ANDAND, "&&", "%s && %s")
	opxx(OROR, "||", "%s || %s")
	opxx(NOTNOT, "!", "!%s")
	opxx(NOT, "~", "~%s")
	opxx(INDEX, "[]", "%s[%s]")
	opxx(CALL, "()", "%s(%s)")
	opxx(DOT, ".", "%s.%s")
	opxx(ARROW, "->", "%s->%s")
	opxx(POSFIX_ADDADD, "++", "%s++")
	opxx(POSFIX_SUBSUB, "--", "--%s")
	opxx(ADDADD, "++", "++%s")
	opxx(SUBSUB, "--", "--%s")
	opxx(DEREF, "*", "*%s")
	opxx(ADDRESS, "&", "&%s")
	opxx(REF, "@", "%s")
	opxx(CAST, "cast", "cast<%s>(%s)")
	opxx(READ, "read", "read(%s)")
	opxx(WRITE, "write", "write(%s)")
	opxx(SIZEOF, "sizeof", "sizeof(%s)")
	opxx(ASSIGN, "=", "%s = %s")
	opxx(ADD_ASSIGN, "+=", "%s += %s")
	opxx(SUB_ASSIGN, "-=", "%s -= %s")
	opxx(MUL_ASSIGN, "*=", "%s *= %s")
	opxx(DIV_ASSIGN, "/=", "%s /= %s")
	opxx(MOD_ASSIGN, "%=", "%s %= %s")
	opxx(AND_ASSIGN, "&=", "%s &= %s")
	opxx(OR_ASSIGN, "|=", "%s |= %s")
	opxx(XOR_ASSIGN, "^=", "%s ^= %s")
	opxx(LSH_ASSIGN, "<<=", "%s <<= %s")
	opxx(RSH_ASSIGN, ">>=", "%s >>= %s")
	opxx(COMMA, ",", "%s , %s")

#endif