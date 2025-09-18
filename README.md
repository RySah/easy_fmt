# EasyFMT

> [!CAUTION]
> The libary is not ready for commercial use, bugs may still be present.

A modern C++ header-only formatting library that provides powerful string manipulation utilities and custom formatter creation capabilities for std::format.

## Features

- 🚀 Header-only library - just include and use
- 📦 Modern C++20 implementation
- 🛠️ Rich string manipulation utilities
- 🎨 Custom formatter creation with macros
- 🔧 Namespace and case formatting options
- 💪 Type-safe formatting with compile-time checks

## Installation

Since EasyFMT is a header-only library, you just need to include the headers in your project:

```cpp
#include <easy_fmt/easy_fmt.hpp>
```

Make sure your compiler supports C++20 features, as this library uses modern C++ features like concepts and std::format.

## String Utilities

The library provides a comprehensive set of string manipulation functions under the `easy_fmt::string_utils` namespace:

```cpp
using namespace easy_fmt::string_utils;

// String trimming
auto trimmed = trim("  hello  ");           // "hello"
auto frontTrimmed = trimStart("  hello");   // "hello"
auto backTrimmed = trimEnd("hello  ");      // "hello"

// Case conversion
auto upper = toUpper("hello");              // "HELLO"
auto lower = toLower("HELLO");              // "hello"
auto cap = capitalize("hello");             // "Hello"

// String operations
auto parts = split("a,b,c", ',');           // vector<string>{"a", "b", "c"}
auto joined = join({"a", "b", "c"}, ",");   // "a,b,c"
auto repeated = repeat("ha", 3);            // "hahaha"

// String checks
bool has = contains("hello", "ll");         // true
bool starts = startsWith("hello", "he");    // true
bool ends = endsWith("hello", "lo");        // true
```

## Custom Formatters

EasyFMT makes it easy to create custom formatters for your types using macros. Here's an example:

```cpp
namespace log {
    enum class LogLevel { Debug, Info, Warning, Error };
}

CREATE_FORMATTER(
    log::LogLevel,
    "log::LogLevel::{}",
    FORMATTER_MODULES(
        FORMATTER_USING_NAMESPACE(log)
        FORMATTER_CREATE_ENUM_MAP_MODULE(
            LogLevel,
            AS_STR,
            static_cast<int>,
            {
                { LogLevel::Debug, "DEBUG" },
                { LogLevel::Info, "INFO" },
                { LogLevel::Warning, "WARN" },
                { LogLevel::Error, "ERROR" }
            }
        )
    ),
    AS_STR(OBJ)
);

// Usage:
LogLevel level = LogLevel::Warning;
std::cout << std::format("{}", level);      // Outputs: log::LogLevel::WARN
std::cout << std::format("{:L}", level);    // Outputs (Same as "{:fL}"): log::loglevel::warn        
std::cout << std::format("{:U}", level);    // Outputs (Same as "{:fU}"): LOG::LOGLEVEL::WARN
std::cout << std::format("{:sL}", level);    // Outputs: warn
std::cout << std::format("{:sU}", level);    // Outputs: WARN
```

### Formatter Options

- Namespace options (use with `:s` or `:f`):
  - `:f` - Full namespace (default)
  - `:s` - Short form (last part only)
  
- Case options (use with `:L` or `:U`):
  - Default - No case modification
  - `:L` - Convert to lowercase
  - `:U` - Convert to uppercase

## Requirements

- C++20 compatible compiler
- Standard library with std::format support

## Types Reference Guide

This section explains all the available functions and there uses.

### Types "formatter.hpp" "easy_fmt.hpp"

```cpp
enum class FormatterNamespaceOption : unsigned char {
    eFull,
    eShort
};
```

```cpp
enum class FormatterCaseOption : unsigned char
{
    eDefault,
    eLower,
    eUpper
};
```

Each formatter will have these types assigned to variables (NAMESPACE_OPTION & CASE_OPTION).
Before the the formatting stage begins the options "s", "f", "L", "U" will be parsed and will update the respective NAMESPACE_OPTION and CASE_OPTION variables, which can be used by modules and the actual formatting process. (NOTE: Some expressions may not work, its adviced to at max simply use ternary operations e.g. (CASE_OPTION == easy_fmt::FormatterCaseOption::eFull ? "Formatting for full ns" ? "Formatting for short ns"))



## Macro Reference Guide

This section explains all the available macros and how they work together to create custom formatters.

### Core Formatter Macros

#### `CREATE_FORMATTER(Type, StringFmt, Modules, FmtStringArguments...)`
The main macro to create a formatter for your custom type.
- `Type`: The type you want to create a formatter for
- `StringFmt`: The format string template (e.g., "{}", "MyType::{}") (treat this the same as a std::format string)
- `Modules`: Module definitions (use FORMATTER_MODULES to group them)
- `...`: Arguments to be passed to the std::format string

```cpp
CREATE_FORMATTER(MyType, "{}", FORMATTER_NO_MODULES, toString(OBJ))
```

This macro will also provide an variable called `OBJ` (const Type&) by which you can process.

#### `FORMATTER_MODULES(...)`
Groups multiple module definitions together. (NOTE: Its important you do not seperate the items by ',')
```cpp
FORMATTER_MODULES(
    FORMATTER_USING_NAMESPACE(my_namespace)
    FORMATTER_CREATE_MODULE(...)
)
```

#### `FORMATTER_NO_MODULES`
Used when no additional modules are needed for the formatter.

### Module Creation Macros

#### `FORMATTER_CREATE_MODULE(Type, Name, Implementation)`
Creates a custom module with specified type and implementation.
```cpp
FORMATTER_CREATE_MODULE(
    FORMATTER_MODULE_TYPE(string, MyType),
    CONVERT_TO_STRING,
    [](MyType v) -> string { return v.toString(); }
)
```

#### `FORMATTER_MODULE_TYPE(ReturnType, ArgTypes...)`
Establishes the function type for an module. (NOTE: The use of `auto` has not been tested and therefore adviced against)

#### `FORMATTER_CREATE_ENUM_MAP_MODULE(Type, Name, AsUnderlying, Map)`
Creates a module for enum-to-string conversion.
- `Type`: The enum type
- `Name`: The name of the conversion function
- `AsUnderlying`: Function to convert enum to a underlying type. (NOTE: Data returned from this will be used as a parameter for easy_fmt::toString)
- `Map`: Mapping of enum values to strings
```cpp
FORMATTER_CREATE_ENUM_MAP_MODULE(
    LogLevel,
    LOG_STR,
    static_cast<int>,
    {
        {LogLevel::Debug, "DEBUG"},
        {LogLevel::Info, "INFO"}
    }
)
```

#### `FORMATTER_CREATE_SWITCH_MODULE(Type, Name, Cases)`
Creates a module using a switch statement for conversion.
```cpp
FORMATTER_CREATE_SWITCH_MODULE(
    LogLevel,
    LOG_STR,
    {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        default: return "UNKNOWN";
    }
)
```

### Utility Module Macros

#### String Manipulation Modules
- `FORMATTER_TRIM_MODULE`: Adds TRIM
- `FORMATTER_TRIM_START_MODULE`: Adds TRIM_START
- `FORMATTER_TRIM_END_MODULE`: Adds TRIM_END
- `FORMATTER_TRIMMING_MODULES`: Adds all trimming functions (TRIM_START, TRIM_END, TRIM)
- `FORMATTER_CASE_CONVERSION_MODULES`: Adds case conversion functions
- `FORMATTER_SPLIT_JOIN_MODULES`: Adds string splitting and joining functions

```cpp
CREATE_FORMATTER(
    MyType,
    "{}",
    FORMATTER_MODULES(
        FORMATTER_CASE_CONVERSION_MODULES
        FORMATTER_TRIMMING_MODULES
    ),
    TO_LOWER(TRIM(toString(OBJ)))
)
```

#### String Operation Modules
- `FORMATTER_CONTAINS_MODULE`: String containment check
- `FORMATTER_STARTS_WITH_MODULE`: Prefix check
- `FORMATTER_ENDS_WITH_MODULE`: Suffix check
- `FORMATTER_REPLACE_MODULE`: String replacement
- `FORMATTER_REMOVE_MODULE`: String removal
- `FORMATTER_SUBSTR_MODULE`: Substring extraction
- `FORMATTER_REPEAT_MODULE`: String repetition

### Advanced Usage

You can combine multiple modules to create sophisticated formatters:

```cpp
CREATE_FORMATTER(
    ComplexType,
    "{}.{}",
    FORMATTER_MODULES(
        FORMATTER_USING_NAMESPACE(my_namespace)
        FORMATTER_CASE_CONVERSION_MODULES
        FORMATTER_CREATE_MODULE(
            FORMATTER_MODULE_TYPE(string, ComplexType),
            GET_PREFIX,
            [](const ComplexType& obj) { return obj.getPrefix(); }
        )
        FORMATTER_CREATE_MODULE(
            FORMATTER_MODULE_TYPE(string, ComplexType),
            GET_SUFFIX,
            [](const ComplexType& obj) { return obj.getSuffix(); }
        )
    ),
    TO_UPPER(GET_PREFIX(OBJ)),
    TO_LOWER(GET_SUFFIX(OBJ))
)
```

This creates a formatter that:
1. Uses a custom namespace
2. Adds case conversion capabilities
3. Creates two custom modules for prefix and suffix extraction
4. Formats the output with uppercase prefix and lowercase suffix
