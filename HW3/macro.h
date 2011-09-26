#ifndef MACROS_H
#define MACROS_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//#define DEBUG

#ifdef DEBUG

#define LOG_BASE         1

#define L_FATAL          ( LOG_BASE << 0 )
#define L_GUESS          ( LOG_BASE << 1 )

#define LOG_EVERYTHING 0xFFFF // for convenience

#define LOG_LEVEL L_GUESS

static size_t _log_indent_level = 0;
#define LOG_INDENT _log_indent_level++;
#define LOG_UNINDENT ASSERT(_log_indent_level-- > 0, "Log indent level is now negative");
static const char* COLORS[ 5 ] = {
  "\e[1;91m", // FATAL bold red. 
  "\e[1;31m", // CRITICAL red.
  "\e[1;93m", // WARNING yellow.
  "\e[1;92m", // DEBUG green.
  "\e[0m"     // reset.
};

#define LOG(level, ...)                           \
  do {                                            \
    if(level & LOG_LEVEL) {                       \
      unsigned int i;                             \
      fprintf(stdout, "%s", COLORS[ 3 ]);         \
      for(i=0; i < _log_indent_level ; i++) {     \
        fprintf(stdout, "\t");                    \
      }                                           \
      fprintf(stdout, "[%ld] ", pthread_self());  \
      fprintf(stdout, __VA_ARGS__);               \
      fprintf(stdout, "\n");                      \
      fprintf(stdout, "%s", COLORS[ 4 ]);         \
    }                                             \
    else if( level & L_FATAL ) {                  \
      fprintf(stdout, "%s", COLORS[ 0 ]);         \
      fprintf(stdout, "[%ld] ", pthread_self());  \
      fprintf(stdout, __VA_ARGS__);               \
      fprintf(stdout, "\n");                      \
      fprintf(stdout, "%s", COLORS[ 4 ]);         \
    }                                             \
  } while(0);                                     \

#define ASSERT(test, msg)                             \
  if (!test) {                                        \
    LOG(L_FATAL, "#! ASSERT FAILED %s:%d:%s\n", __FILE__, __LINE__, #msg); \
    abort();                                          \
  }                                                   \

#define BP                                                        \
  fprintf(stdout, "%s", COLORS[ 2 ]);                             \
  fprintf(stdout, "Breakpoint in %s at %d\n", __FILE__,__LINE__); \
  getchar();                                                      \

#else // if not DEBUG

#define ASSERT(...);
#define LOG(...);
#define BP(...);

#endif

#define TEST_BOUND(x) (x >= 0 && x <= 1.0)
#define POSITIVE(x) (x >= 0)

#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

#endif /* MACROS_H */
