/***
 * @file assert.h
 * @author Lhxl
 * @data 2024-12-28
 * @version build30
 * @brief 断言
 ***/

/* assert */
#define ASSERT
#ifdef ASSERT
void assertion_failure(char* exp, char* file, int line);
#define assert(exp) if(exp); else assertion_failure(#exp, __FILE__, __LINE__)
#else
#define assert(exp)
#endif