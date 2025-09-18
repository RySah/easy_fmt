#pragma once

#include <format>
#include <functional>
#include <unordered_map>
#include <variant>
#include <type_traits>

#include "string_utils.hpp"

namespace EASY_FMT_NAMESPACE
{

    enum class FormatterNamespaceOption : unsigned char
    {
        eFull,
        eShort
    };
    enum class FormatterCaseOption : unsigned char
    {
        eDefault,
        eLower,
        eUpper
    };

    template <typename T, typename = void>
    struct is_formattable : std::false_type
    {
    };

    // Specialization: true if format("{}", t) compiles
    template <typename T>
    struct is_formattable<T, std::void_t<decltype(std::format("{}", std::declval<T const &>()))>> : std::true_type
    {
    };

    template <typename T>
    inline constexpr bool is_formattable_v = is_formattable<T>::value;

} // namespace EASY_FMT_NAMESPACE

#define __xstr(a) __str(a)
#define __str(a) #a

#define __FORMATTER_FORMATTER_NAMESPACE_OPTION_NAME NAMESPACE_OPTION
#define __FORMATTER_FORMATTER_CASE_OPTION_NAME CASE_OPTION

#define __CREATE_FORMATTER_INTERNAL(Type, StringFmt, Modules, ...) \
    template <> \
    struct formatter<Type, char> { \
        using Self = formatter<Type, char>; \
        EASY_FMT_NAMESPACE::FormatterNamespaceOption __FORMATTER_FORMATTER_NAMESPACE_OPTION_NAME = EASY_FMT_NAMESPACE::FormatterNamespaceOption::eFull; \
        EASY_FMT_NAMESPACE::FormatterCaseOption __FORMATTER_FORMATTER_CASE_OPTION_NAME = EASY_FMT_NAMESPACE::FormatterCaseOption::eDefault; \
        constexpr auto parse(format_parse_context& ctx) { \
            auto it = ctx.begin(); \
            auto end = ctx.end(); \
            if (it != end) { \
                if (*it == 's') { \
                    __FORMATTER_FORMATTER_NAMESPACE_OPTION_NAME = EASY_FMT_NAMESPACE::FormatterNamespaceOption::eShort; \
                    ++it; \
                } \
                else if (*it == 'f') { \
                    __FORMATTER_FORMATTER_NAMESPACE_OPTION_NAME = EASY_FMT_NAMESPACE::FormatterNamespaceOption::eFull; \
                    ++it; \
                } \
            } \
            if (it != end) { \
                if (*it == 'L') { \
                    __FORMATTER_FORMATTER_CASE_OPTION_NAME = EASY_FMT_NAMESPACE::FormatterCaseOption::eLower; \
                    ++it; \
                } \
                else if (*it == 'U') { \
                    __FORMATTER_FORMATTER_CASE_OPTION_NAME = EASY_FMT_NAMESPACE::FormatterCaseOption::eUpper; \
                    ++it; \
                } \
            } \
            return it; \
        } \
        template <typename FormatContext> \
        auto format(const Type& OBJ, FormatContext& ctx) const { \
            Modules \
            std::string temp_result = std::format(StringFmt, __VA_ARGS__); \
            switch (__FORMATTER_FORMATTER_NAMESPACE_OPTION_NAME) { \
            case EASY_FMT_NAMESPACE::FormatterNamespaceOption::eShort: { \
                auto pos = temp_result.rfind("::"); \
                if (pos != std::string::npos) { \
                    std::string after = temp_result.substr(pos + 2); \
                    size_t i = 0; \
                    while (i < after.size() && (std::isalnum(static_cast<unsigned char>(after[i])) || after[i] == '_')) \
                        i++; \
                    std::string ident = after.substr(0, i); \
                    std::string rest = after.substr(i); \
                    temp_result = ident + rest; \
                } \
            } break; \
            default: break; \
            } \
            switch (__FORMATTER_FORMATTER_CASE_OPTION_NAME) { \
            case EASY_FMT_NAMESPACE::FormatterCaseOption::eLower: \
                temp_result = EASY_FMT_STRING_UTILS_NAMESPACE::toLower(temp_result); \
                break; \
            case EASY_FMT_NAMESPACE::FormatterCaseOption::eUpper: \
                temp_result = EASY_FMT_STRING_UTILS_NAMESPACE::toUpper(temp_result); \
                break; \
            default: break; \
            } \
            return std::ranges::copy(temp_result, ctx.out()).out; \
        } \
    }

#define CREATE_FORMATTER(Type, StringFmt, Modules, ...) \
    namespace std { \
        __CREATE_FORMATTER_INTERNAL(Type, StringFmt, Modules, __VA_ARGS__); \
    }

#define FORMATTER_MODULE_TYPE(ReturnType, ...) function<ReturnType(__VA_ARGS__)>
#define FORMATTER_CREATE_MODULE(Type, Name, ...) Type Name = __VA_ARGS__;

#define ASSERT_IS_FORMATTABLE(Item) static_assert(EASY_FMT_NAMESPACE::is_formattable_v<Item>, __xstr(Item) " is not formattable.");

#define FORMATTER_CREATE_ENUM_MAP_MODULE(Type, Name, AsUnderlying, ...) \
    FORMATTER_CREATE_MODULE(                                            \
        FORMATTER_MODULE_TYPE(string, Type),                            \
        Name,                                                           \
        [](Type v) -> string { \
            unordered_map<Type, string> m = __VA_ARGS__ ; \
            if (m.contains(v)) return m[v]; \
            else return EASY_FMT_STRING_UTILS_NAMESPACE::toString(AsUnderlying(v)); })

#define FORMATTER_CREATE_SWITCH_MODULE(Type, Name, ...) \
    FORMATTER_CREATE_MODULE( \
        FORMATTER_MODULE_TYPE(string, Type), \
        Name, \
        [](Type OBJ) -> string { \
            switch (OBJ) \
            __VA_ARGS__ \
        } \
    )

#define FORMATTER_TRIM_START_MODULE \
    FORMATTER_CREATE_MODULE( \
        FORMATTER_MODULE_TYPE(string, string_view), \
        TRIM_START, \
        EASY_FMT_STRING_UTILS_NAMESPACE::trimStart \
    )
#define FORMATTER_TRIM_END_MODULE \
    FORMATTER_CREATE_MODULE( \
        FORMATTER_MODULE_TYPE(string, string_view), \
        TRIM_END, \
        EASY_FMT_STRING_UTILS_NAMESPACE::trimEnd \
    )
#define FORMATTER_TRIM_MODULE \
    FORMATTER_CREATE_MODULE( \
        FORMATTER_MODULE_TYPE(string, string_view), \
        TRIM, \
        EASY_FMT_STRING_UTILS_NAMESPACE::trim \
    )
#define FORMATTER_TRIMMING_MODULES \
    FORMATTER_TRIM_START_MODULE \
    FORMATTER_TRIM_END_MODULE \
    FORMATTER_TRIM_MODULE

#define FORMATTER_SPLIT_MODULE \
    FORMATTER_CREATE_MODULE( \
        FORMATTER_MODULE_TYPE(vector<string>, string_view, char), \
        SPLIT, \
        EASY_FMT_STRING_UTILS_NAMESPACE::split \
    )
#define FORMATTER_JOIN_MODULE \
    FORMATTER_CREATE_MODULE( \
        FORMATTER_MODULE_TYPE(string, const vector<string>&, string_view), \
        JOIN, \
        EASY_FMT_STRING_UTILS_NAMESPACE::join \
    )
#define FORMATTER_SPLIT_JOIN_MODULES \
    FORMATTER_SPLIT_MODULE \
    FORMATTER_JOIN_MODULE

#define FORMATTER_TO_UPPER_MODULE \
    FORMATTER_CREATE_MODULE( \
        FORMATTER_MODULE_TYPE(string, string), \
        TO_UPPER, \
        EASY_FMT_STRING_UTILS_NAMESPACE::toUpper \
    )
#define FORMATTER_TO_LOWER_MODULE \
    FORMATTER_CREATE_MODULE( \
        FORMATTER_MODULE_TYPE(string, string), \
        TO_LOWER, \
        EASY_FMT_STRING_UTILS_NAMESPACE::toLower \
    )
#define FORMATTER_CAPITALIZE_MODULE \
    FORMATTER_CREATE_MODULE( \
        FORMATTER_MODULE_TYPE(string, string), \
        CAPITALIZE, \
        EASY_FMT_STRING_UTILS_NAMESPACE::capitalize \
    )
#define FORMATTER_CASE_CONVERSION_MODULES \
    FORMATTER_TO_UPPER_MODULE \
    FORMATTER_TO_LOWER_MODULE \
    FORMATTER_CAPITALIZE_MODULE

#define FORMATTER_CONTAINS_MODULE \
    FORMATTER_CREATE_MODULE( \
        FORMATTER_MODULE_TYPE(bool, string_view, string_view), \
        CONTAINS, \
        EASY_FMT_STRING_UTILS_NAMESPACE::contains \
    )

#define FORMATTER_STARTS_WITH_MODULE \
    FORMATTER_CREATE_MODULE( \
        FORMATTER_MODULE_TYPE(bool, string_view, string_view), \
        STARTS_WITH, \
        EASY_FMT_STRING_UTILS_NAMESPACE::startsWith \
    )

#define FORMATTER_ENDS_WITH_MODULE \
    FORMATTER_CREATE_MODULE( \
        FORMATTER_MODULE_TYPE(bool, string_view, string_view), \
        STARTS_WITH, \
        EASY_FMT_STRING_UTILS_NAMESPACE::endsWith \
    )

#define FORMATTER_REPLACE_MODULE \
    FORMATTER_CREATE_MODULE( \
        FORMATTER_MODULE_TYPE(string, string, string_view, string_view), \
        REPLACE, \
        EASY_FMT_STRING_UTILS_NAMESPACE::replace \
    )

#define FORMATTER_REMOVE_MODULE \
    FORMATTER_CREATE_MODULE( \
        FORMATTER_MODULE_TYPE(string, string, string_view), \
        REMOVE, \
        EASY_FMT_STRING_UTILS_NAMESPACE::remove \
    )

#define FORMATTER_SUBSTR_MODULE \
    FORMATTER_CREATE_MODULE( \
        FORMATTER_MODULE_TYPE(string, string_view, size_t, size_t), \
        SUBSTR, \
        EASY_FMT_STRING_UTILS_NAMESPACE::substrSafe \
    )

#define FORMATTER_FIND_FIRST_MODULE \
    FORMATTER_CREATE_MODULE( \
        FORMATTER_MODULE_TYPE(size_t, string_view, char), \
        FIND_FIRST, \
        EASY_FMT_STRING_UTILS_NAMESPACE::findFirst \
    )

#define FORMATTER_FIND_LAST_MODULE \
    FORMATTER_CREATE_MODULE( \
        FORMATTER_MODULE_TYPE(size_t, string_view, char), \
        FIND_LAST, \
        EASY_FMT_STRING_UTILS_NAMESPACE::findLast \
    )

#define FORMATTER_REPEAT_MODULE \
    FORMATTER_CREATE_MODULE( \
        FORMATTER_MODULE_TYPE(string, string_view, size_t), \
        REPEAT, \
        EASY_FMT_STRING_UTILS_NAMESPACE::repeat \
    )

#define FORMATTER_USING_NAMESPACE(Namespace) using namespace Namespace;

#define FORMATTER_NO_MODULES 
#define FORMATTER_MODULES(...) __VA_ARGS__
