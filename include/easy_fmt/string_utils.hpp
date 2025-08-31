#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <format>
#include <type_traits>
#include <algorithm>
#include <functional>
#include <span>

#define EASY_FMT_NAMESPACE easy_fmt
#define EASY_FMT_STRING_UTILS_NAMESPACE easy_fmt::string_utils

namespace easy_fmt::string_utils
{

    inline std::string trimStart(std::string_view s, std::function<int(int)> predicate = std::isspace) {
		auto it = std::find_if_not(s.begin(), s.end(), [&](char c) { return predicate(c); });
		return std::string(it, s.end());
	}

	inline std::string trimStart(std::string_view s, char predicate) {
		return trimStart(s, [&](int c) -> int { return (int)predicate == c; });
	}

	inline std::string trimStart(std::string_view s, std::string_view predicate) {;
		if (predicate.size() == 1) return trimStart(s, predicate[0]);
		return trimStart(s, [&](int c) -> int { 
			for (const auto& ch : predicate) {
				if (ch == c) return 1;
			}
			return 0;
		});
	}

	inline std::string trimStart(std::string_view s, std::span<const char> predicate) {;
		if (predicate.size() == 1) return trimStart(s, predicate[0]);
		return trimStart(s, [&](int c) -> int { 
			for (const auto& ch : predicate) {
				if (ch == c) return 1;
			}
			return 0;
		});
	}

	inline std::string trimEnd(std::string_view s, std::function<int(int)> predicate = std::isspace) {
		auto it = std::find_if_not(s.rbegin(), s.rend(), [&](char c) { return predicate(c); });
		return std::string(s.begin(), it.base());
	}

	inline std::string trimEnd(std::string_view s, char predicate) {
		return trimEnd(s, [&](int c) -> int { return (int)predicate == c; });
	}

	inline std::string trimEnd(std::string_view s, std::string_view predicate) {
		if (predicate.size() == 1) return trimEnd(s, predicate[0]);
		return trimEnd(s, [&](int c) -> int { 
			for (const auto& ch : predicate) {
				if (ch == c) return 1;
			}
			return 0; 
		});
	}

	inline std::string trimEnd(std::string_view s, std::span<const char> predicate) {;
		if (predicate.size() == 1) return trimEnd(s, predicate[0]);
		return trimEnd(s, [&](int c) -> int { 
			for (const auto& ch : predicate) {
				if (ch == c) return 1;
			}
			return 0;
		});
	}

	inline std::string trim(std::string_view s, std::function<int(int)> predicate = std::isspace) {
		return trimEnd(trimStart(s, predicate), predicate);
	}

	inline std::string trim(std::string_view s, char predicate) {
		return trim(s, [&](int c) -> int { return (int)predicate == c; });
	}

	inline std::string trim(std::string_view s, std::string_view predicate) {
		if (predicate.size() == 1) return trim(s, predicate[0]);
		return trim(s, [&](int c) -> int { 
			for (const auto& ch : predicate) {
				if (ch == c) return 1;
			}
			return 0; 
		});
	}

	inline std::string trim(std::string_view s, std::span<const char> predicate) {;
		if (predicate.size() == 1) return trim(s, predicate[0]);
		return trim(s, [&](int c) -> int { 
			for (const auto& ch : predicate) {
				if (ch == c) return 1;
			}
			return 0;
		});
	}

	inline std::vector<std::string> split(std::string_view s, char delimiter) {
		std::vector<std::string> result;
		size_t start = 0;
		size_t end;
		while ((end = s.find(delimiter, start)) != std::string_view::npos) {
			result.emplace_back(s.substr(start, end - start));
			start = end + 1;
		}
		result.emplace_back(s.substr(start));
		return result;
	}

	inline std::vector<std::string> split(std::string_view s, std::string_view delimiter) {
		std::vector<std::string> result;
		size_t start = 0;
		size_t end;
		while ((end = s.find(delimiter, start)) != std::string_view::npos) {
			result.emplace_back(s.substr(start, end - start));
			start = end + delimiter.size(); // advance by full delimiter length
		}
		result.emplace_back(s.substr(start));
		return result;
	}

	inline std::string join(const std::vector<std::string>& strings, std::string_view sep) {
		std::string result;
		for (size_t i = 0; i < strings.size(); ++i) {
			result += strings[i];
			if (i + 1 < strings.size()) result += sep;
		}
		return result;
	}

	inline std::string toUpper(std::string s) {
		std::string dest(s.size(), '\0');
		std::transform(s.begin(), s.end(), dest.begin(), [](unsigned char c) { return std::toupper(c); });
		return dest;
	}

	inline std::string toLower(std::string s) {
		std::string dest(s.size(), '\0');
		std::transform(s.begin(), s.end(), dest.begin(), [](unsigned char c) { return std::tolower(c); });
		return dest;
	}

	inline std::string capitalize(std::string s) {
		std::string dest = s;
		if (!dest.empty()) dest[0] = std::toupper(static_cast<unsigned char>(dest[0]));
		return dest;
	}

	inline bool contains(std::string_view s, std::string_view sub) {
		return s.find(sub) != std::string_view::npos;
	}

	inline bool startsWith(std::string_view s, std::string_view prefix) {
		return s.size() >= prefix.size() && s.substr(0, prefix.size()) == prefix;
	}

	inline bool endsWith(std::string_view s, std::string_view suffix) {
		return s.size() >= suffix.size() && s.substr(s.size() - suffix.size()) == suffix;
	}

	inline std::string replace(std::string s, std::string_view from, std::string_view to) {
		size_t pos = 0;
		while ((pos = s.find(from, pos)) != std::string::npos) {
			s.replace(pos, from.size(), to);
			pos += to.size();
		}
		return s;
	}

	inline std::string remove(std::string s, std::string_view target) {
		return replace(s, target, "");
	}

	inline std::string substrSafe(std::string_view s, size_t start, size_t length) {
		if (start >= s.size()) return "";
		return std::string(s.substr(start, std::min(length, s.size() - start)));
	}

	inline size_t findFirst(std::string_view s, char c) {
		return s.find(c);
	}

	inline size_t findLast(std::string_view s, char c) {
		return s.rfind(c);
	}

	inline std::string repeat(std::string_view s, size_t times) {
		std::string result;
		result.reserve(s.size() * times);
		for (size_t i = 0; i < times; ++i) result += s;
		return result;
	}

	inline bool isIdent(std::string_view s) {
		if (s.empty()) return false;
		for (const auto& c : s) {
			if ((!std::isalnum(c)) && c != '_') return false;
		}
		if (std::isdigit(s[0])) return false;
		return true;
	}

    // Concept: arithmetic types (int, float, etc.)
    template<typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;

    // Generic toString
    template <Arithmetic T>
    std::string toString(const T& value) {
        return std::to_string(value); // fast path
    }

    template <typename T>
    std::string toString(const T& value, const std::string& mod) {
        return std::vformat("{" + mod + "}", std::make_format_args(value)); // fallback for everything else
    }

    template <typename T>
    std::string toString(const T& value) {
        return std::format("{}", value);
    }

} // namespace easy_fmt::string_utils