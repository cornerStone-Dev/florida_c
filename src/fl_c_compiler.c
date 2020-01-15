/* florida_c compiler */
/* PUBLIC DOMAIN */


#include <stdio.h>
#include <stdlib.h>   
#include <string.h>
#include <stdint.h>
#include <dirent.h>

#include "std_types.h"

#define NDEBUG
#define Parse_ENGINEALWAYSONSTACK

#define STRUCT_FOUND 0x01
#define ENUM_FOUND   0x02
#define UNION_FOUND  0x04

typedef struct Xoken Token;

typedef struct Xoken {
	u8 * s;
	u32  l;
	u32  flags;
} Token;

typedef struct{
	u8 * end;
	u8 * cursor_stack[64];
	u32  item_count_stack[64];
	u32  scopeIdx;
	u8   table[65536];
} ScopeList;

typedef struct parser_s{
	ScopeList * scopeList;
	u8 *        buff_start;
	u8 *        out;
	u8 *        decl_end;
	u8 *        funcProto_end;
	Token       type_name;
	Token       ident_type;
	Token       fnptr_ident;
	u8 *        dot_markers[550];
	u32         is_ptr[4096];
	u32         num_dots;
	u32         line_num;
	u32         func_start_line_num;
	u8          file_name_buff[512];
	u8          is_pub;
	u8          is_type;
	u8          is_struct;
	u8          is_union;
	u8          is_enum;
	u8          is_main;
	u8          is_custom_type;
	u8          inside_function;
	u8          printed_error;
	u8          local_macro;
	u8          is_inline;
} ParserState;

#include "../tool_output/fl_c_gram.h"
/* function prototypes */
static s32
add_to(ScopeList * restrict str_l, u8 * restrict s, u32  l);
static s32
is_within(ScopeList * restrict str_l, u8 * restrict s, u32  l);
static u8 *
indx_within(ScopeList * restrict str_l, u32  indx, u8 * restrict output);
static void
enter_scope(ScopeList * restrict scope_l);
static inline void
leave_scope(ScopeList * restrict scope_l);
static u8 *
add_restrict(u8 ** sp, u32 l, u8 * out, u8 * proto_end);
static u8
type_decl(ParserState * p_s, u8 * restrict s, u32  l, u8 is_pub);
/* end funtion prototypes */

/* globals */

static FILE * outputFile,* typeProtoFile, * typesFile,
	* funcProtoFile, * globalsFile, * interfaceFile, * includesFile; 

#include "../tool_output/fl_c_lex.c"
#include "../tool_output/fl_c_gram.c"

//#define INTPUT_FILE "input/weeklystat.c"
#define DEFAULT_DIR     "c_src/"
#define OUTPUT_FILE     "source.c"
#define TYPE_PROTO      "type_proto.h"
#define TYPES           "types.h"
#define FUNC_PROTO      "func_proto.h"
#define FL_STD_FILE     "fl_std.h"
#define FL_GLOBALS_FILE "globals.h"
#define INTERFACE_FILE  "interface.h"
#define INCLUDES_FILE   "includes.h"

int main(int argc, char **argv)
{
	
	unsigned char * data;
	void *pEngine;     /* The LEMON-generated LALR(1) parser */
	yyParser sEngine;  /* Space to hold the Lemon-generated Parser object */
	unsigned char output_string[65536] = {0};
	//unsigned char file_name_buff[512] = {0};
	unsigned char * output = output_string;
	u8 dirName[512];
	u8 * dirName_p;
	Token token = {0};
	ScopeList scopeList={0};
	int tmp_token;
	ParserState p_s = {0};
	FILE * pFile;
	size_t lSize;
	unsigned char * buffer;
	size_t result;
	DIR *d;
	struct dirent *dir;
	p_s.scopeList = &scopeList;
	p_s.out = output;
	p_s.buff_start = output_string;
	scopeList.cursor_stack[0]=scopeList.table;
	scopeList.end=&scopeList.table[65535];
	
	// end output fl_std file
	//~ printf("argc: %d\n", argc);
	//~ for (s32 x=0; x<argc; x++){
		//~ printf("%d:%s\n", x, argv[x]);
	//~ }
	
	if ( (argc > 1)
	  && (argv[1][0]=='-')
	  && (argv[1][1]=='d') 
	  && (strlen(&argv[1][2])<498) ) 
	{
		dirName_p = (u8*)stpcpy((char *)dirName, &argv[1][2]);
		dirName_p = (u8*)stpcpy((char *)dirName_p, "/");
	} else {
		dirName_p = (u8*)stpcpy((char *)dirName, DEFAULT_DIR);
	}
	
	// write out fl_std file
	strcpy((char *)dirName_p, FL_STD_FILE);
	typeProtoFile = fopen ( (const char *)dirName, "w" );
	if (typeProtoFile==NULL) {fputs ("File error",stderr); exit (1);}
	
	output = (uint8_t *)stpcpy(
				(char *)output, 
				FL_STD_HEADER);
	fwrite (output_string , sizeof(char), (output-output_string), typeProtoFile);
	fflush (typeProtoFile); 
	fclose (typeProtoFile);
	output = output_string;
	
	// open all output files
	strcpy((char *)dirName_p, TYPE_PROTO);
	typeProtoFile = fopen ( (const char *)dirName, "w" );
	if (typeProtoFile==NULL) {fputs ("File error",stderr); exit (1);}
	
	strcpy((char *)dirName_p, TYPES);
	typesFile = fopen ( (const char *)dirName, "w" );
	if (typeProtoFile==NULL) {fputs ("File error",stderr); exit (1);}
	
	strcpy((char *)dirName_p, FUNC_PROTO);
	funcProtoFile = fopen ( (const char *)dirName, "w" );
	if (typeProtoFile==NULL) {fputs ("File error",stderr); exit (1);}
	
	strcpy((char *)dirName_p, OUTPUT_FILE);
	outputFile = fopen ( (const char *)dirName, "w" );
	if (outputFile==NULL) {fputs ("File error",stderr); exit (1);}
	
	strcpy((char *)dirName_p, FL_GLOBALS_FILE);
	globalsFile = fopen ( (const char *)dirName, "w" );
	if (globalsFile==NULL) {fputs ("File error",stderr); exit (1);}
	
	strcpy((char *)dirName_p, INTERFACE_FILE);
	interfaceFile = fopen ( (const char *)dirName, "w" );
	if (interfaceFile==NULL) {fputs ("File error",stderr); exit (1);}
	
	strcpy((char *)dirName_p, INCLUDES_FILE);
	includesFile = fopen ( (const char *)dirName, "w" );
	if (includesFile==NULL) {fputs ("File error",stderr); exit (1);}
	
	/** Set up parser **/
	pEngine = &sEngine;
	ParseInit(pEngine, &p_s);
	
	/* open current directory */
	d = opendir("fl_src");
	if (d==0)
	{
		//printf("NO DIR!!!\n");
		return -1;
	}
	
	// insert includes into top of file
	output = (uint8_t *)stpcpy(
				(char *)output, 
				"#include \"fl_std.h\"\n");
	
	output = (uint8_t *)stpcpy(
				(char *)output, 
				"#include \"includes.h\"\n");
	
	output = (uint8_t *)stpcpy(
				(char *)output, 
				"#include \"type_proto.h\"\n");
	
	output = (uint8_t *)stpcpy(
				(char *)output, 
				"#include \"types.h\"\n");
	
	output = (uint8_t *)stpcpy(
				(char *)output, 
				"#include \"func_proto.h\"\n");
	
	output = (uint8_t *)stpcpy(
				(char *)output, 
				"#include \"globals.h\"\n");
	
	//printf("about to start\n");
	while ( ((dir = readdir(d)) != NULL) /*&& (strstr(dir->d_name, ".c")!=0)*/ )
	{
		//printf("Got in\n");
		if ( (strstr(dir->d_name, ".fl")!=0) ) {
		//printf("%s\n", dir->d_name);
		p_s.line_num=1;
		output = (uint8_t *)stpcpy((char *)output, "/* src/");
		output = (uint8_t *)stpcpy((char *)output, dir->d_name);
		output = (uint8_t *)stpcpy((char *)output, " */\n");
		fwrite (output_string,
				sizeof(char),
				output-output_string,
				outputFile);
		output = output_string;
		
		p_s.out = output;
		
		sprintf((char *)p_s.file_name_buff, "fl_src/%s", dir->d_name);

		pFile = fopen ( (char *)p_s.file_name_buff, "rb" );
		if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
		
		
		
		// obtain file size:
		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);

		// allocate memory to contain the whole file:
		buffer = (unsigned char*) malloc (sizeof(char)*lSize+1);
		if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}
		data = buffer;

		// copy the file into the buffer:
		result = fread (buffer,1,lSize,pFile);
		if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
		
		/* null terminate buffer */
		buffer[lSize]=0;
		
#ifndef NDEBUG
		ParseTrace(stdout, "debug:: ");
#endif
		//printf("starting parse\n");
		do {
			tmp_token = lex(&data, &token, &p_s.line_num);

			Parse(pEngine, tmp_token, token);
			
		} while (tmp_token != 0);
		
		

		fclose (pFile);
		/* free memory that stored copy of file */
		free (buffer);
		}
	}
	
	/* close directory */
	closedir(d);
	
	/* flush file out of cache and close all files */
	fflush (outputFile); 
	fclose (outputFile);
	fflush (typeProtoFile); 
	fclose (typeProtoFile);
	fflush (typesFile); 
	fclose (typesFile);
	fflush (funcProtoFile); 
	fclose (funcProtoFile);
	fflush (globalsFile); 
	fclose (globalsFile);
	fflush (interfaceFile); 
	fclose (interfaceFile);
	fflush (includesFile); 
	fclose (includesFile);
	
	
	/*** De-comission parser * */
	ParseFinalize(pEngine);
	s32 rcode;
	if(p_s.printed_error){
		rcode = 1;
	} else {
		rcode = 0;
	}
	return rcode;
}

static s32
add_to(ScopeList * restrict str_l, u8 * restrict s, u32  l)
{
	u8 * cursor = str_l->cursor_stack[str_l->scopeIdx];
	u8 * end =    str_l->end;
	
	if ( (end - cursor) < l )
	{
		printf ("ScopeList: out of room\n");
		return -1;
	}
	
	/* copy string to type_table */
	for (s32 i=0;i<l;i++)
	{
		*cursor = *s;
		cursor++;
		s++;
	}
	
	/* store length of string */
	*cursor = l;
	cursor++;
	
	
	//printf("*str_l->count =%d\n", str_l->item_count_stack[str_l->scopeIdx]);
	str_l->cursor_stack[str_l->scopeIdx] = cursor;
	str_l->item_count_stack[str_l->scopeIdx]+=1;
	
	return str_l->item_count_stack[str_l->scopeIdx]-1;
}

/* search type table for IDENT */
static s32
is_within(ScopeList * restrict str_l, u8 * restrict s, u32  l)
{
	u8 * cursor = str_l->cursor_stack[str_l->scopeIdx]-1;
	u8 * start =  str_l->table;
	u8 * string = s;
	u8 * len_cursor;
	s32  idx = str_l->item_count_stack[str_l->scopeIdx];
	u8   len;
	
	
	/* search backwards, view as a stack */
	while ( cursor >= start )
	{
		len = (*cursor);
		len_cursor = cursor;
		idx--;
		if ( l != len )
		{
			/* skip to next one */
			cursor = len_cursor-len-1;
			continue;
		}
		cursor-=len;
		/* matching length */
		for (s32 i=0;i<l;i++)
		{
			if ( (*cursor)!= (*s) )
			{
				/* skip to next string */
				cursor = len_cursor-len-1;
				/* reset string */
				s = string;
				break;
			}
			cursor++;
			s++;
		}
		/* see if success */
		if ( s != string )
		{
			//printf(" found something!!\n");
			return idx;
		}
	}
	/* we have found nothing */
	//printf(" found nothing!!\n");
	return -1;
}

/* search type table at index */
static u8 *
indx_within(ScopeList * restrict str_l, u32 indx, u8 * restrict output)
{
	u8 * cursor = str_l->cursor_stack[str_l->scopeIdx]-1;
	s32  idx = str_l->item_count_stack[str_l->scopeIdx];
	u8   len=0;
	
	if (indx > (idx-1) ) {
		return output;
	}
	
	/* one comparision */
	while ( idx > 0 )
	{
		len = (*cursor);
		idx--;
		if ( idx != indx )
		{
			/* skip to next one */
			cursor = cursor-len-1;
			continue;
		}
		break;
	}
	cursor-=len;
	/* copy out string */
	for (s32 i=0;i<len;i++)
	{
		*output = *cursor;
		cursor++;
		output++;
	}
	
	return output;
}

static void
enter_scope(ScopeList * restrict scope_l)
{
	u32 idx;
	scope_l->scopeIdx++;
	idx = scope_l->scopeIdx;
	/* copy end of previous scope to new scope */
	scope_l->item_count_stack[idx] = scope_l->item_count_stack[idx-1];
	scope_l->cursor_stack[idx] = scope_l->cursor_stack[idx-1];
	//printf("entering scope!%d\n",scope_l->scopeIdx);
}

static inline void
leave_scope(ScopeList * restrict scope_l)
{
	scope_l->scopeIdx--;
	//printf("leaving scope!%d\n",scope_l->scopeIdx);
}

static u8 *
add_restrict(u8 ** sp, u32 l, u8 * out, u8 * proto_end)
{
	u8 * s = *sp;
	// find open paren
	while (*s!='('){
		*out = *s;
		s++;
		out++;
	}
	// s == '('
	while (s!=proto_end) {
		if (*s=='*'){
			out = (uint8_t *)stpcpy((char *)out, "* restrict ");
			s++;
		} else {
		*out = *s;
		s++;
		out++;
		}
	}
	/* update for prototypes */
	*out = *s;
	s++;
	out++;
	*sp = s;
	return out;
}

static u8
type_decl(ParserState * p_s, u8 * restrict s, u32  l, u8 is_pub)
{
	u8 * endOfName;
	u8 * buff_p;
	u32 length;
	u8  buff[256];
	buff_p = buff;
	if (p_s->is_type) { //type to be typedef'ed
		p_s->is_type=0;
		p_s->out = (uint8_t *)stpcpy((char *)p_s->out, "typedef ");
		buff_p = (uint8_t *)stpcpy((char *)buff_p, "typedef ");
		if (p_s->is_struct){
			p_s->is_struct=0;
			p_s->out = (uint8_t *)stpcpy((char *)p_s->out, "struct ");
			buff_p = (uint8_t *)stpcpy((char *)buff_p, "struct ");
		} else if (p_s->is_enum) {
			p_s->is_enum=0;
			p_s->out = (uint8_t *)stpcpy((char *)p_s->out, "enum ");
			buff_p = (uint8_t *)stpcpy((char *)buff_p, "enum ");
		} else if (p_s->is_union) {
			p_s->is_union=0;
			p_s->out = (uint8_t *)stpcpy((char *)p_s->out, "union ");
			buff_p = (uint8_t *)stpcpy((char *)buff_p, "union ");
		}
		memcpy ( p_s->out, p_s->type_name.s, p_s->type_name.l );
		p_s->out += p_s->type_name.l;
		memcpy ( buff_p, p_s->type_name.s, p_s->type_name.l );
		buff_p += p_s->type_name.l;
		endOfName = (p_s->type_name.s+p_s->type_name.l);
		length = p_s->decl_end - endOfName;
		memcpy ( p_s->out, endOfName, length );
		p_s->out += length;
		// types
		memcpy ( p_s->out, p_s->type_name.s, p_s->type_name.l );
		p_s->out += p_s->type_name.l;
		p_s->out = (uint8_t *)stpcpy((char *)p_s->out, ";\n");
		fwrite (p_s->buff_start,
				sizeof(char),
				p_s->out-p_s->buff_start,
				typesFile);
		if (is_pub){ // declared public, export to interface header
			is_pub=0;
			fwrite (p_s->buff_start,
			sizeof(char),
			p_s->out-p_s->buff_start,
			interfaceFile);
		}
		p_s->out = p_s->buff_start;
		// type protos
		buff_p = (uint8_t *)stpcpy((char *)buff_p, " ");
		memcpy ( buff_p, p_s->type_name.s, p_s->type_name.l );
		buff_p += p_s->type_name.l;
		buff_p = (uint8_t *)stpcpy((char *)buff_p, ";\n");
		fwrite (buff , sizeof(char), buff_p-buff, typeProtoFile);
	} else { // global
		//printf("p_s->decl_end %ld s:%ld\n",p_s->decl_end, s);
		
		endOfName = s+l;
		//memcpy ( p_s->out, s, l );
		//p_s->out += l;
		if (p_s->decl_end>endOfName){ // not a macro
			if (is_pub==0){ // default is static "scoping"
				p_s->out = (uint8_t *)stpcpy((char *)p_s->out, "static ");
			}
			memcpy ( p_s->out, s, l );
			p_s->out += l;
			length = p_s->decl_end - endOfName;
			memcpy ( p_s->out, endOfName, length );
			p_s->out += length;
			p_s->out = (uint8_t *)stpcpy((char *)p_s->out, ";\n");
			fwrite (p_s->buff_start,
				sizeof(char),
				p_s->out-p_s->buff_start,
				globalsFile);
			if (is_pub){ // declared public, export to interface header
				is_pub=0;
				fwrite (p_s->buff_start,
				sizeof(char),
				p_s->out-p_s->buff_start,
				interfaceFile);
			}
			p_s->out = p_s->buff_start;
		} else { // this is a macro
			memcpy ( p_s->out, s, l );
			p_s->out += l;
			s++;
			u8 is_include;
			if( *s == 'i' 
			 && (*(s+1) == 'n')
			 && (*(s+2) == 'c') ) {
				is_include = 1;
			} else {
				is_include = 0;
			}
			// flags at play: is_pub, local_macro, is_include
			if (is_pub){ // declared public, export to interface header
				is_pub=0;
				fwrite (p_s->buff_start,
					sizeof(char),
					p_s->out-p_s->buff_start,
					interfaceFile);
			}
			if (p_s->local_macro){ // output to source e.g. ifdef on functions
				p_s->local_macro = 0;
				fwrite (p_s->buff_start,
					sizeof(char),
					p_s->out-p_s->buff_start,
					outputFile);
				p_s->out = p_s->buff_start;
			} else if (is_include==1) { // includes are put at the top
				fwrite (p_s->buff_start,
					sizeof(char),
					p_s->out-p_s->buff_start,
					includesFile);
				p_s->out = p_s->buff_start;
			} else { // macros are put into prototypes file to make "global"
				fwrite (p_s->buff_start,
					sizeof(char),
					p_s->out-p_s->buff_start,
					typeProtoFile);
				p_s->out = p_s->buff_start;
			}
		}
	}
	return is_pub;
}


