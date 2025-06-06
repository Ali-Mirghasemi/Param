# Param
Param Library help you to parse string parameters

## Features 
- Parse all standard data types
- Support insensitive case mode
- Support custom param separator
- Support serialize params to string
- Full configuration

## Supported Data Types
- `Number` (`int32_t`) - Ex: "1234", "-146"
- `NumberBinary` (`uint32_t`) - Ex: "0b1010", "-0b1010"
- `NumberHex` (`uint32_t`) - Ex: "0xABCD", "0x12D"
- `Float` (`float`) - Ex: "12.34", "-12.34"
- `String` (`char*`) - String types must include double quote Ex: "\"Hello World\""
- `Boolean` (`uint8_t`) - Ex: "true", "false"
- `State` (`uint8_t`) - Ex: "high", "low"
- `StateKey` (`uint8_t`) - Ex: "on", "off"
- `Null` (`char*`) - Ex: "null"
- `Unknown` (`char*`) - Ex: unknown, asdd, 12x32

## Examples
- [Param-Test](./Examples/Param-Test/) Show how to use Param Library
