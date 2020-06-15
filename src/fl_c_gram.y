/*
 * grammar for a c-like language that is context free
*/
/* florida_c compiler */
/* PUBLIC DOMAIN */

%include{
#define YYNOERRORRECOVERY 1
}
/* not active when YYNOERRORRECOVERY is defined */
/*%parse_failure {
	printf("parse_failure\n");
	p_s->error = 1;
}*/


%extra_context {ParserState * p_s}

%token_type   {Token}
%default_type {Token}


%left  COMMA.
%right ASSIGN MULASSIGN DIVASSIGN MODASSIGN ADDASSIGN SUBASSIGN LSHASSIGN RSHASSIGN ANDASSIGN XORASSIGN ORASSIGN.
%right QMARK.
%left  LOGIC_OR LOGIC_AND.
%left  EQUALS NOTEQUALS LT GT LTEQ GTEQ.
%left  OR XOR AND.
%left  LBITSHIFT RBITSHIFT.
%left  PLUS MINUS.
%left  STAR DIVI MOD.
%right DOLLAR ATSIGN SIZEOF NOT TILDA.
%left  LPAREN RPAREN LBRACKET RBRACKET DOT ARROW.

%syntax_error {
	u8 * start_of_line=TOKEN.s;
	u8 * end_of_line=TOKEN.s;
	
	if (!p_s->printed_error){
		p_s->printed_error=1;
		if (p_s->line_num>1) {
			while (*start_of_line!='\n'){
				start_of_line--;
			}
			start_of_line++;
		}
		while (*end_of_line!='\n'){
				end_of_line++;
		}
		end_of_line++;
		*end_of_line=0;
		
		printf("%s:%d: syntax error on token %d\n", p_s->file_name_buff, p_s->line_num, yymajor);
		printf("%s\n", start_of_line);
	}
	
	while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
}

translation_unit ::= external_declaration. {
	//printf("parse done, input exhausted\n");
}

external_declaration ::= external_declaration function_definition.
external_declaration ::= external_declaration declaration(A). { p_s->is_pub=type_decl(p_s, A.s, A.l, p_s->is_pub); }
external_declaration ::= function_definition.
external_declaration ::= declaration(A).{ p_s->is_pub=type_decl(p_s, A.s, A.l, p_s->is_pub); }

declaration(A) ::=  MACRO(B).{p_s->decl_end = B.s;A=B;}
declaration(A) ::=  LOCAL_MACRO(B).{p_s->decl_end = B.s;p_s->local_macro = 1;A=B;}
declaration(A) ::=  PUB MACRO(B).{p_s->decl_end = B.s;p_s->is_pub = 1;A=B;}
declaration(A) ::=  PUB LOCAL_MACRO(B).{p_s->decl_end = B.s;p_s->is_pub = 1;p_s->local_macro = 1;A=B;}
declaration(A) ::=  declaration_specifiers(B) init_declarator_list SEMI(C).{p_s->decl_end = C.s;A=B;}
declaration(A) ::=  declaration_specifiers(B) SEMI(C). {p_s->decl_end = C.s;A=B;}// no change to parser size

function_definition ::= FNPTR declaration_specifiers(C) funcptr_declarator SEMI(E).{
	u8  buff[256];
	u8 *buff_p;
	buff_p = buff;
	
	p_s->out = (uint8_t *)stpcpy((char *)p_s->out, "typedef ");
	buff_p = (uint8_t *)stpcpy((char *)buff_p, "typedef ");
	p_s->out = (uint8_t *)stpcpy((char *)p_s->out, "struct ");
	buff_p = (uint8_t *)stpcpy((char *)buff_p, "struct ");
	// name of new type for function
	memcpy ( p_s->out, p_s->fnptr_ident.s, p_s->fnptr_ident.l);
	p_s->out += p_s->fnptr_ident.l;
	memcpy ( buff_p, p_s->fnptr_ident.s, p_s->fnptr_ident.l);
	buff_p += p_s->fnptr_ident.l;
	// type prototype
	buff_p = (uint8_t *)stpcpy((char *)buff_p, " ");
	memcpy ( buff_p, p_s->fnptr_ident.s, p_s->fnptr_ident.l);
	buff_p += p_s->fnptr_ident.l;
	buff_p = (uint8_t *)stpcpy((char *)buff_p, ";\n");
	fwrite (buff , sizeof(char), buff_p-buff, typeProtoFile);
	
	// open up struct
	p_s->out = (uint8_t *)stpcpy((char *)p_s->out, "{ ");
	
	// return types
	u32 length = p_s->fnptr_ident.s - C.s;
	memcpy ( p_s->out, C.s, length );
	p_s->out += length;
	
	// name of new type for function
	p_s->out = (uint8_t *)stpcpy((char *)p_s->out, "(*f)");
	//~ p_s->out = (uint8_t *)stpcpy((char *)p_s->out, "(*");
	//~ memcpy ( p_s->out, p_s->fnptr_ident.s, p_s->fnptr_ident.l);
	//~ p_s->out += p_s->fnptr_ident.l;
	//~ p_s->out = (uint8_t *)stpcpy((char *)p_s->out, ")");
	
	length = E.s - (p_s->fnptr_ident.s+p_s->fnptr_ident.l)+1;
	
	memcpy ( p_s->out, (p_s->fnptr_ident.s+p_s->fnptr_ident.l), length );
	p_s->out += length;
	p_s->out = (u8 *)stpcpy((char *)p_s->out, "}");
	memcpy ( p_s->out, p_s->fnptr_ident.s, p_s->fnptr_ident.l);
	p_s->out += p_s->fnptr_ident.l;
	p_s->out = (u8 *)stpcpy((char *)p_s->out, ";\n");
	fwrite (p_s->buff_start,
			sizeof(char),
			p_s->out-p_s->buff_start,
			typesFile);
	
	if (p_s->is_pub){ // declared public, export to interface header
		p_s->is_pub=0;
		fwrite (p_s->buff_start,
		sizeof(char),
		p_s->out-p_s->buff_start,
		interfaceFile);
	}
	p_s->out = p_s->buff_start;
}

function_definition ::= declaration_specifiers(A) function_declarator(D) compound_statement(B).{
	
	u8 buff[256];
	// insert line number and file info
	p_s->out += 
		sprintf((char *)p_s->out,
		"#line %d \"%s\"\n", A.flags, p_s->file_name_buff);
	
	
	u8 * out_p=p_s->out;
	if (p_s->is_pub == 0){
		p_s->out = (uint8_t *)stpcpy((char *)p_s->out, "static ");
	}
	
	/*           end - start */
	u32 length = D.s - A.s;
	memcpy ( p_s->out, A.s, length );
	p_s->out += length;
	
	p_s->out = add_restrict(&D.s, D.l, p_s->out, p_s->funcProto_end);
	
	// func prototypes
	length = p_s->out - out_p;
	memcpy ( buff, out_p, length );
	if (p_s->is_inline) {
		p_s->is_inline=0;
		stpcpy((char *)&buff[length], " __attribute__((always_inline));\n");
	} else {
		stpcpy((char *)&buff[length], ";\n");
	}
	fwrite (buff,
			sizeof(char),
			strlen((const char *)buff),
			funcProtoFile);
	if (p_s->is_pub==1){
		p_s->is_pub=0;
		if (p_s->is_main==0) {
		fwrite (buff,
			sizeof(char),
			length+2,
			interfaceFile);
		} else {
			p_s->is_main=0;
		}
	}
	
	u8 * start = D.s;
	if ( p_s->num_dots > 0 ){ // there are some dots to fix
		
		for(int i = 0; i<p_s->num_dots; i++){
			length = p_s->dot_markers[i] - start; // one less than normal
			memcpy ( p_s->out, start, length );
			p_s->out += length;
			p_s->out = (u8 *)stpcpy((char *)p_s->out, "->");
			start=p_s->dot_markers[i]+1;
		}
	}
	
	
	length = B.s - start+1;
	
	memcpy ( p_s->out, start, length );
	p_s->out += length;
	p_s->out = (u8 *)stpcpy((char *)p_s->out, "\n");
	
	fwrite (p_s->buff_start,
			sizeof(char),
			p_s->out-p_s->buff_start,
			outputFile);
	p_s->out = p_s->buff_start;
	
	p_s->num_dots=0;
	leave_scope(p_s->scopeList);
	p_s->inside_function=0;
}

declaration_specifiers ::= declaration_specifier.
declaration_specifiers ::= declaration_specifiers declaration_specifier.
//declaration_specifiers ::= declaration_specifiers declaration_specifier.
//~ declaration_specifiers ::= declaration_specifier storage_class_specifier.
//~ declaration_specifiers ::= declaration_specifier type_specifier.
//~ declaration_specifiers ::= declaration_specifier type_qualifier.

declaration_specifier ::= storage_class_specifier.
declaration_specifier ::= type_specifier.
declaration_specifier ::= type_qualifier.
declaration_specifier ::= function_specifier.
declaration_specifier ::= alignment_specifier.

//~ storage_class_specifier ::= AUTO.
//~ storage_class_specifier ::= REGISTER.
//~ storage_class_specifier ::= STATIC.
//~ storage_class_specifier ::= EXTERN.
storage_class_specifier ::= PUB.{p_s->is_pub = 1;}
storage_class_specifier ::= PERSIST.

function_specifier ::= INLINE.{p_s->is_inline = 1;}
function_specifier ::= NORETURN.

alignment_specifier ::= ALIGNAS LPAREN type_name RPAREN.
alignment_specifier ::= ALIGNAS LPAREN constant_expression RPAREN.

struct_or_union_specifier ::= struct_or_union_def LBLOCK struct_declaration_list RBLOCK. {/*add_to_type_table(p_s->lexer, A.s, A.l);*/ //TODO change identifier
	p_s->is_type = 1;
}
struct_or_union_specifier ::= struct_or_union LBLOCK struct_declaration_list RBLOCK.
struct_or_union_specifier ::= struct_or_union_id(A). {p_s->is_custom_type=1;p_s->ident_type = A;}
//struct_or_union_specifier ::= struct_or_union identifier(A). {add_to_type_table(p_s->lexer, A.s, A.l);} //TODO change identifier

type_specifier ::= U8.
type_specifier ::= S8.
type_specifier ::= U16.
type_specifier ::= S16.
type_specifier ::= U32.
type_specifier ::= S32.
type_specifier ::= U64.
type_specifier ::= S64.
type_specifier ::= F32.
type_specifier ::= F64.
type_specifier ::= VOID.
type_specifier ::= BOOL.
type_specifier ::= T_ID(A). {p_s->is_custom_type=1;p_s->ident_type = A;}
type_specifier ::= F_ID.
//type_specifier ::= E_ID.
//type_specifier ::= struct_or_union_id.
type_specifier ::= struct_or_union_specifier.
type_specifier ::= enum_specifier.
type_specifier ::= COMPLEX.
type_specifier ::= IMAGINARY.
//type_specifier ::= TYPE_IDENT. //TODO change identifier

type_qualifier ::= CONST.
type_qualifier ::= VOLATILE.
type_qualifier ::= RESTRICT.
type_qualifier ::= ATOMIC.

struct_or_union_def ::= struct_or_union struct_or_union_id(A).{
	p_s->type_name = A;
	add_to(p_s->scopeList, A.s, A.l);
}

struct_or_union_id ::= S_ID.
struct_or_union_id ::= U_ID.

struct_or_union ::= STRUCT. {p_s->is_struct = 1;} // is within a struct
struct_or_union ::= UNION. {p_s->is_union = 1;} // is within a union

struct_declaration_list ::= struct_declaration.
struct_declaration_list ::= struct_declaration_list struct_declaration.

struct_declaration ::= specifier_qualifier_list struct_declarator_list SEMI. // different from standard
struct_declaration ::= MACRO.

//~ specifier_qualifier_list ::= specifier_qualifier_list type_specifier.
//~ specifier_qualifier_list ::= specifier_qualifier_list type_qualifier.
specifier_qualifier_list ::= type_specifier. // strict order, not standard
specifier_qualifier_list ::= type_qualifier type_specifier.
specifier_qualifier_list ::= alignment_specifier type_specifier.
specifier_qualifier_list ::= alignment_specifier type_qualifier type_specifier.

//~ specifier_qualifier ::= type_specifier.
//~ specifier_qualifier ::= type_qualifier.

struct_declarator_list ::= struct_declarator.
struct_declarator_list ::= struct_declarator_list COMMA struct_declarator.

struct_declarator ::= declarator.
struct_declarator ::= declarator COLON constant_expression.
struct_declarator ::= COLON constant_expression.

funcptr_declarator ::= pointer funcptr_direct_declarator.
funcptr_declarator ::= funcptr_direct_declarator.

funcptr_direct_declarator ::= F_ID(A) LPAREN parameter_list RPAREN.{p_s->fnptr_ident=A;}
funcptr_direct_declarator ::= F_ID(A) LPAREN parameter_list COMMA DOTDOTDOT RPAREN.{p_s->fnptr_ident=A;}
funcptr_direct_declarator ::= F_ID(A) LPAREN RPAREN.{p_s->fnptr_ident=A;}

function_declarator ::= pointer func_direct_declarator.
function_declarator ::= func_direct_declarator.

func_direct_declarator ::= direct_start parameter_list RPAREN(A).{p_s->funcProto_end=A.s;}
func_direct_declarator ::= direct_start parameter_list COMMA DOTDOTDOT RPAREN(A).{p_s->funcProto_end=A.s;}
func_direct_declarator ::= direct_start RPAREN(A).{p_s->funcProto_end=A.s;}

direct_start ::= identifier(A) LPAREN. {
	p_s->func_start_line_num=p_s->line_num;
	enter_scope(p_s->scopeList);
	p_s->inside_function=1;
	if ( (A.l == 4) 
	  && (*A.s     == 'm')
	  && (*(A.s+1) == 'a')
	  && (*(A.s+2) == 'i')
	  && (*(A.s+3) == 'n') ){
		  p_s->is_main=1;
	  }
	}

// (what type am I?)need to record if I am a struct|union and if so that string
// (what type am I within?) need to record myself as type.myName if I am in a struct|union
declarator ::= pointer direct_declarator(A). {
	u8 * buff_p;
	u32  is_type_idx, len, ident_idx;
	u8   buff[128];
	buff_p = buff;
	
	if (p_s->is_struct || p_s->is_union){// within struct or union
		// update name with type name and "."
		memcpy ( buff, p_s->type_name.s, p_s->type_name.l );
		buff_p += p_s->type_name.l;
		*buff_p='.';
		buff_p++;
		memcpy ( buff_p, A.s, A.l );
		buff_p += A.l;
		len = buff_p - buff;
		ident_idx = add_to(p_s->scopeList, buff, len);
		p_s->is_ptr[ident_idx]=0xC0000000;
	} else {
		ident_idx = add_to(p_s->scopeList, A.s, A.l);
		p_s->is_ptr[ident_idx]=0x80000000;
	}
	
	// check if we are a struct or union
	if (p_s->is_custom_type){
		p_s->is_custom_type=0;
		is_type_idx = is_within(p_s->scopeList, p_s->ident_type.s, p_s->ident_type.l);
		is_type_idx &=0x0FFFFFFF; // mask off -1 to prevent false positives
		// save off type index
		p_s->is_ptr[ident_idx]|=(is_type_idx|0x20000000);
	}
}
declarator ::= direct_declarator(A). {
	u8 * buff_p;
	u32  is_type_idx, len, ident_idx;
	u8   buff[128];
	buff_p = buff;
	
	if (p_s->is_struct || p_s->is_union){// within struct or union
		// update name with type name and "."
		memcpy ( buff, p_s->type_name.s, p_s->type_name.l );
		buff_p += p_s->type_name.l;
		*buff_p='.';
		buff_p++;
		memcpy ( buff_p, A.s, A.l );
		buff_p += A.l;
		len = buff_p - buff;
		ident_idx = add_to(p_s->scopeList, buff, len);
		p_s->is_ptr[ident_idx]=0x40000000;
	} else {
		ident_idx = add_to(p_s->scopeList, A.s, A.l);
		p_s->is_ptr[ident_idx]=0x00000000;
	}
	
	// check if we are a struct or union
	if (p_s->is_custom_type){
		p_s->is_custom_type=0;
		is_type_idx = is_within(p_s->scopeList, p_s->ident_type.s, p_s->ident_type.l);
		is_type_idx &=0x0FFFFFFF; // mask off -1 to prevent false positives
		//printf("recorded struct ident at %d with type %d\n",ident_idx, is_type_idx);
		// save off type index
		p_s->is_ptr[ident_idx]|=(is_type_idx|0x20000000);
	}
}

pointer ::= pointer_t.
pointer ::= pointer pointer_t.

pointer_t ::= DOLLAR.
pointer_t ::= DOLLAR type_qualifier. // could add second type_qualifier here
pointer_t ::= DOLLAR type_qualifier type_qualifier. 

direct_declarator ::= identifier.
//direct_declarator ::= LPAREN declarator RPAREN.
direct_declarator ::= direct_declarator LBRACKET assignment_expression RBRACKET.
direct_declarator ::= direct_declarator LBRACKET RBRACKET.
//direct_declarator ::= func_direct_declarator. // attempt
//direct_declarator ::= direct_declarator LPAREN parameter_list RPAREN.
//direct_declarator ::= direct_declarator LPAREN parameter_list COMMA DOTDOTDOT RPAREN.
//direct_declarator ::= direct_declarator LPAREN identifier RPAREN.
//direct_declarator ::= direct_declarator LPAREN identifier identifier_list RPAREN.
//direct_declarator ::= direct_declarator LPAREN RPAREN. // TODO add parameter list?

//constant_expression ::= conditional_expression.
constant_expression ::= constant_expression QMARK expression COLON constant_expression. //[QMARK]
constant_expression ::= constant_expression LOGIC_OR constant_expression.
constant_expression ::= constant_expression LOGIC_AND constant_expression.
constant_expression ::= constant_expression OR constant_expression.
constant_expression ::= constant_expression XOR constant_expression.
constant_expression ::= constant_expression AND constant_expression.
constant_expression ::= constant_expression EQUALS constant_expression.
constant_expression ::= constant_expression NOTEQUALS constant_expression.
constant_expression ::= constant_expression LT constant_expression.
constant_expression ::= constant_expression GT constant_expression.
constant_expression ::= constant_expression LTEQ constant_expression.
constant_expression ::= constant_expression GTEQ constant_expression.
constant_expression ::= constant_expression LBITSHIFT constant_expression.
constant_expression ::= constant_expression RBITSHIFT constant_expression.
constant_expression ::= constant_expression PLUS constant_expression.
constant_expression ::= constant_expression MINUS constant_expression.
constant_expression ::= constant_expression STAR constant_expression.
constant_expression ::= constant_expression DIVI constant_expression.
constant_expression ::= constant_expression MOD constant_expression.
constant_expression ::= cast_expression.
////////////////////////////////////////////////////////////////////////////////
cast_expression ::= unary_expression.
cast_expression ::= LPAREN type_name RPAREN cast_expression.

unary_expression ::= postfix_expression.
unary_expression ::= unary_operator cast_expression.
//unary_expression ::= unary_assignable_expression.
unary_expression ::= SIZEOF unary_expression.
unary_expression ::= SIZEOF LPAREN type_name RPAREN.
unary_expression ::= ALIGNOF LPAREN type_name RPAREN.

//unary_assignable_expression ::= postfix_expression.
//unary_assignable_expression ::= INCREMENT unary_expression.
//unary_assignable_expression ::= DECREMENT unary_expression.
//unary_assignable_expression ::= unary_operator cast_expression.

postfix_expression ::= primary_expression.
postfix_expression ::= postfix_expression LBRACKET expression RBRACKET.
postfix_expression ::= postfix_expression LPAREN argument_expression_list RPAREN.
postfix_expression ::= postfix_expression LPAREN RPAREN.
postfix_expression(C) ::= postfix_expression(B) DOT(A) identifier(D). { // work for removing arrow
	u8 * buff_p;
	u8   buff[128];
	s32  idx, len;
	// check if flag is set that a struct type was before us
	if (B.flags&0x20000000) { // flag was set
		// get that struct type
		buff_p = indx_within(p_s->scopeList, (B.flags&0x0FFFFFFF), buff);
		// append our identifier to type with "." and search
		*buff_p = 0;
		//printf("buff:%s\n", buff);
		*buff_p='.';
		buff_p++;
		memcpy ( buff_p, C.s, C.l );
		buff_p += C.l;
		len = buff_p - buff;
		*buff_p = 0;
		//printf("buff:%s\n", buff);
		idx = is_within(p_s->scopeList, buff, len);
	} else {
		idx = is_within(p_s->scopeList, B.s, B.l);
	}
	
	if ( (idx!=-1) && (p_s->is_ptr[idx]&0x80000000) ){
		//printf("recorded dot operator to fix\n");
		p_s->dot_markers[p_s->num_dots] = A.s;
		p_s->num_dots++;
	}
	
	//printf("search for struct ident at %d with val %08X\n",idx, is_ptr[idx]);
	
	// check if search shows us as an struct type
	if ((idx!=-1) && p_s->is_ptr[idx]&0x20000000){
		// set struct flag
		//printf("search shows struct ident\n");
		C.flags = p_s->is_ptr[idx]&0x2FFFFFFF;
		C.s=D.s; // record ident information
		C.l=D.l; // record ident information
	}
}
postfix_expression ::= postfix_expression ARROW identifier.
postfix_expression ::= LPAREN type_name RPAREN LBLOCK initializer_list RBLOCK. // anonymous decl
postfix_expression ::= LPAREN type_name RPAREN LBLOCK initializer_list COMMA RBLOCK. // anonymous decl

primary_expression ::= identifier.
primary_expression ::= constant.
primary_expression ::= STR_LIT.
primary_expression ::= LPAREN expression RPAREN.

constant ::= INTEGER.
constant ::= FLOAT.
constant ::= CHAR_LIT.
			 //~ | character_constant
			 //~ | floating_constant
			 //~ | enumeration_constant

expression ::= assignment_expression.
expression ::= expression COMMA assignment_expression.

argument_expression_list ::= assignment_expression.
argument_expression_list ::= argument_expression_list COMMA assignment_expression.

assignment_expression ::= constant_expression.
assignment_expression ::= unary_expression assignment_operator assignment_expression.

assignment_operator ::= ASSIGN.
assignment_operator ::= MULASSIGN.
assignment_operator ::= DIVASSIGN.
assignment_operator ::= MODASSIGN.
assignment_operator ::= ADDASSIGN.
assignment_operator ::= SUBASSIGN.
assignment_operator ::= LSHASSIGN.
assignment_operator ::= RSHASSIGN.
assignment_operator ::= ANDASSIGN.
assignment_operator ::= XORASSIGN.
assignment_operator ::= ORASSIGN.

unary_operator ::= ATSIGN. //get addr
unary_operator ::= DOLLAR. //get value of pointer
unary_operator ::= PLUS.
unary_operator ::= MINUS.
unary_operator ::= TILDA.
unary_operator ::= NOT.

type_name ::= specifier_qualifier_list abstract_declarator.
type_name ::= specifier_qualifier_list.

parameter_list ::= parameter_declaration.
parameter_list ::= parameter_list COMMA parameter_declaration.

parameter_declaration ::= declaration_specifiers declarator.
parameter_declaration ::= declaration_specifiers abstract_declarator.
parameter_declaration ::= declaration_specifiers.

abstract_declarator ::= pointer.
abstract_declarator ::= pointer direct_abstract_declarator.
abstract_declarator ::= direct_abstract_declarator.

direct_abstract_declarator ::= LPAREN abstract_declarator RPAREN.
direct_abstract_declarator ::= direct_abstract_declarator LBRACKET assignment_expression RBRACKET.
direct_abstract_declarator ::= direct_abstract_declarator LPAREN parameter_list COMMA DOTDOTDOT RPAREN.
direct_abstract_declarator ::= direct_abstract_declarator LPAREN parameter_list RPAREN.
direct_abstract_declarator ::= LBRACKET assignment_expression RBRACKET.
direct_abstract_declarator ::= LPAREN parameter_list COMMA DOTDOTDOT RPAREN.
direct_abstract_declarator ::= LPAREN parameter_list RPAREN.
direct_abstract_declarator ::= direct_abstract_declarator LBRACKET RBRACKET.
direct_abstract_declarator ::= direct_abstract_declarator LPAREN RPAREN.
direct_abstract_declarator ::= LBRACKET RBRACKET.
direct_abstract_declarator ::= LPAREN RPAREN.

enum_specifier ::= ENUM E_ID(A) LBLOCK enumerator_list RBLOCK.{
	p_s->type_name = A;
	p_s->is_enum = 1;
	p_s->is_type = 1;
}
enum_specifier ::= ENUM E_ID(A) LBLOCK enumerator_list COMMA RBLOCK.{
	p_s->type_name = A;
	p_s->is_enum = 1;
	p_s->is_type = 1;
	
}
enum_specifier ::= ENUM LBLOCK enumerator_list RBLOCK.
enum_specifier ::= ENUM LBLOCK enumerator_list COMMA RBLOCK.
enum_specifier ::= E_ID.

enumerator_list ::= enumerator.
enumerator_list ::= enumerator_list COMMA enumerator.

enumerator ::= identifier.
enumerator ::= identifier ASSIGN constant_expression.

init_declarator_list ::= init_declarator.
init_declarator_list ::= init_declarator_list COMMA init_declarator.

init_declarator ::= declarator.
init_declarator ::= declarator ASSIGN initializer.

initializer ::= assignment_expression.
initializer ::= LBLOCK initializer_list RBLOCK. //structs
initializer ::= LBLOCK initializer_list COMMA RBLOCK. //structs

initializer_list ::= initializer.
initializer_list ::= initializer_list COMMA initializer.
initializer_list ::= designation initializer.
initializer_list ::= initializer_list COMMA designation initializer.

designation ::= designator_list ASSIGN.

designator_list ::= designator.
designator_list ::= designator_list designator.

designator ::= LBRACKET constant_expression RBRACKET.
designator ::= DOT identifier.

compound_statement(A) ::= open_block block_item_list close_block(B).{A=B;}
compound_statement(A) ::= open_block close_block(B).{A=B;}

block_item_list ::= block_item.
block_item_list ::= block_item_list block_item.

block_item ::= declaration.
block_item ::= statement.

statement ::= labeled_statement.
statement ::= expression_statement.
statement ::= compound_statement.
statement ::= selection_statement.
statement ::= iteration_statement.
statement ::= jump_statement.

labeled_statement ::= identifier COLON statement.
labeled_statement ::= CASE constant_expression COLON statement.
labeled_statement ::= DEFAULT COLON statement.

expression_statement ::= expression SEMI.
expression_statement ::= SEMI.

selection_statement ::= if LPAREN expression RPAREN compound_statement. {leave_scope(p_s->scopeList);}
selection_statement ::= if LPAREN expression RPAREN compound_statement ELSE statement. {leave_scope(p_s->scopeList);}
selection_statement ::= switch LPAREN expression RPAREN compound_statement.  {leave_scope(p_s->scopeList);}

iteration_statement ::= while LPAREN expression RPAREN compound_statement. {leave_scope(p_s->scopeList);}
iteration_statement ::= DO compound_statement WHILE LPAREN expression RPAREN SEMI. // delta from standard
iteration_statement ::= for LPAREN expression SEMI expression SEMI expression RPAREN compound_statement. {leave_scope(p_s->scopeList);}
iteration_statement ::= for LPAREN SEMI expression SEMI expression RPAREN compound_statement. {leave_scope(p_s->scopeList);}
iteration_statement ::= for LPAREN expression SEMI SEMI expression RPAREN compound_statement. {leave_scope(p_s->scopeList);}
iteration_statement ::= for LPAREN expression SEMI expression SEMI RPAREN compound_statement. {leave_scope(p_s->scopeList);}
iteration_statement ::= for LPAREN SEMI SEMI expression RPAREN compound_statement. {leave_scope(p_s->scopeList);}
iteration_statement ::= for LPAREN expression SEMI SEMI RPAREN compound_statement. {leave_scope(p_s->scopeList);}
iteration_statement ::= for LPAREN SEMI expression SEMI RPAREN compound_statement. {leave_scope(p_s->scopeList);}
iteration_statement ::= for LPAREN SEMI SEMI RPAREN compound_statement. {leave_scope(p_s->scopeList);}
iteration_statement ::= for LPAREN declaration expression SEMI expression RPAREN compound_statement. {leave_scope(p_s->scopeList);}
iteration_statement ::= for LPAREN declaration expression SEMI RPAREN compound_statement. {leave_scope(p_s->scopeList);}
iteration_statement ::= for LPAREN declaration SEMI expression RPAREN compound_statement. {leave_scope(p_s->scopeList);}
iteration_statement ::= for LPAREN declaration SEMI RPAREN compound_statement. {leave_scope(p_s->scopeList);}

jump_statement ::= GOTO identifier SEMI.
jump_statement ::= CONTINUE SEMI.
jump_statement ::= BREAK SEMI.
jump_statement ::= RETURN expression SEMI.
jump_statement ::= RETURN SEMI.

//identifier_list ::= COMMA identifier.
//identifier_list ::= identifier_list COMMA identifier.

identifier ::= IDENT.

open_block ::= LBLOCK. {enter_scope(p_s->scopeList);}

close_block ::= RBLOCK. {leave_scope(p_s->scopeList);}

while ::= WHILE. {enter_scope(p_s->scopeList);}
for ::= FOR. {enter_scope(p_s->scopeList);}
if ::= IF. {enter_scope(p_s->scopeList);}
switch ::= SWITCH. {enter_scope(p_s->scopeList);}

//~ decl ::= type IDENT SEMI.
//~ decl ::= PUB type IDENT SEMI.
//~ decl ::= STRUCT IDENT LBLOCK decl_list RBLOCK. //direct output to types.h? need prototypes of types too

//~ decl_list ::= decl_list param SEMI.
//~ decl_list ::= param SEMI.

//~ func_def ::=  fn_scope type IDENT args LBLOCK function_body RBLOCK. {
	//~ //printf("type accepted\n");
	//~ p_s->func_found = 1;
//~ }


//~ fn_scope ::= FUNCTION.
//~ fn_scope ::= PUB FUNCTION.

//~ function_body ::= RETURN IDENT SEMI.

//~ args ::= LPAREN paramlist RPAREN.
//~ args ::= LPAREN RPAREN.

//~ paramlist ::= paramlist COMMA param.
//~ paramlist ::= param.

//~ param ::= type IDENT.


