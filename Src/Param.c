#include "Param.h"

#define PARAM_DEFAULT_NULL_LEN     (sizeof(PARAM_DEFAULT_NULL) - 1)
#define PARAM_DEFAULT_TRUE_LEN     (sizeof(PARAM_DEFAULT_TRUE) - 1)
#define PARAM_DEFAULT_FALSE_LEN    (sizeof(PARAM_DEFAULT_FALSE) - 1)
#define PARAM_DEFAULT_OFF_LEN      (sizeof(PARAM_DEFAULT_OFF) - 1)
#define PARAM_DEFAULT_ON_LEN       (sizeof(PARAM_DEFAULT_ON) - 1)
#define PARAM_DEFAULT_HIGH_LEN     (sizeof(PARAM_DEFAULT_HIGH) - 1)
#define PARAM_DEFAULT_LOW_LEN      (sizeof(PARAM_DEFAULT_LOW) - 1)
#define PARAM_DEFAULT_HEX_LEN      (sizeof(PARAM_DEFAULT_HEX) - 1)
#define PARAM_DEFAULT_BIN_LEN      (sizeof(PARAM_DEFAULT_BIN) - 1)

#if PARAM_CASE_MODE == PARAM_CASE_INSENSITIVE
    #define __convert(STR) Str_lowerCase((STR))
#else 
    #define __convert(STR) 
#endif // PARAM_CASE_MODE

/**
 * @brief initialize the parameter cursor
 * 
 * @param cursor 
 * @param ptr 
 * @param len 
 * @param paramSeparator 
 */
void Param_initCursor(Param_Cursor* cursor, char* ptr, Str_LenType len, char paramSeparator) {
    cursor->Ptr = ptr;
    cursor->Len = len;
    cursor->ParamSeparator = paramSeparator;
    cursor->Index = 0;
}
/**
 * @brief parse next param and return
 *
 * @param cursor
 * @param param
 * @return Param* return param
 */
Param* Param_next(Param_Cursor* cursor, Param* param) {
    char* pStr = cursor->Ptr;
    char* paramStr;
    // check cursor is valid
    if (cursor->Ptr == NULL || (*cursor->Ptr == '\0' && cursor->Len == 0)) {
        return NULL;
    }
    // ignore whitspaces
    cursor->Ptr = Str_ignoreWhitespace(cursor->Ptr);
    cursor->Len -= (Str_LenType)(cursor->Ptr - pStr);
    // find end of param
    paramStr = cursor->Ptr;
    pStr = Str_indexOf(cursor->Ptr, cursor->ParamSeparator);
    if (pStr != NULL) {
        Str_LenType len = (Str_LenType)(pStr - cursor->Ptr);
        *pStr = '\0';
        cursor->Ptr = pStr + 1;
        cursor->Len -= len + 1;
    }
    else {
        cursor->Ptr = NULL;
        cursor->Len = 0;
    }
    // trim right
    if(*paramStr){
        paramStr = Str_trimRight(paramStr);
    }
    // find value type base on first character
    Param_parse(paramStr, &param->Value);
    // return param
    param->Index = cursor->Index++;
    return param;
}
/**
 * @brief Parse a string into param object
 * 
 * @param str 
 * @param param 
 * @return Param_Result 
 */
Param_Result Param_parse(char* str, Param_Value* param) {
    Param_Result res = Param_Error;

    switch (*str) {
    #if PARAM_TYPE_NUMBER
        case '0':
        #if PARAM_TYPE_NUMBER_BINARY || PARAM_TYPE_NUMBER_HEX
            switch (*(str + 1)) {
            #if PARAM_TYPE_NUMBER_BINARY
            #if PARAM_CASE_MODE & PARAM_CASE_HIGHER != 0
                case 'b':
            #endif
            #if PARAM_CASE_MODE & PARAM_CASE_HIGHER != 0
                case 'B':
            #endif
                    // binary num
                    res = Param_parseBinary(str, param);
                    break;
            #endif // PARAM_TYPE_NUMBER_BINARY
            #if PARAM_TYPE_NUMBER_HEX
            #if PARAM_CASE_MODE & PARAM_CASE_LOWER != 0
                case 'x':
            #endif
            #if PARAM_CASE_MODE & PARAM_CASE_HIGHER != 0
                case 'X':
            #endif
                    // hex num
                    res = Param_parseHex(str, param);
                    break;
            #endif // PARAM_TYPE_NUMBER_HEX
            }
        #endif // PARAM_TYPE_NUMBER_BINARY || PARAM_TYPE_NUMBER_HEX
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            // check for number or its float
            if (res != Param_Ok) {
                res = Param_parseNum(str, param);
            }
            break;
    #endif // PARAM_TYPE_NUMBER
    #if PARAM_TYPE_BOOLEAN
    #if PARAM_CASE_MODE & PARAM_CASE_LOWER != 0
        case 't':
        case 'f':
    #endif
    #if PARAM_CASE_MODE & PARAM_CASE_HIGHER != 0
        case 'T':
        case 'F':
    #endif
            // boolean
            res = Param_parseBoolean(str, param);
            break;
    #endif // PARAM_TYPE_BOOLEAN
    #if PARAM_TYPE_STATE
    #if PARAM_CASE_MODE & PARAM_CASE_LOWER != 0
        case 'o':
    #endif
    #if PARAM_CASE_MODE & PARAM_CASE_HIGHER != 0
        case 'O':
    #endif
            // state
            res = Param_parseStateKey(str, param);
            break;
    #endif // PARAM_TYPE_STATE
    #if PARAM_TYPE_STATE_KEY
    #if PARAM_CASE_MODE & PARAM_CASE_LOWER != 0
        case 'l':
        case 'h':
    #endif
    #if PARAM_CASE_MODE & PARAM_CASE_HIGHER != 0
        case 'L':
        case 'H':
    #endif
            // state key
            res = Param_parseState(str, param);
            break;
    #endif // PARAM_TYPE_STATE_KEY
    #if PARAM_TYPE_STRING
        case '"':
            // string
            res = Param_parseString(str, param);
            break;
    #endif // PARAM_TYPE_STRING
    #if PARAM_TYPE_NULL
    #if PARAM_CASE_MODE & PARAM_CASE_LOWER != 0
        case 'n':
    #endif
    #if PARAM_CASE_MODE & PARAM_CASE_HIGHER != 0
        case 'N':
    #endif
            res = Param_parseNull(str, param);
            break;
    #endif // PARAM_TYPE_NULL
    }

    // check if param is not valid
    if (res != Param_Ok) {
        Param_parseUnknown(str, param);
    }

    return res;
}
#if PARAM_TYPE_NUMBER_BINARY
/**
 * @brief parse binary strings
 * ex: "0b1010" -> 0xA
 *
 * @param cursor
 * @param param
 * @return Param_Result
 */
Param_Result Param_parseBinary(char* str, Param_Value* param) {
    param->Type = Param_ValueType_NumberBinary;
    return (Param_Result) Str_convertUNum(str + 2, (unsigned int*) &param->NumberBinary, Str_Binary);
}
#endif // PARAM_TYPE_NUMBER_BINARY
#if PARAM_TYPE_NUMBER_HEX
/**
 * @brief parse hex strings
 * ex: "0xA" -> 0xA
 *
 * @param cursor
 * @param param
 * @return Param_Result
 */
Param_Result Param_parseHex(char* str, Param_Value* param) {
    param->Type = Param_ValueType_NumberHex;
    return (Param_Result) Str_convertUNum(str + 2, (unsigned int*) &param->NumberHex, Str_Hex);
}
#endif // PARAM_TYPE_NUMBER_HEX
#if PARAM_TYPE_NUMBER
/**
 * @brief parse number strings
 * ex: "123" -> 123
 *
 * @param cursor
 * @param param
 * @return Param_Result
 */
Param_Result Param_parseNum(char* str, Param_Value* param) {
    if (Str_indexOf(str, '.') != NULL) {
        // it's float
        param->Type = Param_ValueType_Float;
        return (Param_Result) Str_convertFloat(str, &param->Float);
    }
    else {
        // it's number
        param->Type = Param_ValueType_Number;
        return (Param_Result) Str_convertNum(str, (int*) &param->Number, Str_Decimal);
    }
}
#endif // PARAM_TYPE_NUMBER
#if PARAM_TYPE_STRING
/**
 * @brief parse strings
 * ex: "\"Text\"" -> "Text"
 *
 * @param cursor
 * @param param
 * @return Param_Result
 */
Param_Result Param_parseString(char* str, Param_Value* param) {
    Str_LenType len = Str_fromString(str);
    if (len != -1) {
        param->Type = Param_ValueType_String;
        param->String = str;
        return Param_Ok;
    }
    else {
        return Param_Error;
    }
}
#endif // PARAM_TYPE_STRING
#if PARAM_TYPE_STATE
/**
 * @brief parse state strings
 * ex: "high" -> 1 or "low" -> 0
 *
 * @param cursor
 * @param param
 * @return Param_Result
 */
Param_Result Param_parseState(char* str, Param_Value* param) {
    __convert(str);
    param->Type = Param_ValueType_State;
    if (
    #if PARAM_CASE_MODE == PARAM_CASE_LOWER
        Str_compare(str, "high") == 0
    #elif PARAM_CASE_MODE == PARAM_CASE_HIGHER
        Str_compare(str, "HIGH") == 0
    #else
        Str_compare(str, "high") == 0
    #endif
    ) {
        param->State = 1;
        return Param_Ok;
    }
    else if (
    #if PARAM_CASE_MODE == PARAM_CASE_LOWER
        Str_compare(str, "low") == 0
    #elif PARAM_CASE_MODE == PARAM_CASE_HIGHER
        Str_compare(str, "LOW") == 0
    #else
        Str_compare(str, "low") == 0
    #endif
    ) {
        param->State = 0;
        return Param_Ok;
    }
    else {
        return Param_Error;
    }
}
#endif // PARAM_TYPE_STATE
#if PARAM_TYPE_STATE_KEY
/**
 * @brief parse state key strings
 * ex: "on" -> 1 or "off" -> 0
 *
 * @param cursor
 * @param param
 * @return Param_Result
 */
Param_Result Param_parseStateKey(char* str, Param_Value* param) {
    __convert(str);
    if (
    #if PARAM_CASE_MODE == PARAM_CASE_LOWER
        Str_compare(str, "on") == 0
    #elif PARAM_CASE_MODE == PARAM_CASE_HIGHER
        Str_compare(str, "ON") == 0
    #else
        Str_compare(str, "on") == 0
    #endif
    ) {
        param->Type = Param_ValueType_StateKey;
        param->StateKey = 1;
        return Param_Ok;
    }
    else if (
    #if PARAM_CASE_MODE == PARAM_CASE_LOWER
        Str_compare(str, "off") == 0
    #elif PARAM_CASE_MODE == PARAM_CASE_HIGHER
        Str_compare(str, "OFF") == 0
    #else
        Str_compare(str, "off") == 0
    #endif
    ) {
        param->Type = Param_ValueType_StateKey;
        param->StateKey = 0;
        return Param_Ok;
    }
    else {
        return Param_Error;
    }
}
#endif // PARAM_TYPE_STATE_KEY
#if PARAM_TYPE_BOOLEAN
/**
 * @brief parse boolean strings
 * ex: "true" -> true or "false" -> false
 *
 * @param cursor
 * @param param
 * @return Param_Result
 */
Param_Result Param_parseBoolean(char* str, Param_Value* param) {
    __convert(str);
    if (
    #if PARAM_CASE_MODE == PARAM_CASE_LOWER
        Str_compare(str, "true") == 0
    #elif PARAM_CASE_MODE == PARAM_CASE_HIGHER
        Str_compare(str, "TRUE") == 0
    #else
        Str_compare(str, "true") == 0
    #endif
    ) {
        param->Type = Param_ValueType_Boolean;
        param->Boolean = 1;
        return Param_Ok;
    }
    else if (
    #if PARAM_CASE_MODE == PARAM_CASE_LOWER
        Str_compare(str, "false") == 0
    #elif PARAM_CASE_MODE == PARAM_CASE_HIGHER
        Str_compare(str, "FALSE") == 0
    #else
        Str_compare(str, "false") == 0
    #endif
    ) {
        param->Type = Param_ValueType_Boolean;
        param->Boolean = 0;
        return Param_Ok;
    }
    else {
        return Param_Error;
    }
}
#endif // PARAM_TYPE_BOOLEAN
#if PARAM_TYPE_NULL
/**
 * @brief parse null
 * ex: "null" -> NULL
 *
 * @param cursor
 * @param param
 * @return Param_Result
 */
Param_Result Param_parseNull(char* str, Param_Value* param) {
    __convert(str);
    if (
    #if PARAM_CASE_MODE == PARAM_CASE_LOWER
        Str_compare(str, "null") == 0
    #elif PARAM_CASE_MODE == PARAM_CASE_HIGHER
        Str_compare(str, "NULL") == 0
    #else
        Str_compare(str, "null") == 0
    #endif
    ) {
        param->Type = Param_ValueType_Null;
        param->Null = str;
        return Param_Ok;
    }
    else {
        return Param_Error;
    }
}
#endif // PARAM_TYPE_NULL
/**
 * @brief parse unknown type
 *
 * @param cursor
 * @param param
 * @return Param_Result
 */
Param_Result Param_parseUnknown(char* str, Param_Value* param) {
    param->Type = Param_ValueType_Unknown;
    param->Unknown = str;
    return Param_Ok;
}
/**
 * @brief compare param values, first ValueType and second Value
 *
 * @param a
 * @param b
 * @return char return 0 if not equal otherwise return 1
 */
char Param_compareValue(Param_Value* a, Param_Value* b) {
    if (a->Type != b->Type) {
        return 0;
    }
    switch (a->Type) {
        case Param_ValueType_NumberBinary:
            return a->NumberBinary == b->NumberBinary;
        case Param_ValueType_NumberHex:
            return a->NumberHex == b->NumberHex;
        case Param_ValueType_Number:
            return a->Number == b->Number;
        case Param_ValueType_Float:
            return a->Float == b->Float;
        case Param_ValueType_String:
            return Str_compare(a->String, b->String) == 0;
        case Param_ValueType_State:
            return a->State == b->State;
        case Param_ValueType_StateKey:
            return a->StateKey == b->StateKey;
        case Param_ValueType_Boolean:
            return a->Boolean == b->Boolean;
        case Param_ValueType_Null:
        #if PARAM_COMPARE_NULL_VAL
            return Str_compare(a->Null, b->Null) == 0;
        #else
            return 1;
        #endif // PARAM_COMPARE_NULL_VAL
        case Param_ValueType_Unknown:
        #if PARAM_COMPARE_UNKNOWN_VAL
            return Str_compare(a->Unknown, b->Unknown) == 0;
        #else
            return 1;
        #endif // PARAM_COMPARE_UNKNOWN_VAL
        default:
            return 0;
    }
}
/**
 * @brief convert array of values to string
 *
 * @param str
 * @param values
 * @param len
 * @param separator
 * @return Str_LenType
 */
Str_LenType Param_toStr(char* str, Param_Value* values, Param_LenType len, char* separator) {
    char* base = str;
    Str_LenType sepLen = Str_len(separator);
    while (--len > 0) {
        // convert value
        str += Param_valueToStr(str, values++);
        // add separator
        if (separator) {
            Str_copy(str, separator);
            str += sepLen;
        }
    }
    // convert value
    str += Param_valueToStr(str, values++);
    return (Str_LenType)(str - base);
}
/**
 * @brief convert value to string and return string length
 *
 * @param str
 * @param value
 * @return Str_LenType
 */
Str_LenType Param_valueToStr(char* str, Param_Value* value) {
    char* pStr;
    switch (value->Type) {
        case Param_ValueType_Number:
            return Str_parseNum(value->Number, Str_Decimal, STR_NORMAL_LEN, str);
        case Param_ValueType_NumberHex:
            Str_copy(str, PARAM_DEFAULT_HEX);
            return Str_parseNum(value->NumberHex, Str_Hex, STR_NORMAL_LEN, str + PARAM_DEFAULT_HEX_LEN) + PARAM_DEFAULT_HEX_LEN;
        case Param_ValueType_NumberBinary:
            Str_copy(str, PARAM_DEFAULT_BIN);
            return Str_parseNum(value->NumberBinary, Str_Binary, STR_NORMAL_LEN, str + PARAM_DEFAULT_BIN_LEN) + PARAM_DEFAULT_BIN_LEN;
        case Param_ValueType_Float:
        #if PARAM_FLOAT_DECIMAL_LEN != 0
            return Str_parseFloatFix(value->Float, str, PARAM_FLOAT_DECIMAL_LEN);
        #else
            return Str_parseFloat(value->Float, str);
        #endif
        case Param_ValueType_String:
            pStr = Str_convertString(value->String, str);
            return (Str_LenType)(pStr - str);
        case Param_ValueType_State:
            if (value->State != 0) {
                Str_copy(str, PARAM_DEFAULT_HIGH);
                return PARAM_DEFAULT_HIGH_LEN;
            }
            else {
                Str_copy(str, PARAM_DEFAULT_LOW);
                return PARAM_DEFAULT_LOW_LEN;
            }
        case Param_ValueType_StateKey:
            if (value->StateKey != 0) {
                Str_copy(str, PARAM_DEFAULT_ON);
                return PARAM_DEFAULT_ON_LEN;
            }
            else {
                Str_copy(str, PARAM_DEFAULT_OFF);
                return PARAM_DEFAULT_OFF_LEN;
            }
        case Param_ValueType_Boolean:
            if (value->Boolean != 0) {
                Str_copy(str, PARAM_DEFAULT_TRUE);
                return PARAM_DEFAULT_TRUE_LEN;
            }
            else {
                Str_copy(str, PARAM_DEFAULT_FALSE);
                return PARAM_DEFAULT_FALSE_LEN;
            }
        case Param_ValueType_Null:
            Str_copy(str, PARAM_DEFAULT_NULL);
            return PARAM_DEFAULT_NULL_LEN;
        case Param_ValueType_Unknown:
            Str_copy(str, value->Unknown);
            return Str_len(value->Unknown);
        default:
            return 0;
    }
}
