
// MIT License
//
// Copyright (c) 2019-2025 niXman (github dot nixman at pm dot me)
// All rights reserved.
//
// This file is part of DTF(https://github.com/niXman/dtf) project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef __dtf__dtf_hpp
#define __dtf__dtf_hpp

#include <string>
#include <ostream>
#include <chrono>

#include <cstdint>
#include <ctime>
#include <cassert>
#include <cstring>

namespace dtf {

/*************************************************************************************************/

// always in nanoseconds resolution
std::uint64_t timestamp(int offset_in_hours = 0);

// converts to `time_t`
std::time_t to_time_t(std::uint64_t ts);

// converts from `time_t`
std::uint64_t from_time_t(std::time_t ts);

// fmt flags
enum flags: std::uint32_t {
     yyyy_mm_dd        = 1u << 0u  // yyyy-mm-dd
    ,dd_mm_yyyy        = 1u << 1u  // dd-mm-yyyy
    ,date_sep_dash     = 1u << 2u  // 2018-12-11/yyyy-mm-dd
    ,date_sep_point    = 1u << 3u  // 2018.12.11/yyyy.mm.dd
    ,date_sep_empty    = 1u << 4u  // 20181211/yyyymmdd
    ,dt_sep_T          = 1u << 5u  // 2018-12-11T13:58:59 (required when `yyyy_mm_dd || date_sep_empty`)
    ,dt_sep_t          = 1u << 6u  // 2018-12-11t13:58:59 (required when `dd_mm_yyyy || date_sep_empty`)
    ,dt_sep_space      = 1u << 7u  // 2018-12-11 13:58:59
    ,dt_sep_underscore = 1u << 8u  // 2018-12-11_13:58:59
    ,dt_sep_slash      = 1u << 9u  // 2018-12-11/13:58:59
    ,time_sep_colon    = 1u << 10u // 13:58:59/hh:mm:ss
    ,time_sep_point    = 1u << 11u // 13.58.59/hh.mm.ss
    ,time_sep_empty    = 1u << 12u // 135859/hhmmss
    ,secs              = 1u << 13u // seconds resolution
    ,msecs             = 1u << 14u // milliseconds resolution
    ,usecs             = 1u << 15u // microseconds resolution
    ,nsecs             = 1u << 16u // nanoseconds resolution
};

// NOTE:
// when specified `date_sep_empty`:
// for `yyyy_mm_dd` format the `dt_sep_T` MUST be specified too!
// for `dd_mm_yyyy` format the `dt_sep_t` MUST be specified too!

// the required size of buffer for the min length of string representation
// WITHOUT advanced subseconds precision:
// yyyy(4) + mm(2) + dd(2) + `date-time sep`(1)
// + hh(2) + mm(2) + ss(2) = 15

// the required size of buffer for the min length of string representation
// WITH advanced subseconds precision in nanoseconds:
// yyyy(4) + mm(2) + dd(2) + `date-time sep`(1)
// + hh(2) + mm(2) + ss(2) + `period char when advanced resolution specified`(1)
// + `nanoseconds counter`(9) = 25

// the required size of buffer for the max length of string representation:
// yyyy(4) + `date sep`(1) + mm(2) + `date sep`(1) + dd(2) + `date-time sep`(1)
// + hh(2) + `time sep`(1) + mm(2) + `time sep`(1) + ss(2) + `period char when advanced resolution specified`(1)
// + `nanoseconds counter`(9) = 29
enum {
     bufsize = 32
    ,bufsize_min = 15
    ,bufsize_max = 29
};

constexpr auto default_flags =
    flags::yyyy_mm_dd |
    flags::date_sep_dash |
    flags::dt_sep_slash |
    flags::time_sep_colon |
    flags::msecs
;

// format as number in C-string representation.
// returns the num of chars placed.
// `buf` - the destination buffer with at least `dtf::bufsize` bytes.
std::size_t to_chars(char *buf, std::uint64_t ts, std::uint32_t flags = default_flags);

std::string to_str(std::uint64_t ts, std::uint32_t flags = default_flags);

std::string to_str(std::uint32_t flags = default_flags, int offset_in_hours = 0);

// formats as date-time string.
// returns the num of chars placed.
// `buf` - the destination buffer with at least `dtf::bufsize` bytes.
std::size_t to_dt_chars(char *buf, std::uint64_t ts, std::uint32_t flags = default_flags);

std::string to_dt_str(std::uint64_t ts, std::uint32_t flags = default_flags);

std::string dt_str(std::uint32_t flags = default_flags, int offset_in_hours = 0);

/*************************************************************************************************/

// gets the respective flags using given date-time string (DTF format only!)
std::uint32_t get_flags(const char *buf, std::size_t n);

std::uint32_t get_flags(const std::string &str);

// dump the flags
std::ostream& dump_flags(std::ostream &os, std::uint32_t flags, bool with_dtf_prefix = false);

/*************************************************************************************************/

} // ns dtf

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

namespace dtf {

#if __cplusplus >= 201703L
#   define __DTF_FALLTHROUGH [[fallthrough]]
#else
#   if defined(__GNUC__) || defined(__clang__)
#       define __DTF_FALLTHROUGH __attribute__((fallthrough))
#   else
#       define __DTF_FALLTHROUGH
#   endif
#endif // __cplusplus >= 201703L

#define __DTF_NSECS_PER_SEC 1000000000ull
#define __DTF_SECS_PER_MIN  60u
#define __DTF_MINS_PER_HOUR 60u
#define __DTF_HOURS_PER_DAY 24u
#define __DTF_SECS_PER_HOUR (__DTF_SECS_PER_MIN * __DTF_MINS_PER_HOUR)
#define __DTF_SECS_PER_DAY  (__DTF_SECS_PER_HOUR * __DTF_HOURS_PER_DAY)
#define __DTF_DAYS_PER_WEEK 7u
#define __DTF_MONS_PER_YEAR 12u

#define __DTF_EPOCH_ADJUSTMENT_DAYS 719468
#define __DTF_ADJUSTED_EPOCH_YEAR   0
#define __DTF_DAYS_PER_ERA          146097
#define __DTF_DAYS_PER_4_YEARS      (3 * 365 + 366)
#define __DTF_DAYS_PER_YEAR         365
#define __DTF_DAYS_PER_CENTURY      36524
#define __DTF_YEARS_PER_ERA         400

#define __DTF_DHMS(p, v) \
    *p++ = static_cast<char>((v / 10) % 10) + '0'; \
    *p++ = static_cast<char>((v % 10)) + '0';

#define __DTF_YEAR(p, v) \
    *p++ = static_cast<char>((v / 1000) % 10) + '0'; \
    *p++ = static_cast<char>((v / 100) % 10) + '0'; \
    __DTF_DHMS(p, v)

#define __DTF_MONTH(p, v) \
    *p++ = static_cast<char>(((v + 1) / 10) % 10) + '0'; \
    *p++ = static_cast<char>(((v + 1)) % 10) + '0';

#define __DTF_DATE_SEP_IS_DASH(ch) (ch == '-')
#define __DTF_DATE_SEP_IS_POINT(ch) (ch == '.')
#define __DTF_IS_DATE_SEPARATOR(ch) \
    (__DTF_DATE_SEP_IS_DASH(ch) || __DTF_DATE_SEP_IS_POINT(ch))

#define __DTF_TIME_SEP_IS_COLON(ch) (ch == ':')
#define __DTF_TIME_SEP_IS_POINT(ch) (ch == '.')
#define __DTF_IS_TIME_SEPARATOR(ch) \
    (__DTF_TIME_SEP_IS_COLON(ch) || __DTF_TIME_SEP_IS_POINT(ch))

#define __DTF_DATETIME_SEP_IS_T(ch) (ch == 'T')
#define __DTF_DATETIME_SEP_IS_t(ch) (ch == 't')
#define __DTF_DATETIME_SEP_IS_SPACE(ch) (ch == ' ')
#define __DTF_DATETIME_SEP_IS_UNDERSCORE(ch) (ch == '_')
#define __DTF_DATETIME_SEP_IS_SLASH(ch) (ch == '/')

#define __DTF_IS_DT_SEPARATOR(ch) \
    (__DTF_DATETIME_SEP_IS_T(ch) \
        || __DTF_DATETIME_SEP_IS_t(ch) \
        || __DTF_DATETIME_SEP_IS_SPACE(ch) \
        || __DTF_DATETIME_SEP_IS_UNDERSCORE(ch) \
        || __DTF_DATETIME_SEP_IS_SLASH(ch) \
    )

#define __DTF_IS_DIGIT(ch) (ch >= '0' && ch <= '9')

#define __DTF_IS_DIGIT_3(p) \
    (__DTF_IS_DIGIT((p)[0]) && __DTF_IS_DIGIT((p)[1]) && __DTF_IS_DIGIT((p)[2]))

#define __DTF_IS_DIGIT_6(p) \
    (__DTF_IS_DIGIT_3(p+0) && __DTF_IS_DIGIT_3(p+3))

#define __DTF_IS_DIGIT_9(p) \
    (__DTF_IS_DIGIT_6(p+0) && __DTF_IS_DIGIT_3(p+6))

#define __DTF_IS_YYYY_MM_DD_SEP_VALID(p) \
    (__DTF_IS_DIGIT(p[0]) && __DTF_IS_DIGIT(p[1]) && __DTF_IS_DIGIT(p[2]) && __DTF_IS_DIGIT(p[3]) \
        && __DTF_IS_DIGIT(p[5]) && __DTF_IS_DIGIT(p[6]) \
        && __DTF_IS_DIGIT(p[8]) && __DTF_IS_DIGIT(p[9]) \
    )
#define __DTF_IS_YYYY_MM_DD_NOSEP_VALID(p) \
    (__DTF_IS_DIGIT(p[0]) && __DTF_IS_DIGIT(p[1]) && __DTF_IS_DIGIT(p[2]) && __DTF_IS_DIGIT(p[3]) \
        && __DTF_IS_DIGIT(p[4]) && __DTF_IS_DIGIT(p[5]) \
        && __DTF_IS_DIGIT(p[6]) && __DTF_IS_DIGIT(p[7]) \
    )

#define __DTF_IS_DD_MM_YYYY_SEP_VALID(p) \
    (__DTF_IS_DIGIT(p[0]) && __DTF_IS_DIGIT(p[1]) \
        && __DTF_IS_DIGIT(p[3]) && __DTF_IS_DIGIT(p[4]) \
        && __DTF_IS_DIGIT(p[6]) && __DTF_IS_DIGIT(p[7]) && __DTF_IS_DIGIT(p[8]) && __DTF_IS_DIGIT(p[9]) \
    )
#define __DTF_IS_DD_MM_YYYY_NOSEP_VALID(p) \
    (__DTF_IS_DIGIT(p[0]) && __DTF_IS_DIGIT(p[1]) \
        && __DTF_IS_DIGIT(p[2]) && __DTF_IS_DIGIT(p[3]) \
        && __DTF_IS_DIGIT(p[4]) && __DTF_IS_DIGIT(p[5]) && __DTF_IS_DIGIT(p[6]) && __DTF_IS_DIGIT(p[7]) \
    )

#define __DTF_IS_TIME_SEP_VALID(p) \
    (__DTF_IS_DIGIT(p[0]) && __DTF_IS_DIGIT(p[1]) \
        && __DTF_IS_DIGIT(p[3]) && __DTF_IS_DIGIT(p[4]) \
        && __DTF_IS_DIGIT(p[6]) && __DTF_IS_DIGIT(p[7]) \
    )

#define __DTF_IS_TIME_NOSEP_VALID(p) \
    (__DTF_IS_DIGIT(p[0]) && __DTF_IS_DIGIT(p[1]) \
        && __DTF_IS_DIGIT(p[2]) && __DTF_IS_DIGIT(p[3]) \
        && __DTF_IS_DIGIT(p[4]) && __DTF_IS_DIGIT(p[5]) \
    )

/*************************************************************************************************/

inline std::uint64_t timestamp(int offset_in_hours) {
    auto ts = static_cast<std::uint64_t>(
        std::chrono::duration_cast<
            std::chrono::nanoseconds
        >(std::chrono::system_clock::now().time_since_epoch()).count()
    );

    std::uint64_t val = 60 * 60 * __DTF_NSECS_PER_SEC
        * static_cast<std::uint64_t>(std::abs(offset_in_hours));

    ts = (offset_in_hours < 0) ? ts - val : ts + val;

    return ts;
}

/*************************************************************************************************/

inline std::time_t to_time_t(std::uint64_t ts) {
    return static_cast<std::time_t>(ts / __DTF_NSECS_PER_SEC);
}

inline std::uint64_t from_time_t(std::time_t ts) {
    return static_cast<std::uint64_t>(ts * __DTF_NSECS_PER_SEC);
}

/*************************************************************************************************/

static std::size_t num_chars(std::size_t v) {
    std::size_t n = 1;
    v = (v >= 100000000000000000ull) ? ((n += 17), (v / 100000000000000000ull)) : v;
    v = (v >= 100000000ull) ? ((n += 8), (v / 100000000ull)) : v;
    v = (v >= 10000ull) ? ((n += 4), (v / 10000ull)) : v;
    v = (v >= 100ull) ? ((n += 2), (v / 100ull)) : v;
    n = (v >= 10ull) ? n + 1 : n;

    return n;
}

static void utoa(char *ptr, std::size_t n, std::uint64_t v) {
    char *p = ptr + n - 1;
    switch ( n ) {
        case 20: *p-- = static_cast<char>('0'+(v % 10)); v /= 10; __DTF_FALLTHROUGH;
        case 19: *p-- = static_cast<char>('0'+(v % 10)); v /= 10; __DTF_FALLTHROUGH;
        case 18: *p-- = static_cast<char>('0'+(v % 10)); v /= 10; __DTF_FALLTHROUGH;
        case 17: *p-- = static_cast<char>('0'+(v % 10)); v /= 10; __DTF_FALLTHROUGH;
        case 16: *p-- = static_cast<char>('0'+(v % 10)); v /= 10; __DTF_FALLTHROUGH;
        case 15: *p-- = static_cast<char>('0'+(v % 10)); v /= 10; __DTF_FALLTHROUGH;
        case 14: *p-- = static_cast<char>('0'+(v % 10)); v /= 10; __DTF_FALLTHROUGH;
        case 13: *p-- = static_cast<char>('0'+(v % 10)); v /= 10; __DTF_FALLTHROUGH;
        case 12: *p-- = static_cast<char>('0'+(v % 10)); v /= 10; __DTF_FALLTHROUGH;
        case 11: *p-- = static_cast<char>('0'+(v % 10)); v /= 10; __DTF_FALLTHROUGH;
        case 10: *p-- = static_cast<char>('0'+(v % 10)); v /= 10; __DTF_FALLTHROUGH;
        case 9 : *p-- = static_cast<char>('0'+(v % 10)); v /= 10; __DTF_FALLTHROUGH;
        case 8 : *p-- = static_cast<char>('0'+(v % 10)); v /= 10; __DTF_FALLTHROUGH;
        case 7 : *p-- = static_cast<char>('0'+(v % 10)); v /= 10; __DTF_FALLTHROUGH;
        case 6 : *p-- = static_cast<char>('0'+(v % 10)); v /= 10; __DTF_FALLTHROUGH;
        case 5 : *p-- = static_cast<char>('0'+(v % 10)); v /= 10; __DTF_FALLTHROUGH;
        case 4 : *p-- = static_cast<char>('0'+(v % 10)); v /= 10; __DTF_FALLTHROUGH;
        case 3 : *p-- = static_cast<char>('0'+(v % 10)); v /= 10; __DTF_FALLTHROUGH;
        case 2 : *p-- = static_cast<char>('0'+(v % 10)); v /= 10; __DTF_FALLTHROUGH;
        case 1 : *p-- = static_cast<char>('0'+(v % 10)); v /= 10;
    }
}

/*************************************************************************************************/

inline std::size_t to_chars(char *buf, std::uint64_t ts, std::uint32_t f) {
    ts = (f & flags::secs)
    ? ts / 1000000000ull
    : (f & flags::msecs)
        ? ts / 1000000ull
        : (f & flags::usecs)
            ? ts / 1000ull
            : ts
    ;

    const auto n = num_chars(ts);
    utoa(buf, n, ts);

    return n;
}

inline std::string to_str(std::uint64_t ts, std::uint32_t f) {
    std::string res;
    res.reserve(bufsize);

    const auto n = to_chars(std::addressof(res[0]), ts, f);
    res.resize(n);

    return res;
}

inline std::string to_str(std::uint32_t f, int offset_in_hours) {
    const auto ts = timestamp(offset_in_hours);
    return to_str(ts, f);
}

/*************************************************************************************************/

inline std::size_t to_dt_chars(char *ptr, std::uint64_t ts, std::uint32_t f) {
    constexpr auto date_fmt_mask = yyyy_mm_dd | dd_mm_yyyy;
    assert(f & date_fmt_mask && "the date format MUST be specified");
    constexpr auto time_prec_mask = secs | msecs | usecs | nsecs;
    assert(f & time_prec_mask && "the time precision MUST be specified");

    constexpr char empty_char = '~';
    constexpr char no_sep_specified = '`';
    const char datesep = (f & flags::date_sep_dash)
        ? '-'
        : (f & flags::date_sep_point)
            ? '.'
            : (f & flags::date_sep_empty)
                ? empty_char
                : no_sep_specified
    ;
    assert(datesep != no_sep_specified && "the separator type for date MUST be specified!");

    const char timesep = (f & flags::time_sep_colon)
        ? ':'
        : (f & flags::time_sep_point)
            ? '.'
            : (f & flags::time_sep_empty)
                ? empty_char
                : no_sep_specified
    ;
    assert(timesep != no_sep_specified && "the separator type for time MUST be specified!");

    const char dtsep = (f & flags::dt_sep_T)
        ? 'T'
        : (f & flags::dt_sep_t)
            ? 't'
            : (f & flags::dt_sep_space)
                ? ' '
                : (f & flags::dt_sep_underscore)
                    ? '_'
                    : (f & flags::dt_sep_slash)
                        ? '/'
                        : no_sep_specified
    ;
    assert(dtsep != no_sep_specified && "the separator type for date-time MUST be specified!");
    assert(datesep == empty_char ? (dtsep == 'T' || dtsep == 't') : true);

    std::uint32_t ss = ts / __DTF_NSECS_PER_SEC;
    std::uint32_t ps = ts % __DTF_NSECS_PER_SEC;

    // based on: https://howardhinnant.github.io/date_algorithms.html#civil_from_days
    int days = ss / __DTF_SECS_PER_DAY + __DTF_EPOCH_ADJUSTMENT_DAYS;
    ss %= __DTF_SECS_PER_DAY;
    std::size_t hours = ss / __DTF_SECS_PER_HOUR;
    ss %= __DTF_SECS_PER_HOUR;
    std::size_t mins = ss / __DTF_SECS_PER_MIN;
    std::size_t secs = ss % __DTF_SECS_PER_MIN;
    std::size_t era = (days >= 0 ? days : days - (__DTF_DAYS_PER_ERA - 1)) / __DTF_DAYS_PER_ERA;
    std::size_t eraday = days - era * __DTF_DAYS_PER_ERA;
    std::size_t erayear = (eraday - eraday / (__DTF_DAYS_PER_4_YEARS - 1) + eraday / __DTF_DAYS_PER_CENTURY -
        eraday / (__DTF_DAYS_PER_ERA - 1)) / 365;
    std::size_t yearday = eraday - (__DTF_DAYS_PER_YEAR * erayear + erayear / 4 - erayear / 100);
    std::size_t month = (5 * yearday + 2) / 153;
    std::size_t day = yearday - (153 * month + 2) / 5 + 1;
    month += month < 10 ? 2 : -10;
    std::size_t year = __DTF_ADJUSTED_EPOCH_YEAR + erayear + era * __DTF_YEARS_PER_ERA + static_cast<int>(month <= 1);

    char *p = ptr;
    if ( f & flags::yyyy_mm_dd ) {
        if ( f & flags::date_sep_empty ) {
            assert(dtsep == 'T' && "'T' MUST be used as date-time separator when used `flags::yyyy_mm_dd|flags::date_sep_empty`");
        }
        __DTF_YEAR(p, year);
        if ( datesep != empty_char ) { *p++ = datesep; }
        __DTF_MONTH(p, month);
        if ( datesep != empty_char ) { *p++ = datesep; }
        __DTF_DHMS(p, day);
    } else if ( f & flags::dd_mm_yyyy ) {
        if ( f & flags::date_sep_empty ) {
            assert(dtsep == 't' && "'t' MUST be used as date-time separator when used `flags::dd_mm_yyyy|flags::date_sep_empty`");
        }
        __DTF_DHMS(p, day);
        if ( datesep != empty_char ) { *p++ = datesep; }
        __DTF_MONTH(p, month);
        if ( datesep != empty_char ) { *p++ = datesep; }
        __DTF_YEAR(p, year);
    } else {
        assert(!"unreachable");
    }

    *p++ = dtsep;

    __DTF_DHMS(p, hours);
    if ( timesep != empty_char ) { *p++ = timesep; }
    __DTF_DHMS(p, mins);
    if ( timesep != empty_char ) { *p++ = timesep; }
    __DTF_DHMS(p, secs);

    const auto pi = (f & flags::secs)
        ? std::make_pair(static_cast<std::uint32_t>(ps / 1000000000), 0u)
        : (f & flags::msecs)
            ? std::make_pair(static_cast<std::uint32_t>(ps / 1000000), 3u)
            : (f & flags::usecs)
                ? std::make_pair(static_cast<std::uint32_t>(ps / 1000), 6u)
                : std::make_pair(ps, 9u)
    ;
    if ( pi.second ) {
        *p++ = '.';

        auto n = num_chars(pi.first);
        for ( auto cnt = pi.second - n; cnt; --cnt ) {
            *p++ = '0';
        }
        utoa(p, n, pi.first);
        p += n;
    }

    assert((p - ptr) > 0);
    return static_cast<std::size_t>(p - ptr);
}

/*************************************************************************************************/

inline std::string to_dt_str(std::uint64_t ts, std::uint32_t f) {
    std::string res;
    res.resize(bufsize);

    const auto n = to_dt_chars(std::addressof(res[0]), ts, f);
    res.resize(n);

    return res;
}

inline std::string dt_str(std::uint32_t f, int offset_in_hours) {
    const auto ts = timestamp(offset_in_hours);
    return to_dt_str(ts, f);
}

/*************************************************************************************************/

inline std::uint32_t get_flags(const char *buf, std::size_t len) {
    std::uint32_t res{};

    if ( len < 15 || len > 29 ) {
        return 0u;
    }

    const auto ch2 = buf[2];
    const auto ch4 = buf[4];
    const auto ch8 = buf[8];
    const auto ch10 = buf[10];
    const auto dt_sep_pos = __DTF_IS_DT_SEPARATOR(ch8)
        ? 8u
        : __DTF_IS_DT_SEPARATOR(ch10)
            ? 10u
            : 0u
    ;
    assert(dt_sep_pos != 0);

    // date sep processing
    if ( dt_sep_pos == 8u ) {
        // date without seperators
        bool dt_sep_T = __DTF_DATETIME_SEP_IS_T(ch8);
        bool dt_sep_t = __DTF_DATETIME_SEP_IS_t(ch8);
        res |= flags::date_sep_empty;
        res |= dt_sep_T
            ? flags::dt_sep_T
            : dt_sep_t
                ? flags::dt_sep_t
                : __DTF_DATETIME_SEP_IS_SPACE(ch8)
                    ? flags::dt_sep_space
                    : __DTF_DATETIME_SEP_IS_UNDERSCORE(ch8)
                        ? flags::dt_sep_underscore
                        : flags::dt_sep_slash
        ;

        if ( dt_sep_T ) {
            bool valid = __DTF_IS_YYYY_MM_DD_NOSEP_VALID(buf);
            if ( !valid ) {
                return 0u;
            }

            res |= flags::yyyy_mm_dd;
        } else {
            bool valid = __DTF_IS_DD_MM_YYYY_NOSEP_VALID(buf);
            if ( !valid ) {
                return 0u;
            }

            res |= flags::dd_mm_yyyy;
        }
    } else {
        // date WITH separators
        if ( __DTF_IS_DATE_SEPARATOR(ch2) ) {
            bool valid = __DTF_IS_DD_MM_YYYY_SEP_VALID(buf);
            if ( !valid ) {
                return 0u;
            }

            res |= flags::dd_mm_yyyy;
            res |= __DTF_DATE_SEP_IS_DASH(ch2)
                ? flags::date_sep_dash
                : flags::date_sep_point
            ;
        } else if ( __DTF_IS_DATE_SEPARATOR(ch4) ) {
            bool valid = __DTF_IS_YYYY_MM_DD_SEP_VALID(buf);
            if ( !valid ) {
                return  0u;
            }

            res |= flags::yyyy_mm_dd;
            res |= __DTF_DATE_SEP_IS_DASH(ch4)
                ? flags::date_sep_dash
                : flags::date_sep_point
            ;
        } else {
            assert(!"unreachable!");
        }

        res |= __DTF_DATETIME_SEP_IS_T(ch10)
            ? flags::dt_sep_T
            : __DTF_DATETIME_SEP_IS_t(ch10)
                ? flags::dt_sep_t
                : __DTF_DATETIME_SEP_IS_SPACE(ch10)
                    ? flags::dt_sep_space
                    : __DTF_DATETIME_SEP_IS_UNDERSCORE(ch10)
                        ? flags::dt_sep_underscore
                        : flags::dt_sep_slash
        ;
    }

    auto eos_time_pos = 0u;
    // time sep processing
    if ( __DTF_IS_TIME_SEPARATOR(buf[dt_sep_pos + 1 + 2]) ) {
        // time WITH separator
        const char *ptr = buf + dt_sep_pos + 1;
        bool valid = __DTF_IS_TIME_SEP_VALID(ptr);
        if ( !valid ) {
            return 0u;
        }

        res |= __DTF_TIME_SEP_IS_COLON(buf[dt_sep_pos + 1 + 2])
            ? flags::time_sep_colon
            : flags::time_sep_point
        ;

        eos_time_pos = dt_sep_pos
            + 1 // next after date-time separator
            + 2 // skip first two chars (hours)
            + 1 // skip next time separator
            + 2 // skip next two chars (mins)
            + 1 // skip next time separator
            + 2 // skip next two chars (secs)
        ;
    } else {
        // time without separator
        const char *ptr = buf + dt_sep_pos + 1;
        bool valid = __DTF_IS_TIME_NOSEP_VALID(ptr);
        if ( !valid ) {
            return 0u;
        }

        res |= flags::time_sep_empty;
        eos_time_pos = dt_sep_pos
            + 1 // next after date-time separator
            + 2 // skip first two chars (hours)
            + 2 // skip next two chars (mins)
            + 2 // skip next two chars (secs)
        ;
    }

    char eos_char = buf[eos_time_pos];
    if ( eos_char == '.' ) {
        auto chars_left = len - eos_time_pos - 1;
        if ( chars_left % 3 != 0 ) {
            return 0u;
        }

        const char *time_fract = buf + eos_time_pos + 1;
        switch ( chars_left ) {
            case 3: {
                if ( ! __DTF_IS_DIGIT_3(time_fract) ) {
                    return 0u;
                }
                res |= flags::msecs;
                break;
            }
            case 6: {
                if ( ! __DTF_IS_DIGIT_6(time_fract) ) {
                    return 0u;
                }
                res |= flags::usecs;
                break;
            }
            case 9: {
                if ( ! __DTF_IS_DIGIT_9(time_fract) ) {
                    return 0u;
                }
                res |= flags::nsecs;
                break;
            }
            default: {
                return 0u;
            }
        }
    } else {
        res |= flags::secs;
    }

    return res;
}

inline std::uint32_t get_flags(const std::string &str) {
    return get_flags(str.c_str(), str.length());
}

/*************************************************************************************************/

inline std::ostream& dump_flags(std::ostream &os, std::uint32_t flags, bool with_dtf_prefix) {
    static const char *arr[] = {
         "yyyy_mm_dd"
        ,"dd_mm_yyyy"
        ,"date_sep_dash"
        ,"date_sep_point"
        ,"date_sep_empty"
        ,"dt_sep_T"
        ,"dt_sep_t"
        ,"dt_sep_space"
        ,"dt_sep_underscore"
        ,"dt_sep_slash"
        ,"time_sep_colon"
        ,"time_sep_point"
        ,"time_sep_empty"
        ,"secs"
        ,"msecs"
        ,"usecs"
        ,"nsecs"
    };

    for ( auto idx = 0u; flags; ++idx ) {
        auto f = (1u << idx);
        if ( flags & f ) {
            if ( with_dtf_prefix ) {
                os << "dtf::";
            }
            os << arr[idx];

            flags = flags ^ f;
            if ( flags ) {
                os << ", ";
            }
        }
    }

    return os;
}

/*************************************************************************************************/

#undef __DTF_FALLTHROUGH
#undef __DTF_NSECS_PER_SEC
#undef __DTF_SECS_PER_MIN
#undef __DTF_MINS_PER_HOUR
#undef __DTF_HOURS_PER_DAY
#undef __DTF_SECS_PER_HOUR
#undef __DTF_SECS_PER_DAY
#undef __DTF_DAYS_PER_WEEK
#undef __DTF_MONS_PER_YEAR
#undef __DTF_EPOCH_ADJUSTMENT_DAYS
#undef __DTF_ADJUSTED_EPOCH_YEAR
#undef __DTF_DAYS_PER_ERA
#undef __DTF_DAYS_PER_4_YEARS
#undef __DTF_DAYS_PER_YEAR
#undef __DTF_DAYS_PER_CENTURY
#undef __DTF_YEARS_PER_ERA
#undef __DTF_DHMS
#undef __DTF_YEAR
#undef __DTF_MONTH
#undef __DTF_DATE_SEP_IS_DASH
#undef __DTF_DATE_SEP_IS_POINT
#undef __DTF_IS_DATE_SEPARATOR
#undef __DTF_TIME_SEP_IS_COLON
#undef __DTF_TIME_SEP_IS_POINT
#undef __DTF_IS_TIME_SEPARATOR
#undef __DTF_DATETIME_SEP_IS_T
#undef __DTF_DATETIME_SEP_IS_t
#undef __DTF_DATETIME_SEP_IS_SPACE
#undef __DTF_DATETIME_SEP_IS_UNDERSCORE
#undef __DTF_DATETIME_SEP_IS_SLASH
#undef __DTF_IS_DT_SEPARATOR
#undef __DTF_IS_DIGIT
#undef __DTF_IS_DIGIT_3
#undef __DTF_IS_DIGIT_6
#undef __DTF_IS_DIGIT_9
#undef __DTF_IS_YYYY_MM_DD_SEP_VALID
#undef __DTF_IS_YYYY_MM_DD_NOSEP_VALID
#undef __DTF_IS_DD_MM_YYYY_SEP_VALID
#undef __DTF_IS_DD_MM_YYYY_NOSEP_VALID
#undef __DTF_IS_TIME_SEP_VALID
#undef __DTF_IS_TIME_NOSEP_VALID

} // ns dtf

/*************************************************************************************************/

#endif // __dtf__dtf_hpp
