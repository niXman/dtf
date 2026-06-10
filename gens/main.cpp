
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

#include <dtf/dtf.hpp>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

#include <cassert>
#include <cstring>


/*************************************************************************************************/

bool is_good_specified(const char *str) {
    return std::strcmp(str, "good") == 0;
}

bool is_wrong_specified(const char *str) {
    return std::strcmp(str, "wrong") == 0;
}

bool is_valid_flag(const char *str) {
    return is_good_specified(str) || is_wrong_specified(str);
}

/*************************************************************************************************/

std::string replace_substr(std::string s, const std::string &search, const std::string &replace ) {
    for( std::size_t pos = 0u; ; pos += replace.length() ) {
        pos = s.find(search, pos);
        if( pos == s.npos )
            break;

        s.erase(pos, search.length());
        s.insert(pos, replace);
    }

    return s;
}

// 1546966223006057057ull = 2019-01-08 16:50:23.006057057

static const char *date_year = "2019";
static const char *date_month= "01";
static const char *date_day  = "08";

static const char *time_hours = "16";
static const char *time_mins = "50";
static const char *time_secs = "23";
static const char *time_msecs = ".006";
static const char *time_usecs = ".006057";
static const char *time_nsecs = ".006057057";

std::string print_date(std::uint32_t flags) {
    char buf[dtf::bufsize]{};

    const char sep = (flags & dtf::date_sep_dash)
        ? '-'
        : (flags & dtf::date_sep_point)
            ? '.'
            : '~' // no separator
    ;

    if ( sep == '~' ) {
        (flags & dtf::yyyy_mm_dd)
            ? std::snprintf(buf, sizeof(buf), "%s%s%s", date_year, date_month, date_day)
            : std::snprintf(buf, sizeof(buf), "%s%s%s", date_day, date_month, date_year)
        ;
    } else {
        (flags & dtf::yyyy_mm_dd)
            ? std::snprintf(buf, sizeof(buf), "%s%c%s%c%s", date_year, sep, date_month, sep, date_day)
            : std::snprintf(buf, sizeof(buf), "%s%c%s%c%s", date_day, sep, date_month, sep, date_year)
        ;
    }

    return buf;
}

std::string print_time(std::uint32_t flags) {
    char buf[dtf::bufsize]{};
    std::string res;

    const char sep = (flags & dtf::time_sep_colon)
        ? ':'
        : (flags & dtf::time_sep_point)
            ? '.'
            : '~' // no separator
    ;

    if ( sep == '~' ) {
        std::snprintf(buf, sizeof(buf), "%s%s%s", time_hours, time_mins, time_secs);
    } else {
        std::snprintf(buf, sizeof(buf), "%s%c%s%c%s", time_hours, sep, time_mins, sep, time_secs);
    }
    res = buf;

    constexpr auto prec_mask = dtf::secs | dtf::msecs | dtf::usecs | dtf::nsecs;
    const auto prec = flags & prec_mask;
    assert(prec);
    switch ( prec ) {
        case dtf::secs: { break; }
        case dtf::msecs: { res += time_msecs; break; }
        case dtf::usecs: { res += time_usecs; break; }
        case dtf::nsecs: { res += time_nsecs; break; }
    }

    return res;
}

char get_dt_sep_char(std::uint32_t flags) {
    constexpr auto dt_sep_mask =
          dtf::dt_sep_T
        | dtf::dt_sep_t
        | dtf::dt_sep_space
        | dtf::dt_sep_underscore
        | dtf::dt_sep_slash
        | dtf::dt_sep_dash
    ;
    const auto dt_sep = flags & dt_sep_mask;
    switch ( dt_sep ) {
        case dtf::dt_sep_T: return 'T';
        case dtf::dt_sep_t: return 't';
        case dtf::dt_sep_space: return ' ';
        case dtf::dt_sep_underscore: return '_';
        case dtf::dt_sep_slash: return '/';
        case dtf::dt_sep_dash: return '-';
    }
    assert(!"unreachable");
    return '~';
}

/*************************************************************************************************/

void emit_good_case(std::vector<std::string> &dst, std::size_t case_, std::uint32_t flags) {
    std::ostringstream os;
    dtf::dump_flags(os, flags, true);
    auto flags_str = replace_substr(os.str(), ", ", " | ");

    std::ostringstream os2;
    dtf::dump_flags(os2, flags);

    std::string line;
    if ( case_ != 0u ) {
        line += ',';
    }
    line += "make_correct_val(\n    ";
    line += std::to_string(case_);
    line += "\n   ,";
    line += flags_str;
    line += "\n   ,\"";
    line += os2.str();
    line += "\"\n   ,\"";
    line += print_date(flags);
    line += get_dt_sep_char(flags);
    line += print_time(flags);
    line += "\"\n)";

    dst.push_back(std::move(line));
}

void generate_good(std::vector<std::string> &dst) {
    std::size_t case_ = 0u;

    const std::uint32_t date_fmts[] = {dtf::yyyy_mm_dd, dtf::dd_mm_yyyy};
    const std::uint32_t date_seps[] = {dtf::date_sep_dash, dtf::date_sep_point, dtf::date_sep_empty};
    const std::uint32_t dt_seps[]   = {dtf::dt_sep_T, dtf::dt_sep_t, dtf::dt_sep_space, dtf::dt_sep_underscore, dtf::dt_sep_slash, dtf::dt_sep_dash};
    const std::uint32_t time_seps[] = {dtf::time_sep_colon, dtf::time_sep_point, dtf::time_sep_empty};
    const std::uint32_t time_precs[]= {dtf::secs, dtf::msecs, dtf::usecs, dtf::nsecs};

    for ( const auto date_fmt: date_fmts ) {
        for ( const auto date_sep: date_seps ) {
            if ( date_sep == dtf::date_sep_empty ) {
                // yyyy_mm_dd + date_sep_empty requires dt_sep_T
                // dd_mm_yyyy + date_sep_empty requires dt_sep_t
                const std::uint32_t forced_dt_sep = (date_fmt == dtf::yyyy_mm_dd)
                    ? static_cast<std::uint32_t>(dtf::dt_sep_T)
                    : static_cast<std::uint32_t>(dtf::dt_sep_t)
                ;
                for ( const auto time_sep: time_seps ) {
                    for ( const auto time_prec: time_precs ) {
                        std::uint32_t flags = date_fmt | date_sep | forced_dt_sep | time_sep | time_prec;
                        emit_good_case(dst, case_++, flags);
                    }
                }
            } else {
                for ( const auto dt_sep: dt_seps ) {
                    for ( const auto time_sep: time_seps ) {
                        for ( const auto time_prec: time_precs ) {
                            std::uint32_t flags = date_fmt | date_sep | dt_sep | time_sep | time_prec;
                            emit_good_case(dst, case_++, flags);
                        }
                    }
                }
            }
        }
    }
}

/*************************************************************************************************/

const char *error_name(dtf::error e) {
    switch ( e ) {
        case dtf::error::ok:                          return "dtf::error::ok";
        case dtf::error::too_short:                   return "dtf::error::too_short";
        case dtf::error::too_long:                    return "dtf::error::too_long";
        case dtf::error::wrong_dt_sep:                return "dtf::error::wrong_dt_sep";
        case dtf::error::wrong_yyyy_mm_dd_nosep:      return "dtf::error::wrong_yyyy_mm_dd_nosep";
        case dtf::error::wrong_dd_mm_yyyy_nosep:      return "dtf::error::wrong_dd_mm_yyyy_nosep";
        case dtf::error::wrong_dd_mm_yyyy_sep:        return "dtf::error::wrong_dd_mm_yyyy_sep";
        case dtf::error::wrong_yyyy_mm_dd_sep:        return "dtf::error::wrong_yyyy_mm_dd_sep";
        case dtf::error::wrong_time_sep:              return "dtf::error::wrong_time_sep";
        case dtf::error::wrong_time_nosep:            return "dtf::error::wrong_time_nosep";
        case dtf::error::wrong_dt_end_char:           return "dtf::error::wrong_dt_end_char";
        case dtf::error::wrong_num_of_secs_fractions: return "dtf::error::wrong_num_of_secs_fractions";
        case dtf::error::wrong_ms_digits:             return "dtf::error::wrong_ms_digits";
        case dtf::error::wrong_us_digits:             return "dtf::error::wrong_us_digits";
        case dtf::error::wrong_ns_digits:             return "dtf::error::wrong_ns_digits";
    }
    return "dtf::error::ok";
}

void emit_wrong_case(std::vector<std::string> &dst, std::size_t case_, dtf::error err, const char *str) {
    std::string line;
    if ( case_ != 0u ) {
        line += ',';
    }
    line += "make_wrong_val(\n    ";
    line += std::to_string(case_);
    line += "\n   ,";
    line += error_name(err);
    line += "\n   ,\"";
    line += str;
    line += "\"\n)";

    dst.push_back(std::move(line));
}

void generate_wrong(std::vector<std::string> &dst) {
    std::size_t case_ = 0u;

    // too_short: length < 15
    emit_wrong_case(dst, case_++, dtf::error::too_short, "2019-01-08T16:");
    emit_wrong_case(dst, case_++, dtf::error::too_short, "");
    emit_wrong_case(dst, case_++, dtf::error::too_short, "20190108T16502");

    // too_long: length > 29
    emit_wrong_case(dst, case_++, dtf::error::too_long, "2019-01-08T16:50:23.0060570570");
    emit_wrong_case(dst, case_++, dtf::error::too_long, "2019-01-08T16:50:23.00605705700");

    // wrong_dt_sep: no valid dt separator at position 8 or 10
    emit_wrong_case(dst, case_++, dtf::error::wrong_dt_sep, "123456789012345");
    emit_wrong_case(dst, case_++, dtf::error::wrong_dt_sep, "12345678901234567");
    emit_wrong_case(dst, case_++, dtf::error::wrong_dt_sep, "2019X01X08X165023");

    // wrong_yyyy_mm_dd_nosep: 'T' at pos 8 but invalid yyyymmdd digits
    emit_wrong_case(dst, case_++, dtf::error::wrong_yyyy_mm_dd_nosep, "2019X108T165023");
    emit_wrong_case(dst, case_++, dtf::error::wrong_yyyy_mm_dd_nosep, "XXXX0108T165023");
    emit_wrong_case(dst, case_++, dtf::error::wrong_yyyy_mm_dd_nosep, "201901X8T165023");

    // wrong_dd_mm_yyyy_nosep: 't' at pos 8 but invalid ddmmyyyy digits
    emit_wrong_case(dst, case_++, dtf::error::wrong_dd_mm_yyyy_nosep, "08X12019t165023");
    emit_wrong_case(dst, case_++, dtf::error::wrong_dd_mm_yyyy_nosep, "XX012019t165023");
    emit_wrong_case(dst, case_++, dtf::error::wrong_dd_mm_yyyy_nosep, "0801201Xt165023");

    // wrong_dd_mm_yyyy_sep: date sep at pos 2 (dd-mm-yyyy) but invalid digits
    emit_wrong_case(dst, case_++, dtf::error::wrong_dd_mm_yyyy_sep, "0X-01-2019T16:50:23");
    emit_wrong_case(dst, case_++, dtf::error::wrong_dd_mm_yyyy_sep, "08-0X-2019T16:50:23");
    emit_wrong_case(dst, case_++, dtf::error::wrong_dd_mm_yyyy_sep, "08-01-X019T16:50:23");

    // wrong_yyyy_mm_dd_sep: date sep at pos 4 (yyyy-mm-dd) but invalid digits
    emit_wrong_case(dst, case_++, dtf::error::wrong_yyyy_mm_dd_sep, "201X-01-08T16:50:23");
    emit_wrong_case(dst, case_++, dtf::error::wrong_yyyy_mm_dd_sep, "2019-0X-08T16:50:23");
    emit_wrong_case(dst, case_++, dtf::error::wrong_yyyy_mm_dd_sep, "2019-01-X8T16:50:23");

    // wrong_time_sep: time with separator but invalid digits
    emit_wrong_case(dst, case_++, dtf::error::wrong_time_sep, "2019-01-08T1X:50:23");
    emit_wrong_case(dst, case_++, dtf::error::wrong_time_sep, "2019-01-08T16:X0:23");
    emit_wrong_case(dst, case_++, dtf::error::wrong_time_sep, "2019-01-08T16:50:X3");

    // wrong_time_nosep: time without separator but invalid digits
    emit_wrong_case(dst, case_++, dtf::error::wrong_time_nosep, "20190108T1X5023");
    emit_wrong_case(dst, case_++, dtf::error::wrong_time_nosep, "20190108T16X023");
    emit_wrong_case(dst, case_++, dtf::error::wrong_time_nosep, "20190108T1650X3");

    // wrong_dt_end_char: char after time portion is not '.' or '\0'
    emit_wrong_case(dst, case_++, dtf::error::wrong_dt_end_char, "2019-01-08T16:50:23X006");
    emit_wrong_case(dst, case_++, dtf::error::wrong_dt_end_char, "2019-01-08T16:50:23,006");
    emit_wrong_case(dst, case_++, dtf::error::wrong_dt_end_char, "20190108T165023X006");

    // wrong_num_of_secs_fractions: fractional part length not divisible by 3
    emit_wrong_case(dst, case_++, dtf::error::wrong_num_of_secs_fractions, "2019-01-08T16:50:23.00");
    emit_wrong_case(dst, case_++, dtf::error::wrong_num_of_secs_fractions, "2019-01-08T16:50:23.0000");
    emit_wrong_case(dst, case_++, dtf::error::wrong_num_of_secs_fractions, "2019-01-08T16:50:23.00000");

    // wrong_ms_digits: 3 fractional chars with non-digit
    emit_wrong_case(dst, case_++, dtf::error::wrong_ms_digits, "2019-01-08T16:50:23.X06");
    emit_wrong_case(dst, case_++, dtf::error::wrong_ms_digits, "2019-01-08T16:50:23.0X6");
    emit_wrong_case(dst, case_++, dtf::error::wrong_ms_digits, "2019-01-08T16:50:23.00X");

    // wrong_us_digits: 6 fractional chars with non-digit
    emit_wrong_case(dst, case_++, dtf::error::wrong_us_digits, "2019-01-08T16:50:23.X06057");
    emit_wrong_case(dst, case_++, dtf::error::wrong_us_digits, "2019-01-08T16:50:23.006X57");
    emit_wrong_case(dst, case_++, dtf::error::wrong_us_digits, "2019-01-08T16:50:23.00605X");

    // wrong_ns_digits: 9 fractional chars with non-digit
    emit_wrong_case(dst, case_++, dtf::error::wrong_ns_digits, "2019-01-08T16:50:23.X06057057");
    emit_wrong_case(dst, case_++, dtf::error::wrong_ns_digits, "2019-01-08T16:50:23.006057X57");
    emit_wrong_case(dst, case_++, dtf::error::wrong_ns_digits, "2019-01-08T16:50:23.00605705X");
}

/*************************************************************************************************/

void write_file(const char *fname, const std::vector<std::string> &strings) {
    std::ofstream file{fname, std::ios::out|std::ios::trunc};
    for ( const auto &it: strings) {
        file << it << std::endl;
    }
}

/*************************************************************************************************/

int main(int argc, char *const *argv) {
    if ( argc != 2 || !is_valid_flag(argv[1]) ) {
        const char *exename = std::strrchr(argv[0], '/');
        exename = exename ? exename+1 : "gens";

        std::cerr
            << "wrong command line!" << std::endl
            << exename << " <good|wrong>" << std::endl
        ;

        return EXIT_FAILURE;
    }

    const bool is_good = is_good_specified(argv[1]);
    const char *fname = is_good ? "good.inc" : "wrong.inc";
    std::cout << "generating \"" << fname << "\"..." << std::endl;

    std::vector<std::string> strings;
    if ( is_good ) {
        generate_good(strings);
    } else {
        generate_wrong(strings);
    }

    write_file(fname, strings);

    return EXIT_SUCCESS;
}

/*************************************************************************************************/
