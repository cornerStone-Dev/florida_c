typedef struct S_BigPapa {
	u32 trunk;
}S_BigPapa;
typedef struct S_Token{
	u8 * s;
	u32  l;
	u32  flags;
}S_Token;
typedef struct S_Lexer{
	u8   type_table[4096];
	u8 * cursor;
	u8 * end;
}S_Lexer;
typedef s32 (*F_adder)(s32, s32);
typedef struct S_ParserState{
	S_Lexer * lexer;
#ifdef NDEBUG
	u8 * start_attr;
#endif
	s32 func_found;
	s32 begin_attrs;
}S_ParserState;
typedef struct S_yyParser{
	S_Lexer * lexer;
	u8 * start_attr;
	s32 func_found;
	s32 begin_attrs;
}S_yyParser;
