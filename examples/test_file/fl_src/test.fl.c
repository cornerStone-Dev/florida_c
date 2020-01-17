
//u64 bigNum;

//~ u32 ParseFinalize(S_Engine pEngine){

    //~ ParseFinalize(pEngine);
    //~ return 0;
//~ }



/* here is the code start */

#include <stdio.h>
#include <stdlib.h>   
#include <string.h>
#include <dirent.h>

pub struct S_BigPapa {
	u32 trunk;
};

u32 add(s32 x, s32 y) 
{
	S_BigPapa hello;
	return x;
}

pub s32 total=10;

#define NDEBUG
#define Parse_ENGINEALWAYSONSTACK

#define STRUCT_FOUND 0x01
#define ENUM_FOUND   0x02
#define UNION_FOUND  0x04

struct S_Token{
	u8 $ s;
	u32  l;
	u32  flags;
};

struct S_Lexer{
	u8   type_table[4096];
	u8 $ cursor;
	u8 $ end;
};

fnptr s32 F_adder(s32, s32);

//~ typedef struct parser_s{
	//~ u8 * string_start;
	//~ u8 * string_end;
	//~ u32  qn_key;
//~ } ParserState;

struct S_ParserState{
	S_Lexer $ lexer;
#ifdef NDEBUG
	u8 $ start_attr;
#endif
	s32 func_found;
	s32 begin_attrs;
};

struct S_yyParser{
	S_Lexer $ lexer;
	u8 $ start_attr;
	s32 func_found;
	s32 begin_attrs;
};

//#include "../tool_output/sub_lang_gram.h"
//#include "../gen/prototypes.h"

//#include "../tool_output/sub_lang_lex.c"
//#include "../tool_output/sub_lang_gram.c"

//#define INTPUT_FILE "input/weeklystat.c"
#define OUTPUT_FILE "gen/output.c"

pub s32 main(s32 argc, s8 $$argv)
{
	
	const u8 $ data;
	void $pEngine;     /* The LEMON-generated LALR(1) parser */
	S_yyParser sEngine;  /* Space to hold the Lemon-generated Parser object */
	u8 output_string[32768] = {0};
	u8 file_name_buff[512] = {0};
	u8 $ output = output_string;
	S_Token token = {0};
	S_Lexer lexer = {0};
	s32 tmp_token, lenc=9;
	S_ParserState p_s = {0};
	T_FILE $ pFile, $ outputFile;
	u64 lSize;
	u8 $ buffer;
	persist u64 result;
	T_DIR $d;
    T_struct T_dirent $ dir;
    p_s.lexer = @lexer;
    lexer.cursor = @lexer.type_table[0];
	lexer.end = @lexer.type_table[4095];
	
	add_to_type_table(@lexer, buffer, lenc);
	outputFile = fopen ( OUTPUT_FILE, "w" );
	if (outputFile==NULL) {fputs ("File error",stderr); exit (1);}
	
	/* Set up parser */
	pEngine = @sEngine;
	//ParseInit(pEngine, @p_s);
	
	/* open current directory */
	d = opendir("test");
	if (d==0)
    {
		//printf("NO DIR!!!\n");
		return -1;
	}
	
	//printf("about to start\n");
	while ( ((dir = readdir(d)) != NULL) /*&& (strstr(dir->d_name, ".c")!=0)*/ )
	{
		//printf("Got in\n");
		if ( (strstr(dir.d_name, ".sub")!=0) ) {
		//printf("%s\n", dir->d_name);
		output = (u8 $)stpcpy((s8 $)output, "/* src/");
		output = (u8 $)stpcpy((s8 $)output, dir.d_name);
		output = (u8 $)stpcpy((s8 $)output, " */\n");
		
		sprintf((s8 $)file_name_buff, "test/%s", dir.d_name);

		pFile = fopen ( (s8 $)file_name_buff, "rb" );
		if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
		
		
		
		// obtain file size:
		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);

		// allocate memory to contain the whole file:
		buffer = (u8$) malloc (sizeof(s8)*lSize+1);
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
			//tmp_token = lex(@data, @token, @lexer);

			//Parse(pEngine, tmp_token, token);
			
		} while (tmp_token != 0);
		
		

		fclose (pFile);
		/* free memory that stored copy of file */
		free (buffer);
		}
	}
	
	/* close directory */
	closedir(d);
	
	fwrite (output_string , sizeof(s8), strlen((const s8 $)output_string), outputFile);
	
	/* flush file out of cache and close both files */
	fflush (outputFile); 
	fclose (outputFile);
	
	/* De-comission parser */
	//ParseFinalize(pEngine);
	return 0;
}

pub s32 add_to_type_table(S_Lexer $ lexer, u8 $  s, u32  l)
{
	u8 $ cursor = lexer.cursor;
	u8 $ end =    lexer.end;
	
	if ( (end - cursor) < l )
	{
		printf ("Lex.typetable: out of room\n");
		return 0;
	}
	
	/* store length of string */
	$cursor = l;
	cursor+=1;
	
	/* copy string to type_table */
	for (s32 i=0;i<l;i+=1)
	{
		$cursor = $s;
		cursor+=1;
		s+=1;
	}
	
	lexer.cursor = cursor;
	
	return 0;
}
/* search type table for IDENT */
s32 is_type(S_Lexer $  lexer, u8 $  s, u32  l)
{
	u8 $ cursor = lexer.type_table;
	u8 $ string = s;
	u8 $ len_cursor;
	u8 len;
	
	/* one comparision */
	while ( ($cursor) != 0 )
	{
		len = ($cursor);
		len_cursor = cursor;
		if ( l != len )
		{
			/* skip to next one */
			cursor = len_cursor+len+1;
			continue;
		}
		cursor+=1;
		/* matching length */
		for (s32 i=0;i<l;i+=1)
		{
			if ( ($cursor)!= ($s) )
			{
				/* skip to next string */
				cursor = len_cursor+len+1;
				/* reset string */
				s = string;
				break;
			}
			cursor+=1;
			s+=1;
		}
		/* see if success */
		if ( s != string )
		{
			printf(" found something!!\n\n\n");
			return 1;
		}
	}
	/* we have found nothing */
	printf(" found nothing!!\n\n\n");
	return 0;
	
}


u32 $ secret(u32$ question){
	return question +1;
}

