/*
 * Copyright (c) 2017-2023 zhllxt
 *
 * author   : zhllxt
 * email    : 37792738@qq.com
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __ASIO2_FMT_HPP__
#define __ASIO2_FMT_HPP__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <asio2/base/detail/push_options.hpp>

#include <string_view>

#ifndef ASIO2_DISABLE_AUTO_HEADER_ONLY
#ifndef FMT_HEADER_ONLY
#define FMT_HEADER_ONLY
#endif
#endif

// used to compatible with the UE4 "check" macro
#pragma push_macro("check")
#undef check

#include <fmt/format.h>
#include <fmt/args.h>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/compile.h>
#include <fmt/os.h>
#include <fmt/ostream.h>
#include <fmt/printf.h>
#include <fmt/ranges.h>
#include <fmt/xchar.h>

// https://fmt.dev/latest/api.html#udt

// Custom format for MFC/ATL CString
#if defined(__AFXSTR_H__) || defined(__ATLSTR_H__)
#if __has_include(<afxstr.h>) || __has_include(<atlstr.h>)

template <>
struct fmt::formatter<CStringA, char>
{
	template<typename FormatParseContext>
	constexpr auto parse(FormatParseContext& ctx) -> decltype(ctx.begin())
	{
		// Parse the presentation format and store it in the formatter:
		auto it = ctx.begin(), end = ctx.end();

		// Check if reached the end of the range:
		if (it != end && *it != '}') throw format_error("invalid format");

		// Return an iterator past the end of the parsed range:
		return it;
	}

	template <typename FormatContext>
	auto format(const CStringA& s, FormatContext& ctx) -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}", (LPCSTR)s);
	}
};

// CStringA s;
// fmt::format(L"{}", s);
// above code will compile failed, beacuse the CStringA can be implicitly converted to const char*
// then then fmt::format(L"{}", (const char*)...); will compile failed.
template <>
struct fmt::formatter<CStringA, wchar_t>
{
	template<typename FormatParseContext>
	constexpr auto parse(FormatParseContext& ctx) -> decltype(ctx.begin())
	{
		// Parse the presentation format and store it in the formatter:
		auto it = ctx.begin(), end = ctx.end();

		// Check if reached the end of the range:
		if (it != end && *it != '}') throw format_error("invalid format");

		// Return an iterator past the end of the parsed range:
		return it;
	}

	template <typename FormatContext>
	auto format(const CStringA& s, FormatContext& ctx) -> decltype(ctx.out())
	{
		return format_to(ctx.out(), L"{}", (LPCWSTR)CStringW(s));
	}
};

template <>
struct fmt::formatter<CStringW, char>
{
	template<typename FormatParseContext>
	constexpr auto parse(FormatParseContext& ctx) -> decltype(ctx.begin())
	{
		// Parse the presentation format and store it in the formatter:
		auto it = ctx.begin(), end = ctx.end();

		// Check if reached the end of the range:
		if (it != end && *it != '}') throw format_error("invalid format");

		// Return an iterator past the end of the parsed range:
		return it;
	}

	template <typename FormatContext>
	auto format(const CStringW& s, FormatContext& ctx) -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}", (LPCSTR)CStringA(s));
	}
};

template <>
struct fmt::formatter<CStringW, wchar_t>
{
	template<typename FormatParseContext>
	constexpr auto parse(FormatParseContext& ctx) -> decltype(ctx.begin())
	{
		// Parse the presentation format and store it in the formatter:
		auto it = ctx.begin(), end = ctx.end();

		// Check if reached the end of the range:
		if (it != end && *it != '}') throw format_error("invalid format");

		// Return an iterator past the end of the parsed range:
		return it;
	}

	template <typename FormatContext>
	auto format(const CStringW& s, FormatContext& ctx) -> decltype(ctx.out())
	{
		return format_to(ctx.out(), L"{}", (LPCWSTR)s);
	}
};

#endif
#endif

// Custom format for wxWidgets wxString
// beacuse the wxString can be implicitly converted to const char* and const wchar_t*
// so the wxString can be use fmt::format("{}", wxString()); directly.
#if defined(_WX_WXSTRING_H__) && defined(ASIO2_ENABLE_WXSTRING_FORMATTER)
#if __has_include(<wx/string.h>)

template <>
struct fmt::formatter<wxString, char>
{
	template<typename FormatParseContext>
	constexpr auto parse(FormatParseContext& ctx) -> decltype(ctx.begin())
	{
		// Parse the presentation format and store it in the formatter:
		auto it = ctx.begin(), end = ctx.end();

		// Check if reached the end of the range:
		if (it != end && *it != '}') throw format_error("invalid format");

		// Return an iterator past the end of the parsed range:
		return it;
	}

	template <typename FormatContext>
	auto format(const wxString& s, FormatContext& ctx) -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}", (const char*)s);
	}
};

template <>
struct fmt::formatter<wxString, wchar_t>
{
	template<typename FormatParseContext>
	constexpr auto parse(FormatParseContext& ctx) -> decltype(ctx.begin())
	{
		// Parse the presentation format and store it in the formatter:
		auto it = ctx.begin(), end = ctx.end();

		// Check if reached the end of the range:
		if (it != end && *it != '}') throw format_error("invalid format");

		// Return an iterator past the end of the parsed range:
		return it;
	}

	template <typename FormatContext>
	auto format(const wxString& s, FormatContext& ctx) -> decltype(ctx.out())
	{
		return format_to(ctx.out(), L"{}", (const wchar_t*)s);
	}
};

#endif
#endif

#pragma pop_macro("check")

#include <asio2/base/detail/pop_options.hpp>

#endif // !__ASIO2_FMT_HPP__
