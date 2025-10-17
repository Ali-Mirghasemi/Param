/**
 * @file ParamMacro.h
 * @author Ali Mirghasemi (ali.mirghasemi1376@gmail.com)
 * @brief This is helper macros for param
 * @version 0.1
 * @date 2025-06-09
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef _PARAM_MACO_H_
#define _PARAM_MACO_H_

// ------------------------------------------- Detect System ByteOrder --------------------------------------
#if PARAM_SYSTEM_BYTE_ORDER == PARAM_SYSTEM_BYTE_ORDER_AU

/* canonical values */
#define ENDIAN_LITTLE 1234
#define ENDIAN_BIG    4321

/* --------------------------------------------------------------------
 * 1) Try widely-supported compiler/OS macros (GCC, Clang, glibc, musl, ...)
 * ------------------------------------------------------------------*/

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
  /* GCC / Clang define these on many systems */
  #if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    #define DETECTED_ENDIAN ENDIAN_LITTLE
  #elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    #define DETECTED_ENDIAN ENDIAN_BIG
  #endif
#endif

/* glibc / BSD style headers may define __BYTE_ORDER or _BYTE_ORDER, try guarded includes */
#if !defined(DETECTED_ENDIAN)
  /* try known endian headers if available */
  #if defined(__has_include)
    #if __has_include(<endian.h>)
      #include <endian.h>
    #elif __has_include(<sys/endian.h>)
      #include <sys/endian.h>
    #elif __has_include(<machine/endian.h>)
      #include <machine/endian.h>
    #endif
  #endif

  /* after including, some platforms define __BYTE_ORDER or _BYTE_ORDER or BIG/LITTLE macros */
  #if (defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && (__BYTE_ORDER == __LITTLE_ENDIAN)) \
   || (defined(_BYTE_ORDER) && defined(_LITTLE_ENDIAN) && (_BYTE_ORDER == _LITTLE_ENDIAN))
    #define DETECTED_ENDIAN ENDIAN_LITTLE
  #elif (defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && (__BYTE_ORDER == __BIG_ENDIAN)) \
    || (defined(_BYTE_ORDER) && defined(_BIG_ENDIAN) && (_BYTE_ORDER == _BIG_ENDIAN))
    #define DETECTED_ENDIAN ENDIAN_BIG
  #endif
#endif

/* --------------------------------------------------------------------
 * 2) Common target/architecture/compiler macros
 *    (MSVC/Windows, MinGW, x86/x64, ARM, MIPS, PowerPC, SPARC...)
 * ------------------------------------------------------------------*/
#if !defined(DETECTED_ENDIAN)

  /* Windows is little-endian on mainstream x86/x64/ARM Windows builds */
  #if defined(_WIN32) || defined(_WIN64)
    /* Historically Windows only runs on little-endian (x86/x64/ARM little). */
    #define DETECTED_ENDIAN ENDIAN_LITTLE
  #endif

#endif

#if !defined(DETECTED_ENDIAN)
  /* x86 and x86-64 compilers (GCC/Clang/MSVC/ICC) */
  #if defined(__i386__) || defined(__i386) || defined(i386) \
   || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_IX86)
    #define DETECTED_ENDIAN ENDIAN_LITTLE
  #endif
#endif

#if !defined(DETECTED_ENDIAN)
  /* ARM: __ARMEL__ = little, __ARMEB__ = big for some toolchains */
  #if defined(__ARMEL__) || defined(__ARM__ ) && defined(__ARMEL__) /* common */
    #define DETECTED_ENDIAN ENDIAN_LITTLE
  #elif defined(__ARMEB__)
    #define DETECTED_ENDIAN ENDIAN_BIG
  #endif
#endif

#if !defined(DETECTED_ENDIAN)
  /* ARM compilers (Keil/ARMCC/ARMCLANG) */
  #if defined(__ARMCC_VERSION) || defined(__ARMCLANG_VERSION)
    /* ARM targets are frequently little-endian; check more macros if provided */
    #if defined(__ARMEL__) || defined(__ARM_LITTLE_ENDIAN) || defined(__AARCH64EL__)
      #define DETECTED_ENDIAN ENDIAN_LITTLE
    #elif defined(__ARMEB__) || defined(__ARM_BIG_ENDIAN) || defined(__AARCH64EB__)
      #define DETECTED_ENDIAN ENDIAN_BIG
    #endif
  #endif
#endif

#if !defined(DETECTED_ENDIAN)
  /* IAR Embedded Workbench */
  #if defined(__IAR_SYSTEMS_ICC__)
    /* IAR defines __LITTLE_ENDIAN__ on some targets; check common macros */
    #if defined(__LITTLE_ENDIAN__) || defined(__little_endian__) || defined(__ARMEL__)
      #define DETECTED_ENDIAN ENDIAN_LITTLE
    #elif defined(__BIG_ENDIAN__) || defined(__big_endian__)
      #define DETECTED_ENDIAN ENDIAN_BIG
    #endif
  #endif
#endif

#if !defined(DETECTED_ENDIAN)
  /* Cosmic C (for STM8/ST7/Cosmic families) - many embed targets are little endian but check obvious macros */
  #if defined(__CSMC__) || defined(__CROSSWORKS) || defined(__C51__) /* cosmic variants */
    /* No universally standard macro — leave to runtime fallback unless toolchain sets something */
  #endif
#endif

#if !defined(DETECTED_ENDIAN)
  /* MIPS */
  #if defined(__MIPSEB__) || defined(__MIPSEB) || defined(_MIPSEB)
    #define DETECTED_ENDIAN ENDIAN_BIG
  #elif defined(__MIPSEL__) || defined(__MIPSEL) || defined(_MIPSEL)
    #define DETECTED_ENDIAN ENDIAN_LITTLE
  #endif
#endif

#if !defined(DETECTED_ENDIAN)
  /* PowerPC / PPC */
  #if defined(__ppc__) || defined(__powerpc__) || defined(__PPC__)
    /* PPC historically big-endian on some platforms, but many are bi-endian - try macros */
    #if defined(__BIG_ENDIAN__) || defined(__ppc64__) /* platform dependent */
      #define DETECTED_ENDIAN ENDIAN_BIG
    #else
      /* prefer runtime detection if unsure */
    #endif
  #endif
#endif

#if !defined(DETECTED_ENDIAN)
  /* SPARC */
  #if defined(__sparc) || defined(__sparc__)
    #define DETECTED_ENDIAN ENDIAN_BIG
  #endif
#endif

/* --------------------------------------------------------------------
 * 3) Try compiler-generic macros that sometimes exist
 * ------------------------------------------------------------------*/
#if !defined(DETECTED_ENDIAN)
  #if defined(__ORDER_LITTLE_ENDIAN__) && defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    #define DETECTED_ENDIAN ENDIAN_LITTLE
  #elif defined(__ORDER_BIG_ENDIAN__) && defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    #define DETECTED_ENDIAN ENDIAN_BIG
  #endif
#endif

#if !defined(DETECTED_ENDIAN)
  #if defined(__LITTLE_ENDIAN__) || defined(_LITTLE_ENDIAN) || defined(__LITTLE_ENDIAN)
    #define DETECTED_ENDIAN ENDIAN_LITTLE
  #elif defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN) || defined(__BIG_ENDIAN)
    #define DETECTED_ENDIAN ENDIAN_BIG
  #endif
#endif

/* --------------------------------------------------------------------
 * 4) Use Default config if not foudn any
 * ------------------------------------------------------------------*/
#if !defined(DETECTED_ENDIAN)
    #define DETECTED_ENDIAN ENDIAN_LITTLE
#endif

/* --------------------------------------------------------------------
 * 5) Update main defines
 * ------------------------------------------------------------------*/
#if defined(PARAM_SYSTEM_BYTE_ORDER)
    #undef PARAM_SYSTEM_BYTE_ORDER
#endif

#if DETECTED_ENDIAN == ENDIAN_BIG
    #define PARAM_SYSTEM_BYTE_ORDER PARAM_SYSTEM_BYTE_ORDER_BE
#else
    #define PARAM_SYSTEM_BYTE_ORDER PARAM_SYSTEM_BYTE_ORDER_LE
#endif

#endif // PARAM_SYSTEM_BYTE_ORDER == PARAM_SYSTEM_BYTE_ORDER_AU
// ----------------------------------------------------------------------------------------------------------

#define PARAM_VALUETYPE(NAME)                       __PARAM_VALUETYPE_(NAME)
#define __PARAM_VALUETYPE_(NAME)                    __PARAM_VALUETYPE__(NAME)
#define __PARAM_VALUETYPE__(NAME)                   __PARAM_VALUETYPE_ ##NAME

#define PARAM_IMPL_VALUETYPE(NAME)                  __PARAM_IMPL_VALUETYPE_(NAME)
#define __PARAM_IMPL_VALUETYPE_(NAME)               __PARAM_IMPL_VALUETYPE__(NAME)

#if PARAM_SYSTEM_BYTE_ORDER == PARAM_SYSTEM_BYTE_ORDER_LE
    #define __PARAM_IMPL_VALUETYPE__(NAME)          PARAM_VALUETYPE(NAME)   NAME
#else
    #define __PARAM_IMPL_VALUETYPE__(NAME)          struct { uint8_t __pad_ ##NAME [sizeof(Param_Number) - sizeof(PARAM_VALUETYPE(NAME))]; PARAM_VALUETYPE(NAME)   NAME; }
#endif


#define __PARAM_VALUETYPE_UNKNOWN                   char*
#define __PARAM_VALUETYPE_NULL                      char*
#define __PARAM_VALUETYPE_NUMBER                    Param_Number
#define __PARAM_VALUETYPE_UNUMBER                   Param_UNumber
#define __PARAM_VALUETYPE_NUMBERHEX                 Param_NumberHex
#define __PARAM_VALUETYPE_NUMBERBINARY              Param_NumberBin
#define __PARAM_VALUETYPE_UINT8                     uint8_t
#define __PARAM_VALUETYPE_UINT16                    uint16_t
#define __PARAM_VALUETYPE_UINT32                    uint32_t
#define __PARAM_VALUETYPE_UINT64                    uint64_t
#define __PARAM_VALUETYPE_INT8                      int8_t
#define __PARAM_VALUETYPE_INT16                     int16_t
#define __PARAM_VALUETYPE_INT32                     int32_t
#define __PARAM_VALUETYPE_INT64                     int64_t
#define __PARAM_VALUETYPE_FLOAT                     float
#define __PARAM_VALUETYPE_DOUBLE                    double
#define __PARAM_VALUETYPE_STATE                     uint8_t
#define __PARAM_VALUETYPE_STATEKEY                  uint8_t
#define __PARAM_VALUETYPE_BOOLEAN                   uint8_t
#define __PARAM_VALUETYPE_STRING                    char*

#define __PARAM_VALUETYPE_Unknown                   __PARAM_VALUETYPE_UNKNOWN
#define __PARAM_VALUETYPE_Null                      __PARAM_VALUETYPE_NULL
#define __PARAM_VALUETYPE_Number                    __PARAM_VALUETYPE_NUMBER
#define __PARAM_VALUETYPE_UNumber                    __PARAM_VALUETYPE_UNUMBER
#define __PARAM_VALUETYPE_NumberHex                 __PARAM_VALUETYPE_NUMBERHEX
#define __PARAM_VALUETYPE_NumberBinary              __PARAM_VALUETYPE_NUMBERBINARY
#define __PARAM_VALUETYPE_UInt8                     __PARAM_VALUETYPE_UINT8
#define __PARAM_VALUETYPE_UInt16                    __PARAM_VALUETYPE_UINT16
#define __PARAM_VALUETYPE_UInt32                    __PARAM_VALUETYPE_UINT32
#define __PARAM_VALUETYPE_UInt64                    __PARAM_VALUETYPE_UINT64
#define __PARAM_VALUETYPE_Int8                      __PARAM_VALUETYPE_INT8
#define __PARAM_VALUETYPE_Int16                     __PARAM_VALUETYPE_INT16
#define __PARAM_VALUETYPE_Int32                     __PARAM_VALUETYPE_INT32
#define __PARAM_VALUETYPE_Int64                     __PARAM_VALUETYPE_INT64
#define __PARAM_VALUETYPE_Float                     __PARAM_VALUETYPE_FLOAT
#define __PARAM_VALUETYPE_Double                    __PARAM_VALUETYPE_DOUBLE
#define __PARAM_VALUETYPE_State                     __PARAM_VALUETYPE_STATE
#define __PARAM_VALUETYPE_StateKey                  __PARAM_VALUETYPE_STATEKEY
#define __PARAM_VALUETYPE_Boolean                   __PARAM_VALUETYPE_BOOLEAN
#define __PARAM_VALUETYPE_String                    __PARAM_VALUETYPE_STRING

#endif // _PARAM_MACO_H_
