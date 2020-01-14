/* new standard types */

typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint64_t u64;
typedef int64_t  s64;
typedef float    f32;
typedef double   f64;

#define alignOf  _Alignof
#define bool     _Bool
#define atomic   _Atomic
#define noReturn _Noreturn
#define alignAs  _Alignas

#define FL_STD_HEADER \
"#include <stdint.h>\n"\
"#include <stdbool.h>\n"\
"typedef uint8_t   u8;\n"\
"typedef int8_t    s8;\n"\
"typedef uint16_t  u16;\n"\
"typedef int16_t   s16;\n"\
"typedef uint32_t  u32;\n"\
"typedef int32_t   s32;\n"\
"typedef uint64_t  u64;\n"\
"typedef int64_t   s64;\n"\
"typedef float     f32;\n"\
"typedef double    f64;\n"\
"#define alignOf   _Alignof\n"\
"#define atomic    _Atomic\n"\
"#define noReturn  _Noreturn\n"\
"#define complex   _Complex\n"\
"#define imaginary _Imaginary\n"\
"#define alignAs   _Alignas\n"


