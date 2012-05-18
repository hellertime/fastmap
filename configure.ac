##
# libfastmap Autoconf script
#
# Author: Chris Heller <hellertime@gmail.com>
#

AC_INIT([Fastmap Library],[0.0.1],[hellertime@gmail.com],[libfastmap])

# Configure Autoconf
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([fastmap_config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([include/fastmap.h])

# Check for programs
AC_PROG_CC

# Check for system features
AC_C_BIGENDIAN
AC_SYS_LARGEFILE

# Check for headers
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h])

# Lots of automake warnings
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])

# *Required when using subdir-objects* 
AM_PROG_CC_C_O

# *Required when using -Werror*
AM_PROG_AR

AC_PROG_LIBTOOL

CFLAGS="$CFLAGS -Wall -Wextra -Werror -pedantic -Wstrict-aliasing=2 -Wno-missing-field-initializers"

##
# Normalize integer types as best as we can
AC_CHECK_HEADERS(inttypes.h stdint.h)

# Check for various integer types
#
AC_CHECK_TYPES([  int8_t,   int16_t,   int32_t,   int64_t])  # C99/BSD signed
AC_CHECK_TYPES([  int8,     int16,     int32,     int64])    # ??? signed
AC_CHECK_TYPES([ uint8_t,  uint16_t,  uint32_t,  uint64_t])  # C99 unsigned
AC_CHECK_TYPES([ uint8,    uint16,    uint32,    uint64])    # ??? unsigned
AC_CHECK_TYPES([u_int8_t, u_int16_t, u_int32_t, u_int64_t])  # BSD unsigned
AC_CHECK_TYPES([long long, unsigned long long, socklen_t])  # misc.

# Check the size of various types
#
AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int) 
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)
AC_CHECK_SIZEOF(float)
AC_CHECK_SIZEOF(double)
AC_CHECK_SIZEOF(long double)
AC_CHECK_SIZEOF(void *)
AC_CHECK_SIZEOF(size_t)

AH_VERBATIM([TG_INTEGER_TYPES],
[/* Get the C99 integer types */

#if defined(HAVE_INTTYPES_H)
#  include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#  include <stdint.h>
#endif

#ifndef HAVE_INT8_T
#  if defined(HAVE_INT8)
typedef int8 int8_t;
#  elif SIZEOF_CHAR == 1
typedef signed char int8_t;
#  else
#    error No usable signed 8-bit integer type found
#  endif
#endif /* ! HAVE_INT8_T */

#ifndef HAVE_INT16_T
#  if defined(HAVE_INT16)
typedef int16 int16_t;
#  elif SIZEOF_SHORT == 2
typedef short int16_t;
#  else
#    error No usable signed 16-bit integer type found
#  endif
#endif /* ! HAVE_INT16_T */

#ifndef HAVE_INT32_T
#  if defined(HAVE_INT32)
typedef int32 int32_t;
#  elif SIZEOF_INT == 4
typedef int int32_t;
#  elif SIZEOF_LONG == 4
typedef long int32_t;
#  else
#    error No usable signed 32-bit integer type found
#  endif
#endif /* ! HAVE_INT32_T */

#ifndef HAVE_INT64_T
#  if defined(HAVE_INT64)
typedef int64 int64_t;
#  elif SIZEOF_LONG == 8
typedef long int64_t;
#  elif defined(HAVE_LONG_LONG) && SIZEOF_LONG_LONG == 8
typedef long long int64_t;
#  else
#    error No usable signed 64-bit integer type found
#  endif
#endif /* ! HAVE_INT64_T */

#ifndef HAVE_UINT8_T
#  if defined(HAVE_UINT8)
typedef uint8 uint8_t;
#  elif defined(HAVE_U_INT8_T)
typedef u_int8_t uint8_t;
#  elif SIZEOF_CHAR == 1
typedef unsigned char uint8_t;
#  else
#    error No usable unsigned 8-bit integer type found
#  endif
#endif /* ! HAVE_UINT8_T */

#ifndef HAVE_UINT16_T
#  if defined(HAVE_UINT16)
typedef uint16 uint16_t;
#  elif defined(HAVE_U_INT16_T)
typedef u_int16_t uint16_t;
#  elif SIZEOF_SHORT == 2
typedef unsigned short uint16_t;
#  else
#    error No usable unsigned 16-bit integer type found
#  endif
#endif /* ! HAVE_UINT16_T */

#ifndef HAVE_UINT32_T
#  if defined(HAVE_UINT32)
typedef uint32 uint32_t;
#  elif defined(HAVE_U_INT32_T)
typedef u_int32_t uint32_t;
#  elif SIZEOF_INT == 4
typedef unsigned int uint32_t;
#  else
#    error No usable unsigned 32-bit integer type found
#  endif
#endif /* ! HAVE_UINT32_T */

#ifndef HAVE_UINT64_T
#  if defined(HAVE_UINT64)
typedef uint64 uint64_t;
#  elif defined(HAVE_U_INT64_T)
typedef u_int64_t uint64_t;
#  elif SIZEOF_LONG == 8
typedef unsigned long uint64_t;
#  elif defined(HAVE_UNSIGNED_LONG_LONG) && SIZEOF_LONG_LONG == 8
typedef unsigned long long uint64_t;
#  else
#    error No usable unsigned 64-bit integer type found
#  endif
#endif /* ! HAVE_UINT64_T */])

AH_VERBATIM([CONFIG_H_CHECK],
[/* Check that fastmap_config.h is the very first include */
#if defined(__GLIBC__)
#error fastmap_config.h was not the very first #include
#endif])


AC_OUTPUT([Makefile])
