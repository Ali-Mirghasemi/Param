/**
 * @file Param.h
 * @author Ali Mirghasemi (ali.mirghasemi1376@gmail.com)
 * @brief This library help you to parse string parameters.
 * @version 0.1
 * @date 2022-06-22
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef _PARAM_H_
#define _PARAM_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define PARAM_VER_MAJOR       0
#define PARAM_VER_MINOR       2
#define PARAM_VER_FIX         0

#include "Str.h"
#include <stdint.h>

/********************************************************************************************/
/*                                     Configuration                                        */
/********************************************************************************************/
#define PARAM_CASE_LOWER                0x01
#define PARAM_CASE_HIGHER               0x02
#define PARAM_CASE_INSENSITIVE          0x03
/**
 * @brief define the case mode of parameter name, just for state, null, stateKey, boolean
 */
#define PARAM_CASE_MODE                 PARAM_CASE_INSENSITIVE
/**
 * @brief enable value type number
 */
#define PARAM_TYPE_NUMBER               1
/**
 * @brief enable value type number hex
 */
#define PARAM_TYPE_NUMBER_HEX           1
/**
 * @brief enable value type number hex
 */
#define PARAM_TYPE_NUMBER_BINARY        1
/**
 * @brief enable value type float
 */
#define PARAM_TYPE_FLOAT                1
/**
 * @brief enable value type state
 */
#define PARAM_TYPE_STATE                1
/**
 * @brief enable value type state
 */
#define PARAM_TYPE_STATE_KEY            1
/**
 * @brief enable value type boolean
 */
#define PARAM_TYPE_BOOLEAN              1
/**
 * @brief enable value type string
 */
#define PARAM_TYPE_STRING               1
/**
 * @brief enable value type null
 */
#define PARAM_TYPE_NULL                 1
/**
 * @brief enable 64bit variables
 */
#define PARAM_TYPE_64BIT                1
/**
 * @brief define toStr decimal length, 0 means all digits
 */
#define PARAM_FLOAT_DECIMAL_LEN         0
/**
 * @brief This macro help you to define the maximum number of parameters.
 */
typedef int16_t Param_LenType;
/**
 * @brief if enable this param, Param_compareValue check teh value of Null
 */
#define PARAM_COMPARE_NULL_VAL          0
/**
 * @brief if enable this param, Param_compareValue check teh value of Unknown
 */
#define PARAM_COMPARE_UNKNOWN_VAL       0
/**
 * System byte order
 */
#define PARAM_SYSTEM_BYTE_ORDER_LE      0       /**< Little-Endian */
#define PARAM_SYSTEM_BYTE_ORDER_BE      1       /**< Big-Endian */
#define PARAM_SYSTEM_BYTE_ORDER_AU      2       /**< Auto Detect, if can't find use LE */
/**
 * @brief Defien param system byte order
 */
#define PARAM_SYSTEM_BYTE_ORDER         PARAM_SYSTEM_BYTE_ORDER_AU

/* Default Values for toStr */
#define PARAM_DEFAULT_NULL              "Null"
#define PARAM_DEFAULT_TRUE              "True"
#define PARAM_DEFAULT_FALSE             "False"
#define PARAM_DEFAULT_OFF               "Off"
#define PARAM_DEFAULT_ON                "On"
#define PARAM_DEFAULT_HIGH              "High"
#define PARAM_DEFAULT_LOW               "Low"
#define PARAM_DEFAULT_HEX               "0x"
#define PARAM_DEFAULT_BIN               "0b"

/********************************************************************************************/
/**
 * Number default type
 */
#if PARAM_TYPE_64BIT
    typedef int64_t         Param_Number;
    typedef uint64_t        Param_UNumber;
    typedef uint64_t        Param_NumberHex;
    typedef uint64_t        Param_NumberBin;
#else
    typedef int32_t         Param_Number;
    typedef uint32_t        Param_UNumber;
    typedef uint32_t        Param_NumberHex;
    typedef uint32_t        Param_NumberBin;
#endif

#include "ParamMacro.h"

/**
 * @brief result of parse parameter
 */
typedef enum {
    Param_Ok,
    Param_Error,
} Param_Result;
/**
 * @brief Param value category
 * Each category can have same value if not overflow
 */
typedef enum {
    Param_Category_Number           = 0x00,
    Param_Category_Boolean          = 0x10,
    Param_Category_Float            = 0x20,
    Param_Category_Double           = 0x30,
    Param_Category_String           = 0x40,
    Param_Category_Mask             = 0xF0,
} Param_Category;
/**
 * @brief show type of param
 */
typedef enum {
    Param_ValueType_Number          = Param_Category_Number,    /**< ex: 13 */
    Param_ValueType_UNumber,                                    /**< ex: 13u */
    Param_ValueType_NumberHex,                                  /**< ex: 0xAB25 */
    Param_ValueType_NumberBinary,                               /**< ex: 0b01100101 */
    Param_ValueType_UInt8,                                      /**< ex: 125u8 */
    Param_ValueType_Int8,                                       /**< ex: 125i8 */
    Param_ValueType_UInt16,                                     /**< ex: 125u16 */
    Param_ValueType_Int16,                                      /**< ex: 125i8 */
    Param_ValueType_UInt32,                                     /**< ex: 125u32 */
    Param_ValueType_Int32,                                      /**< ex: 125i32 */
#if PARAM_TYPE_64BIT
    Param_ValueType_UInt64,                                     /**< ex: 125u64 */
    Param_ValueType_Int64,                                      /**< ex: 125i64 */
#endif
    Param_ValueType_Boolean          = Param_Category_Boolean,  /**< (true, false), ex: true */    
    Param_ValueType_State,                                      /**< (high, low), ex: high */
    Param_ValueType_StateKey,                                   /**< (on, off), ex: off */
    Param_ValueType_Float           = Param_Category_Float,     /**< ex: 2.54f or 2.54 or 2.54f32 */
#if PARAM_TYPE_64BIT
    Param_ValueType_Double          = Param_Category_Double,    /**< ex: 2.54f64 */
#endif
    Param_ValueType_String          = Param_Category_String,    /**< ex: "Text" */
    Param_ValueType_Null,                                       /**< ex: null */
    Param_ValueType_Unknown,                                    /**< first character of value not match with any of supported values */
} Param_ValueType;
/**
 * @brief hold type of param in same memory
 */
typedef struct {
    union {
        PARAM_IMPL_VALUETYPE(Unknown);
        PARAM_IMPL_VALUETYPE(Null);
        PARAM_IMPL_VALUETYPE(Number);
        PARAM_IMPL_VALUETYPE(UNumber);
        PARAM_IMPL_VALUETYPE(NumberHex);
        PARAM_IMPL_VALUETYPE(NumberBinary);
        PARAM_IMPL_VALUETYPE(UInt8);
        PARAM_IMPL_VALUETYPE(Int8);
        PARAM_IMPL_VALUETYPE(UInt16);
        PARAM_IMPL_VALUETYPE(Int16);
        PARAM_IMPL_VALUETYPE(UInt32);
        PARAM_IMPL_VALUETYPE(Int32);
    #if PARAM_TYPE_64BIT
        PARAM_IMPL_VALUETYPE(UInt64);
        PARAM_IMPL_VALUETYPE(Int64);
    #endif
        PARAM_IMPL_VALUETYPE(Float);
    #if PARAM_TYPE_64BIT
        PARAM_IMPL_VALUETYPE(Double);
    #endif
        PARAM_IMPL_VALUETYPE(State);
        PARAM_IMPL_VALUETYPE(StateKey);
        PARAM_IMPL_VALUETYPE(Boolean);
        PARAM_IMPL_VALUETYPE(String);
    };
    Param_ValueType     Type;
} Param_Value;
/**
 * @brief show details of param
 */
typedef struct {
    Param_Value         Value;
    Param_LenType       Index;
} Param;
/**
 * @brief use for handle params and show current pos
 */
typedef struct {
    char*               Ptr;
    Str_LenType         Len;
    char                ParamSeparator;
    Param_LenType       Index;
} Param_Cursor;

void Param_initCursor(Param_Cursor* cursor, char* ptr, Str_LenType len, char paramSeparator);

Param* Param_next(Param_Cursor* cursor, Param* param);
Str_LenType Param_toStr(char* str, Param_Value* values, Param_LenType len, char* separator);

Str_LenType Param_valueToStr(char* str, Param_Value* value);

char Param_compareValue(Param_Value* a, Param_Value* b);

// ------------------------------------ Helper Functions --------------------------------------
Param_Result Param_parse(char* str, Param_Value* param);
#if PARAM_TYPE_NUMBER_BINARY
    Param_Result Param_parseBinary(char* str, Param_Value* param);
#endif
#if PARAM_TYPE_NUMBER_HEX
    Param_Result Param_parseHex(char* str, Param_Value* param);
#endif
#if PARAM_TYPE_NUMBER
    Param_Result Param_parseNum(char* str, Param_Value* param);
#endif
#if PARAM_TYPE_STRING
    Param_Result Param_parseString(char* str, Param_Value* param);
#endif
#if PARAM_TYPE_STATE
    Param_Result Param_parseState(char* str, Param_Value* param);
#endif
#if PARAM_TYPE_STATE_KEY
    Param_Result Param_parseStateKey(char* str, Param_Value* param);
#endif
#if PARAM_TYPE_BOOLEAN
    Param_Result Param_parseBoolean(char* str, Param_Value* param);
#endif
#if PARAM_TYPE_NULL
    Param_Result Param_parseNull(char* str, Param_Value* param);
#endif
Param_Result Param_parseUnknown(char* str, Param_Value* param);



#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif // _PARAM_H_
