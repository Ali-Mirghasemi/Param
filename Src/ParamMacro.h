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

#define PARAM_VALUETYPE(NAME)                       __PARAM_VALUETYPE_(NAME)
#define __PARAM_VALUETYPE_(NAME)                    __PARAM_VALUETYPE__(NAME)
#define __PARAM_VALUETYPE__(NAME)                   __PARAM_VALUETYPE_ ##NAME


#define __PARAM_VALUETYPE_UNKNOWN                   char*
#define __PARAM_VALUETYPE_NUMBER                    int32_t
#define __PARAM_VALUETYPE_NUMBERHEX                 uint32_t
#define __PARAM_VALUETYPE_NUMBERBINARY              uint32_t
#define __PARAM_VALUETYPE_FLOAT                     float
#define __PARAM_VALUETYPE_STATE                     uint8_t
#define __PARAM_VALUETYPE_STATEKEY                  uint8_t
#define __PARAM_VALUETYPE_BOOLEAN                   uint8_t
#define __PARAM_VALUETYPE_STRING                    char*
#define __PARAM_VALUETYPE_NULL                      char*

#define __PARAM_VALUETYPE_Unknown                   __PARAM_VALUETYPE_UNKNOWN
#define __PARAM_VALUETYPE_Number                    __PARAM_VALUETYPE_NUMBER
#define __PARAM_VALUETYPE_NumberHex                 __PARAM_VALUETYPE_NUMBERHEX
#define __PARAM_VALUETYPE_NumberBinary              __PARAM_VALUETYPE_NUMBERBINARY
#define __PARAM_VALUETYPE_Float                     __PARAM_VALUETYPE_FLOAT
#define __PARAM_VALUETYPE_State                     __PARAM_VALUETYPE_STATE
#define __PARAM_VALUETYPE_StateKey                  __PARAM_VALUETYPE_STATEKEY
#define __PARAM_VALUETYPE_Boolean                   __PARAM_VALUETYPE_BOOLEAN
#define __PARAM_VALUETYPE_String                    __PARAM_VALUETYPE_STRING
#define __PARAM_VALUETYPE_Null                      __PARAM_VALUETYPE_NULL



#endif // _PARAM_MACO_H_
