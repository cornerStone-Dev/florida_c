#include <stdio.h>
#include <stdlib.h>   
#include <string.h>
#include <dirent.h>
typedef struct S_BigPapa S_BigPapa;
#define NDEBUG
#define Parse_ENGINEALWAYSONSTACK
#define STRUCT_FOUND 0x01
#define ENUM_FOUND   0x02
#define UNION_FOUND  0x04
typedef struct S_Token S_Token;
typedef struct S_Lexer S_Lexer;
typedef struct S_ParserState S_ParserState;
typedef struct S_yyParser S_yyParser;
#define OUTPUT_FILE "gen/output.c"
