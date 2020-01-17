/* florida_c compiler */
/* PUBLIC DOMAIN */


#define MAKE_TOKEN() t->s = (u8 *)start;t->l = (u32)(YYCURSOR - start);t->flags = *line_num
/*!max:re2c*/                        // directive that defines YYMAXFILL (unused)
/*!re2c                              // start of re2c block
	
	mcm = "/*" ([^*] | ("*" [^/]))* "*""/"; // WILL NOT WORK ON "**/" ending!!!!
	scm = "//" [^\n]* "\n";
	wsp = ([ \t\v\n\r] | scm | mcm)+;
	macro = "#" ([^\n] | "\\\n")* "\n";
	local_macro = "#@" ([^\n] | "\\\n")* "\n";
	// integer literals
	oct = "0" [0-7]*;
	dec = [1-9][0-9]*;
	hex = '0x' [0-9a-fA-F]+;
	// floating literals
	frc = [0-9]* "." [0-9]+ | [0-9]+ ".";
	exp = 'e' [+-]? [0-9]+;
	flt = (frc exp? | [0-9]+ exp) [fFlL]?;
	string_lit = ["] ([^"] | ([\\] ["]))* ["];
	char_lit = ['] ([^'] | ([\\] [']))* ['];
	integer = oct | dec | hex;
	lblock =     "{";
	rblock =     "}";
	lparen =     "(";
	rparen =     ")";
	lbracket =   "[";
	rbracket =   "]";
	comma =      ",";
	star =       "*";
	atsign =     "@";
	dollar =     "$";
	semi =     ";";
	u8 =         "u8";
	s8 =         "s8";
	u16 =        "u16";
	s16 =        "s16";
	u32 =        "u32";
	s32 =        "s32";
	u64 =        "u64";
	s64 =        "s64";
	f32 =        "f32";
	f64 =        "f64";
	return =     "return";
	function =   "fn";
	e_id = "E_"[a-zA-Z_0-9]*;
	f_id = "F_"[a-zA-Z_0-9]*;
	s_id = "S_"[a-zA-Z_0-9]*;
	t_id = "T_"[a-zA-Z_0-9]*;
	u_id = "U_"[a-zA-Z_0-9]*;
	id = [a-zA-Z_][a-zA-Z_0-9]*;

	
*/                                   // end of re2c block

static int lex(/*const*/ u8 **YYCURSOR_p, Token * t, u32 * line_num) // YYCURSOR is defined as a function parameter
{                                    //
	u8 * YYMARKER;    // YYMARKER is defined as a local variable
	//const u8 * YYCTXMARKER; // YYCTXMARKER is defined as a local variable
	/*const*/ u8 * YYCURSOR;    // YYCURSOR is defined as a local variable
	/*const*/ u8 * start;
	start = *YYCURSOR_p;
	YYCURSOR = *YYCURSOR_p;

loop: // label for looping within the lexxer

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { start =YYCURSOR; goto loop; }//   default rule with its semantic action
	[\x00] { return 0; }             // EOF rule with null sentinal

	local_macro {
		*YYCURSOR_p = YYCURSOR;
		*start=' ';
		start++;
		*start='#';
		MAKE_TOKEN();
		while (start!=YYCURSOR){
			if(*start=='\n'){
				*line_num+=1;
				//printf("macro, %d\n",*line_num);
			}
			start++;
		}
		return LOCAL_MACRO;
	}

	macro {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		while (start!=YYCURSOR){
			//printf("%c",*start);
			if(*start=='\n'){
				*line_num+=1;
				//printf("macro, %d\n",*line_num);
			}
			start++;
		}
		return MACRO;
	}

	wsp {
		while (start!=YYCURSOR){
			if(*start=='\n'){
				*line_num+=1;
				//printf("wsp, %d\n", *line_num);
			}
			start++;
		}
		goto loop;
	}
 
	integer {
		*YYCURSOR_p = YYCURSOR;//MAKE_TOKEN();
		return INTEGER;
	}
	
	flt {
		*YYCURSOR_p = YYCURSOR;//MAKE_TOKEN();
		return FLOAT;
	}
	
	string_lit {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return STR_LIT;
	}

	char_lit {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return CHAR_LIT;
	}

	lblock {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return LBLOCK;
	}
	
	rblock {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return RBLOCK;
	}
	
	lparen { 
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return LPAREN;
	}
	
	rparen {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return RPAREN;
	}
	
	lbracket {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return LBRACKET;
	}
	
	rbracket {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return RBRACKET;
	}
	
	comma { 
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return COMMA;
	}
	
	star {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return STAR;
	}
	
	atsign {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		*start='&';
		return ATSIGN;
	}
	
	dollar {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		*start='*';
		return DOLLAR;
	}

	semi {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return SEMI;
	}

	u8 {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return U8;
	}

	s8 {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return S8;
	}

	u16 {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return U16;
	}

	s16 {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return S16;
	}

	u32 {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return U32;
	}

	s32 {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return S32;
	}

	u64 {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return U64;
	}

	s64 {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return S64;
	}

	f32 {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return F32;
	}

	f64 {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return F64;
	}

	"=" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return ASSIGN;
	}

	"?" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return QMARK;
	}

	":" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return COLON;
	}

	"||" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return LOGIC_OR;
	}

	"&&" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return LOGIC_AND;
	}

	"&" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return AND;
	}

	"^" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return XOR;
	}

	"|" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return OR;
	}

	"==" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return EQUALS;
	}

	"!=" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return NOTEQUALS;
	}

	"<" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return LT;
	}

	">" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return GT;
	}

	"<=" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return LTEQ;
	}

	">=" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return GTEQ;
	}

	"<<" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return LBITSHIFT;
	}

	">>" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return RBITSHIFT;
	}

	"+" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return PLUS;
	}

	"-" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return MINUS;
	}

	"/" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return DIVI;
	}

	"%" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return MOD;
	}

	//~ "++" {
		//~ *YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		//~ return INCREMENT;
	//~ }

	//~ "--" {
		//~ *YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		//~ return DECREMENT;
	//~ }

	"." {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return DOT;
	}

	"..." {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return DOTDOTDOT;}

	"*=" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  MULASSIGN ;}
	"/=" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  DIVASSIGN ;}
	"%=" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  MODASSIGN ;}
	"+=" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  ADDASSIGN ;}
	"-=" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  SUBASSIGN ;}
	"<<=" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return LSHASSIGN ;}
	">>=" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return RSHASSIGN ;}
	"&=" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  ANDASSIGN ;}
	"^=" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  XORASSIGN ;}
	"|=" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  ORASSIGN  ;}
	"~" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  TILDA  ;}
	"!" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  NOT ;}
	"->" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  ARROW ;}

	"persist" {
		*YYCURSOR_p = YYCURSOR;
		*start=' ';
		start++;
		MAKE_TOKEN();
		*start='s';
		start++;
		*start='t';
		start++;
		*start='a';
		start++;
		*start='t';
		start++;
		*start='i';
		start++;
		*start='c';
		return  PERSIST ;
	}
	//"auto" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  AUTO ;}
	//"register" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  REGISTER ;}
	//"static" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  STATIC ;}
	//"extern" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  EXTERN ;}
	
	"pub" {*YYCURSOR_p = YYCURSOR;start+=2;*start=' ';MAKE_TOKEN();return  PUB;}
	//"typedef" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  TYPEDEF ;}
	"union" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  UNION ;}
	"case" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  CASE ;}
	"default" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  DEFAULT ;}
	"if" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  IF ;}
	"else" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  ELSE ;}
	"switch" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  SWITCH ;}
	"while" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  WHILE ;}
	"do" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  DO ;}
	"for" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  FOR ;}
	"goto" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  GOTO ;}
	"continue" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  CONTINUE ;}
	"break" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  BREAK ;}
	"enum" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  ENUM ;}
	"alignOf" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  ALIGNOF ;}
	"bool" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  BOOL ;}
	//"restrict" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  RESTRICT ;}
	"atomic" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  ATOMIC ;}
	"inline" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  INLINE ;}
	"noReturn" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  NORETURN ;}
	"alignAs" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  ALIGNAS ;}
	"complex" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  COMPLEX ;}
	"imaginary" {*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();return  IMAGINARY ;}

	"sizeof" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return SIZEOF;
	}

	"void" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return VOID;
	}

	"volatile" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return VOLATILE;
	}

	"const" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return CONST;
	}

	"struct" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return STRUCT;
	}
	
	"fnptr" {
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return FNPTR;
	}

	return {
		// record string 
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return RETURN;
	}
	
	e_id {
		// record string 
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return E_ID;
	}
	
	f_id {
		// record string 
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return F_ID;
	}
	
	s_id {
		// record string 
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return S_ID;
	}

	t_id {
		// record string 
		*start=' ';
		start++;
		*start=' ';
		start++;
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return T_ID;
	}

	u_id {
		// record string 
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return U_ID;
	}

	id {
		// record string 
		*YYCURSOR_p = YYCURSOR;MAKE_TOKEN();
		return IDENT;
	}
	*/                               // end of re2c block
}  



