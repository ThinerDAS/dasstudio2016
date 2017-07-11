#ifndef EPRINTF_HEADER
#define EPRINTF_HEADER

#ifdef NDEBUG

#define eprintf(format, ...)

#else

#define eprintf(format, ...) fprintf(stderr, "[%s:%d] [%s] " format, __FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__)

#endif

#endif
