/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */

/* Define if you want a C++ callstack to be displayed after crashing */
/* #undef ENABLE_CALLSTACK */

/* Define if you want to use new_allocator */
/* #undef ENABLE_NEW_ALLOCATOR */

/* The system has gethostbyname_r which takes a result as a pass-in param */
/* #undef GETHOSTBYNAME_R_INPUTS_RESULT */

/* __gnu_cxx::bitmap_allocator exists */
#define HAVE_BITMAP_ALLOCATOR 1

/* Define if you have boost framework */
/* #undef HAVE_BOOST */

/* boost::fast_pool_allocator exists */
#define HAVE_BOOST_FAST_POOL_ALLOCATOR 1

/* boost::pool_allocator exists */
#define HAVE_BOOST_POOL_ALLOCATOR 1

/* whether or not we have to demangle names */
/* #undef HAVE_CPLUS_DEMANGLE */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* found ftime */
#define HAVE_FTIME 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if you have java */
/* #undef HAVE_JAVA */

/* Define to 1 if you have the `compat' library (-lcompat). */
/* #undef HAVE_LIBCOMPAT */

/* Define to 1 if you have the `c_r' library (-lc_r). */
/* #undef HAVE_LIBC_R */

/* Define to 1 if you have the `nsl' library (-lnsl). */
/* #undef HAVE_LIBNSL */

/* Define to 1 if you have the `pthread' library (-lpthread). */
/* #undef HAVE_LIBPTHREAD */

/* Define to 1 if you have the `rt' library (-lrt). */
/* #undef HAVE_LIBRT */

/* Define to 1 if you have the `socket' library (-lsocket). */
/* #undef HAVE_LIBSOCKET */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* __gnu_cxx::mt_allocator exists */
#define HAVE_MT_ALLOCATOR 1

/* Define if you have sql library (-lmysqlclient) */
/* #undef HAVE_MYSQL */

/* __gnu_cxx::pool_allocator exists */
#define HAVE_POOL_ALLOCATOR 1

/* Define if you have sql library (-lpq) */
/* #undef HAVE_POSTGRESQL */

/* Define if you have python */
/* #undef HAVE_PYTHON */

/* Define if you have ruby */
/* #undef HAVE_RUBY */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define if you have stlport library */
/* #undef HAVE_STLPORT */

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "quickfix"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* select statement decrements timeval parameter if interupted */
/* #undef SELECT_MODIFIES_TIMEVAL */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* For location of set_terminate */
#define TERMINATE_IN_STD 1

/* Whether or not we are using the new-style typeinfo header */
#define TYPEINFO_IN_STD 1

/* The system supports AT&T STREAMS */
/* #undef USING_STREAMS */

/* Version number of package */
#define VERSION "1.13.2"

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
#define YYTEXT_POINTER 1

/* enable reentrant system calls */
#define _REENTRANT 1

/* socklen_t needs to be defined if the system doesn't define it */
/* #undef socklen_t */
