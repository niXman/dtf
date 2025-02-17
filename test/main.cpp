
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

#include <cassert>
#include <cstring>

#ifdef NDEBUG
#   error "This file MUST be compiled with NDEBUG undefined!"
#endif

/*************************************************************************************************/

struct testvals {
    std::size_t line;
    std::size_t flags;
    std::string exp_flagsstr;
    const char *exp_str;
    std::size_t exp_len;
};

template<std::size_t N>
testvals make_val(std::size_t line, std::size_t flags, std::string flagsstr, const char (&expstr)[N]) {
    return {line, flags, flagsstr, expstr, N-1};
}

testvals test(char *buf, std::uint64_t ts, std::size_t f, const std::string &flagsstr) {
    auto n = dtf::to_dt_chars(buf, ts, f);
    buf[n] = 0;

    return {0, f, flagsstr, buf, n};
}

/*************************************************************************************************/

int main() {
    if ( 0 ) {
        static const char *date_year = "2019";
        static const char *date_month= "01";
        static const char *date_day  = "08";

        static const char *time_hours = "16";
        static const char *time_mins = "50";
        static const char *time_secs = "23";
        static const char *time_msecs = ".006";
        static const char *time_usecs = ".006057";
        static const char *time_nsecs = ".006057057";

        const auto replace_substr = [](std::string s, const std::string &search, const std::string &replace ) {
            for( std::size_t pos = 0u; ; pos += replace.length() ) {
                // Locate the substring to replace
                pos = s.find(search, pos);
                if( pos == s.npos )
                    break;

                // Replace by erasing and inserting
                s.erase(pos, search.length());
                s.insert(pos, replace);
            }

            return s;
        };
        const auto print_date = [](std::uint32_t flags){
            const char sep = (flags & dtf::date_sep_dash)
                ? '-'
                : (flags & dtf::date_sep_point)
                    ? '.'
                    : '~' // no separator
            ;

            if ( sep == '~' ) {
                (flags & dtf::yyyy_mm_dd)
                    ? std::printf("%s%s%s", date_year, date_month, date_day)
                    : std::printf("%s%s%s", date_day, date_month, date_year)
                ;
            } else {
                (flags & dtf::yyyy_mm_dd)
                    ? std::printf("%s%c%s%c%s", date_year, sep, date_month, sep, date_day)
                    : std::printf("%s%c%s%c%s", date_day, sep, date_month, sep, date_year)
                ;
            }
        };
        const auto print_time = [](std::uint32_t flags){
            const char sep = (flags & dtf::time_sep_colon)
                ? ':'
                : (flags & dtf::time_sep_point)
                    ? '.'
                    : '~' // no separator
            ;

            if ( sep == '~' ) {
                std::printf("%s%s%s", time_hours, time_mins, time_secs);
            } else {
                std::printf("%s%c%s%c%s", time_hours, sep, time_mins, sep, time_secs);
            }

            constexpr auto prec_mask = dtf::secs | dtf::msecs | dtf::usecs | dtf::nsecs;
            const auto prec = flags & prec_mask;
            assert(prec);
            switch ( prec ) {
                case dtf::secs: { break; }
                case dtf::msecs: { std::printf("%s", time_msecs); break; }
                case dtf::usecs: { std::printf("%s", time_usecs); break; }
                case dtf::nsecs: { std::printf("%s", time_nsecs); break; }
            }

        };

        std::uint32_t flags = 0u;
        // date format
        const std::uint32_t date_fmt[] = { dtf::yyyy_mm_dd, dtf::dd_mm_yyyy };
        for ( const auto dfmt_it: date_fmt ) {
            flags = dfmt_it;

            // date sep format
            const std::uint32_t datesep_fmt[] = { dtf::date_sep_dash, dtf::date_sep_point, dtf::date_sep_empty };
            for ( const auto datesep_it: datesep_fmt ) {
                flags |= datesep_it;

                // date-time sep format
                const std::uint32_t dtsep_fmt[] = { dtf::dt_sep_T, dtf::dt_sep_t, dtf::dt_sep_space, dtf::dt_sep_underscore, dtf::dt_sep_slash };
                for ( const auto dtsep_it: dtsep_fmt ) {
                    flags |= dtsep_it;

                    // time sep format
                    const std::uint32_t timesep_fmt[] = { dtf::time_sep_colon, dtf::time_sep_point, dtf::time_sep_empty };
                    for ( const auto timesep_it: timesep_fmt ) {
                        flags |= timesep_it;

                        // time prec
                        const std::uint32_t timeprec_fmt[] = { dtf::secs, dtf::msecs, dtf::usecs, dtf::nsecs };
                        for ( const auto timeprec_it: timeprec_fmt ) {
                            flags |= timeprec_it;

                            std::ostringstream os;
                            dtf::dump_flags(os, flags, true);
                            auto flags_str = os.str();
                            flags_str = replace_substr(std::move(flags_str), ", ", " | ");

                            std::printf(",make_val(\n"
                                        "    __LINE__\n");
                            std::printf("   ,%s\n", flags_str.c_str());

                            std::ostringstream os2;
                            dtf::dump_flags(os2, flags);
                            std::printf("   ,\"%s\"\n", os2.str().c_str());

                            std::printf("   ,\""); // double-quote opens
                            print_date(flags);

                            constexpr auto dt_sep_mask = dtf::dt_sep_T | dtf::dt_sep_t | dtf::dt_sep_space | dtf::dt_sep_underscore | dtf::dt_sep_slash;
                            const auto dt_sep = flags & dt_sep_mask;
                            switch ( dt_sep ) {
                                case dtf::dt_sep_T: { std::printf("%c", 'T'); break; }
                                case dtf::dt_sep_t: { std::printf("%c", 't'); break; }
                                case dtf::dt_sep_space: { std::printf("%c", ' '); break; }
                                case dtf::dt_sep_underscore: { std::printf("%c", '_'); break; }
                                case dtf::dt_sep_slash: { std::printf("%c", '/'); break; }
                            }

                            print_time(flags);
                            std::printf("\""); // double-quote closes

                            std::printf("\n)\n");

                            flags = flags ^ timeprec_it;
                        }

                        flags = flags ^ timesep_it;
                    }

                    flags = flags ^ dtsep_it;
                }

                flags = flags ^ datesep_it;
            }
        }

        return 0;
    }

    static const auto ts = 1546966223006057057ull; // 2019-01-08 16:50:23.006057057
    static const testvals vals[] = {
        make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_colon | dtf::secs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_T, time_sep_colon, secs"
            ,"2019-01-08T16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_colon | dtf::msecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_T, time_sep_colon, msecs"
            ,"2019-01-08T16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_colon | dtf::usecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_T, time_sep_colon, usecs"
            ,"2019-01-08T16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_colon | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_T, time_sep_colon, nsecs"
            ,"2019-01-08T16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_point | dtf::secs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_T, time_sep_point, secs"
            ,"2019-01-08T16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_point | dtf::msecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_T, time_sep_point, msecs"
            ,"2019-01-08T16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_point | dtf::usecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_T, time_sep_point, usecs"
            ,"2019-01-08T16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_point | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_T, time_sep_point, nsecs"
            ,"2019-01-08T16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_empty | dtf::secs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_T, time_sep_empty, secs"
            ,"2019-01-08T165023"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_empty | dtf::msecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_T, time_sep_empty, msecs"
            ,"2019-01-08T165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_empty | dtf::usecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_T, time_sep_empty, usecs"
            ,"2019-01-08T165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_empty | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_T, time_sep_empty, nsecs"
            ,"2019-01-08T165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_colon | dtf::secs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_space, time_sep_colon, secs"
            ,"2019-01-08 16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_colon | dtf::msecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_space, time_sep_colon, msecs"
            ,"2019-01-08 16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_colon | dtf::usecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_space, time_sep_colon, usecs"
            ,"2019-01-08 16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_colon | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_space, time_sep_colon, nsecs"
            ,"2019-01-08 16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_point | dtf::secs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_space, time_sep_point, secs"
            ,"2019-01-08 16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_point | dtf::msecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_space, time_sep_point, msecs"
            ,"2019-01-08 16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_point | dtf::usecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_space, time_sep_point, usecs"
            ,"2019-01-08 16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_point | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_space, time_sep_point, nsecs"
            ,"2019-01-08 16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_empty | dtf::secs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_space, time_sep_empty, secs"
            ,"2019-01-08 165023"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_empty | dtf::msecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_space, time_sep_empty, msecs"
            ,"2019-01-08 165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_empty | dtf::usecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_space, time_sep_empty, usecs"
            ,"2019-01-08 165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_empty | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_space, time_sep_empty, nsecs"
            ,"2019-01-08 165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_colon | dtf::secs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_underscore, time_sep_colon, secs"
            ,"2019-01-08_16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_colon | dtf::msecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_underscore, time_sep_colon, msecs"
            ,"2019-01-08_16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_colon | dtf::usecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_underscore, time_sep_colon, usecs"
            ,"2019-01-08_16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_colon | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_underscore, time_sep_colon, nsecs"
            ,"2019-01-08_16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_point | dtf::secs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_underscore, time_sep_point, secs"
            ,"2019-01-08_16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_point | dtf::msecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_underscore, time_sep_point, msecs"
            ,"2019-01-08_16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_point | dtf::usecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_underscore, time_sep_point, usecs"
            ,"2019-01-08_16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_point | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_underscore, time_sep_point, nsecs"
            ,"2019-01-08_16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_empty | dtf::secs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_underscore, time_sep_empty, secs"
            ,"2019-01-08_165023"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_empty | dtf::msecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_underscore, time_sep_empty, msecs"
            ,"2019-01-08_165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_empty | dtf::usecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_underscore, time_sep_empty, usecs"
            ,"2019-01-08_165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_empty | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_underscore, time_sep_empty, nsecs"
            ,"2019-01-08_165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_colon | dtf::secs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_slash, time_sep_colon, secs"
            ,"2019-01-08/16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_colon | dtf::msecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_slash, time_sep_colon, msecs"
            ,"2019-01-08/16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_colon | dtf::usecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_slash, time_sep_colon, usecs"
            ,"2019-01-08/16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_colon | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_slash, time_sep_colon, nsecs"
            ,"2019-01-08/16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_point | dtf::secs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_slash, time_sep_point, secs"
            ,"2019-01-08/16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_point | dtf::msecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_slash, time_sep_point, msecs"
            ,"2019-01-08/16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_point | dtf::usecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_slash, time_sep_point, usecs"
            ,"2019-01-08/16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_point | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_slash, time_sep_point, nsecs"
            ,"2019-01-08/16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_empty | dtf::secs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_slash, time_sep_empty, secs"
            ,"2019-01-08/165023"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_empty | dtf::msecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_slash, time_sep_empty, msecs"
            ,"2019-01-08/165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_empty | dtf::usecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_slash, time_sep_empty, usecs"
            ,"2019-01-08/165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_empty | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_dash, dt_sep_slash, time_sep_empty, nsecs"
            ,"2019-01-08/165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_colon | dtf::secs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_T, time_sep_colon, secs"
            ,"2019.01.08T16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_colon | dtf::msecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_T, time_sep_colon, msecs"
            ,"2019.01.08T16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_colon | dtf::usecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_T, time_sep_colon, usecs"
            ,"2019.01.08T16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_colon | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_T, time_sep_colon, nsecs"
            ,"2019.01.08T16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_point | dtf::secs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_T, time_sep_point, secs"
            ,"2019.01.08T16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_point | dtf::msecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_T, time_sep_point, msecs"
            ,"2019.01.08T16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_point | dtf::usecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_T, time_sep_point, usecs"
            ,"2019.01.08T16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_point | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_T, time_sep_point, nsecs"
            ,"2019.01.08T16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_empty | dtf::secs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_T, time_sep_empty, secs"
            ,"2019.01.08T165023"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_empty | dtf::msecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_T, time_sep_empty, msecs"
            ,"2019.01.08T165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_empty | dtf::usecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_T, time_sep_empty, usecs"
            ,"2019.01.08T165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_empty | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_T, time_sep_empty, nsecs"
            ,"2019.01.08T165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_colon | dtf::secs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_t, time_sep_colon, secs"
            ,"2019.01.08t16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_colon | dtf::msecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_t, time_sep_colon, msecs"
            ,"2019.01.08t16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_colon | dtf::usecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_t, time_sep_colon, usecs"
            ,"2019.01.08t16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_colon | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_t, time_sep_colon, nsecs"
            ,"2019.01.08t16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_point | dtf::secs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_t, time_sep_point, secs"
            ,"2019.01.08t16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_point | dtf::msecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_t, time_sep_point, msecs"
            ,"2019.01.08t16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_point | dtf::usecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_t, time_sep_point, usecs"
            ,"2019.01.08t16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_point | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_t, time_sep_point, nsecs"
            ,"2019.01.08t16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_empty | dtf::secs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_t, time_sep_empty, secs"
            ,"2019.01.08t165023"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_empty | dtf::msecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_t, time_sep_empty, msecs"
            ,"2019.01.08t165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_empty | dtf::usecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_t, time_sep_empty, usecs"
            ,"2019.01.08t165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_empty | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_t, time_sep_empty, nsecs"
            ,"2019.01.08t165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_colon | dtf::secs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_space, time_sep_colon, secs"
            ,"2019.01.08 16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_colon | dtf::msecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_space, time_sep_colon, msecs"
            ,"2019.01.08 16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_colon | dtf::usecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_space, time_sep_colon, usecs"
            ,"2019.01.08 16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_colon | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_space, time_sep_colon, nsecs"
            ,"2019.01.08 16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_point | dtf::secs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_space, time_sep_point, secs"
            ,"2019.01.08 16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_point | dtf::msecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_space, time_sep_point, msecs"
            ,"2019.01.08 16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_point | dtf::usecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_space, time_sep_point, usecs"
            ,"2019.01.08 16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_point | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_space, time_sep_point, nsecs"
            ,"2019.01.08 16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_empty | dtf::secs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_space, time_sep_empty, secs"
            ,"2019.01.08 165023"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_empty | dtf::msecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_space, time_sep_empty, msecs"
            ,"2019.01.08 165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_empty | dtf::usecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_space, time_sep_empty, usecs"
            ,"2019.01.08 165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_empty | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_space, time_sep_empty, nsecs"
            ,"2019.01.08 165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_colon | dtf::secs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_underscore, time_sep_colon, secs"
            ,"2019.01.08_16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_colon | dtf::msecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_underscore, time_sep_colon, msecs"
            ,"2019.01.08_16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_colon | dtf::usecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_underscore, time_sep_colon, usecs"
            ,"2019.01.08_16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_colon | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_underscore, time_sep_colon, nsecs"
            ,"2019.01.08_16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_point | dtf::secs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_underscore, time_sep_point, secs"
            ,"2019.01.08_16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_point | dtf::msecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_underscore, time_sep_point, msecs"
            ,"2019.01.08_16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_point | dtf::usecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_underscore, time_sep_point, usecs"
            ,"2019.01.08_16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_point | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_underscore, time_sep_point, nsecs"
            ,"2019.01.08_16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_empty | dtf::secs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_underscore, time_sep_empty, secs"
            ,"2019.01.08_165023"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_empty | dtf::msecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_underscore, time_sep_empty, msecs"
            ,"2019.01.08_165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_empty | dtf::usecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_underscore, time_sep_empty, usecs"
            ,"2019.01.08_165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_empty | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_underscore, time_sep_empty, nsecs"
            ,"2019.01.08_165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_colon | dtf::secs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_slash, time_sep_colon, secs"
            ,"2019.01.08/16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_colon | dtf::msecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_slash, time_sep_colon, msecs"
            ,"2019.01.08/16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_colon | dtf::usecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_slash, time_sep_colon, usecs"
            ,"2019.01.08/16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_colon | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_slash, time_sep_colon, nsecs"
            ,"2019.01.08/16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_point | dtf::secs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_slash, time_sep_point, secs"
            ,"2019.01.08/16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_point | dtf::msecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_slash, time_sep_point, msecs"
            ,"2019.01.08/16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_point | dtf::usecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_slash, time_sep_point, usecs"
            ,"2019.01.08/16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_point | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_slash, time_sep_point, nsecs"
            ,"2019.01.08/16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_empty | dtf::secs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_slash, time_sep_empty, secs"
            ,"2019.01.08/165023"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_empty | dtf::msecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_slash, time_sep_empty, msecs"
            ,"2019.01.08/165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_empty | dtf::usecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_slash, time_sep_empty, usecs"
            ,"2019.01.08/165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_empty | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_point, dt_sep_slash, time_sep_empty, nsecs"
            ,"2019.01.08/165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_empty | dtf::dt_sep_T | dtf::time_sep_colon | dtf::secs
            ,"yyyy_mm_dd, date_sep_empty, dt_sep_T, time_sep_colon, secs"
            ,"20190108T16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_empty | dtf::dt_sep_T | dtf::time_sep_colon | dtf::msecs
            ,"yyyy_mm_dd, date_sep_empty, dt_sep_T, time_sep_colon, msecs"
            ,"20190108T16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_empty | dtf::dt_sep_T | dtf::time_sep_colon | dtf::usecs
            ,"yyyy_mm_dd, date_sep_empty, dt_sep_T, time_sep_colon, usecs"
            ,"20190108T16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_empty | dtf::dt_sep_T | dtf::time_sep_colon | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_empty, dt_sep_T, time_sep_colon, nsecs"
            ,"20190108T16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_empty | dtf::dt_sep_T | dtf::time_sep_point | dtf::secs
            ,"yyyy_mm_dd, date_sep_empty, dt_sep_T, time_sep_point, secs"
            ,"20190108T16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_empty | dtf::dt_sep_T | dtf::time_sep_point | dtf::msecs
            ,"yyyy_mm_dd, date_sep_empty, dt_sep_T, time_sep_point, msecs"
            ,"20190108T16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_empty | dtf::dt_sep_T | dtf::time_sep_point | dtf::usecs
            ,"yyyy_mm_dd, date_sep_empty, dt_sep_T, time_sep_point, usecs"
            ,"20190108T16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_empty | dtf::dt_sep_T | dtf::time_sep_point | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_empty, dt_sep_T, time_sep_point, nsecs"
            ,"20190108T16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_empty | dtf::dt_sep_T | dtf::time_sep_empty | dtf::secs
            ,"yyyy_mm_dd, date_sep_empty, dt_sep_T, time_sep_empty, secs"
            ,"20190108T165023"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_empty | dtf::dt_sep_T | dtf::time_sep_empty | dtf::msecs
            ,"yyyy_mm_dd, date_sep_empty, dt_sep_T, time_sep_empty, msecs"
            ,"20190108T165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_empty | dtf::dt_sep_T | dtf::time_sep_empty | dtf::usecs
            ,"yyyy_mm_dd, date_sep_empty, dt_sep_T, time_sep_empty, usecs"
            ,"20190108T165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::yyyy_mm_dd | dtf::date_sep_empty | dtf::dt_sep_T | dtf::time_sep_empty | dtf::nsecs
            ,"yyyy_mm_dd, date_sep_empty, dt_sep_T, time_sep_empty, nsecs"
            ,"20190108T165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_colon | dtf::secs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_T, time_sep_colon, secs"
            ,"08-01-2019T16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_colon | dtf::msecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_T, time_sep_colon, msecs"
            ,"08-01-2019T16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_colon | dtf::usecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_T, time_sep_colon, usecs"
            ,"08-01-2019T16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_colon | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_T, time_sep_colon, nsecs"
            ,"08-01-2019T16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_point | dtf::secs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_T, time_sep_point, secs"
            ,"08-01-2019T16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_point | dtf::msecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_T, time_sep_point, msecs"
            ,"08-01-2019T16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_point | dtf::usecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_T, time_sep_point, usecs"
            ,"08-01-2019T16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_point | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_T, time_sep_point, nsecs"
            ,"08-01-2019T16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_empty | dtf::secs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_T, time_sep_empty, secs"
            ,"08-01-2019T165023"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_empty | dtf::msecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_T, time_sep_empty, msecs"
            ,"08-01-2019T165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_empty | dtf::usecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_T, time_sep_empty, usecs"
            ,"08-01-2019T165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_T | dtf::time_sep_empty | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_T, time_sep_empty, nsecs"
            ,"08-01-2019T165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_t | dtf::time_sep_colon | dtf::secs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_t, time_sep_colon, secs"
            ,"08-01-2019t16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_t | dtf::time_sep_colon | dtf::msecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_t, time_sep_colon, msecs"
            ,"08-01-2019t16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_t | dtf::time_sep_colon | dtf::usecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_t, time_sep_colon, usecs"
            ,"08-01-2019t16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_t | dtf::time_sep_colon | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_t, time_sep_colon, nsecs"
            ,"08-01-2019t16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_t | dtf::time_sep_point | dtf::secs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_t, time_sep_point, secs"
            ,"08-01-2019t16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_t | dtf::time_sep_point | dtf::msecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_t, time_sep_point, msecs"
            ,"08-01-2019t16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_t | dtf::time_sep_point | dtf::usecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_t, time_sep_point, usecs"
            ,"08-01-2019t16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_t | dtf::time_sep_point | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_t, time_sep_point, nsecs"
            ,"08-01-2019t16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_t | dtf::time_sep_empty | dtf::secs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_t, time_sep_empty, secs"
            ,"08-01-2019t165023"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_t | dtf::time_sep_empty | dtf::msecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_t, time_sep_empty, msecs"
            ,"08-01-2019t165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_t | dtf::time_sep_empty | dtf::usecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_t, time_sep_empty, usecs"
            ,"08-01-2019t165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_t | dtf::time_sep_empty | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_t, time_sep_empty, nsecs"
            ,"08-01-2019t165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_colon | dtf::secs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_space, time_sep_colon, secs"
            ,"08-01-2019 16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_colon | dtf::msecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_space, time_sep_colon, msecs"
            ,"08-01-2019 16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_colon | dtf::usecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_space, time_sep_colon, usecs"
            ,"08-01-2019 16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_colon | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_space, time_sep_colon, nsecs"
            ,"08-01-2019 16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_point | dtf::secs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_space, time_sep_point, secs"
            ,"08-01-2019 16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_point | dtf::msecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_space, time_sep_point, msecs"
            ,"08-01-2019 16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_point | dtf::usecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_space, time_sep_point, usecs"
            ,"08-01-2019 16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_point | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_space, time_sep_point, nsecs"
            ,"08-01-2019 16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_empty | dtf::secs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_space, time_sep_empty, secs"
            ,"08-01-2019 165023"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_empty | dtf::msecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_space, time_sep_empty, msecs"
            ,"08-01-2019 165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_empty | dtf::usecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_space, time_sep_empty, usecs"
            ,"08-01-2019 165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_space | dtf::time_sep_empty | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_space, time_sep_empty, nsecs"
            ,"08-01-2019 165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_colon | dtf::secs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_underscore, time_sep_colon, secs"
            ,"08-01-2019_16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_colon | dtf::msecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_underscore, time_sep_colon, msecs"
            ,"08-01-2019_16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_colon | dtf::usecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_underscore, time_sep_colon, usecs"
            ,"08-01-2019_16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_colon | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_underscore, time_sep_colon, nsecs"
            ,"08-01-2019_16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_point | dtf::secs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_underscore, time_sep_point, secs"
            ,"08-01-2019_16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_point | dtf::msecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_underscore, time_sep_point, msecs"
            ,"08-01-2019_16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_point | dtf::usecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_underscore, time_sep_point, usecs"
            ,"08-01-2019_16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_point | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_underscore, time_sep_point, nsecs"
            ,"08-01-2019_16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_empty | dtf::secs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_underscore, time_sep_empty, secs"
            ,"08-01-2019_165023"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_empty | dtf::msecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_underscore, time_sep_empty, msecs"
            ,"08-01-2019_165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_empty | dtf::usecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_underscore, time_sep_empty, usecs"
            ,"08-01-2019_165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_underscore | dtf::time_sep_empty | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_underscore, time_sep_empty, nsecs"
            ,"08-01-2019_165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_colon | dtf::secs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_slash, time_sep_colon, secs"
            ,"08-01-2019/16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_colon | dtf::msecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_slash, time_sep_colon, msecs"
            ,"08-01-2019/16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_colon | dtf::usecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_slash, time_sep_colon, usecs"
            ,"08-01-2019/16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_colon | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_slash, time_sep_colon, nsecs"
            ,"08-01-2019/16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_point | dtf::secs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_slash, time_sep_point, secs"
            ,"08-01-2019/16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_point | dtf::msecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_slash, time_sep_point, msecs"
            ,"08-01-2019/16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_point | dtf::usecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_slash, time_sep_point, usecs"
            ,"08-01-2019/16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_point | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_slash, time_sep_point, nsecs"
            ,"08-01-2019/16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_empty | dtf::secs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_slash, time_sep_empty, secs"
            ,"08-01-2019/165023"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_empty | dtf::msecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_slash, time_sep_empty, msecs"
            ,"08-01-2019/165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_empty | dtf::usecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_slash, time_sep_empty, usecs"
            ,"08-01-2019/165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_dash | dtf::dt_sep_slash | dtf::time_sep_empty | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_dash, dt_sep_slash, time_sep_empty, nsecs"
            ,"08-01-2019/165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_colon | dtf::secs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_T, time_sep_colon, secs"
            ,"08.01.2019T16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_colon | dtf::msecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_T, time_sep_colon, msecs"
            ,"08.01.2019T16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_colon | dtf::usecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_T, time_sep_colon, usecs"
            ,"08.01.2019T16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_colon | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_T, time_sep_colon, nsecs"
            ,"08.01.2019T16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_point | dtf::secs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_T, time_sep_point, secs"
            ,"08.01.2019T16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_point | dtf::msecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_T, time_sep_point, msecs"
            ,"08.01.2019T16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_point | dtf::usecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_T, time_sep_point, usecs"
            ,"08.01.2019T16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_point | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_T, time_sep_point, nsecs"
            ,"08.01.2019T16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_empty | dtf::secs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_T, time_sep_empty, secs"
            ,"08.01.2019T165023"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_empty | dtf::msecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_T, time_sep_empty, msecs"
            ,"08.01.2019T165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_empty | dtf::usecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_T, time_sep_empty, usecs"
            ,"08.01.2019T165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_T | dtf::time_sep_empty | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_T, time_sep_empty, nsecs"
            ,"08.01.2019T165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_colon | dtf::secs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_t, time_sep_colon, secs"
            ,"08.01.2019t16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_colon | dtf::msecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_t, time_sep_colon, msecs"
            ,"08.01.2019t16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_colon | dtf::usecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_t, time_sep_colon, usecs"
            ,"08.01.2019t16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_colon | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_t, time_sep_colon, nsecs"
            ,"08.01.2019t16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_point | dtf::secs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_t, time_sep_point, secs"
            ,"08.01.2019t16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_point | dtf::msecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_t, time_sep_point, msecs"
            ,"08.01.2019t16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_point | dtf::usecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_t, time_sep_point, usecs"
            ,"08.01.2019t16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_point | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_t, time_sep_point, nsecs"
            ,"08.01.2019t16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_empty | dtf::secs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_t, time_sep_empty, secs"
            ,"08.01.2019t165023"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_empty | dtf::msecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_t, time_sep_empty, msecs"
            ,"08.01.2019t165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_empty | dtf::usecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_t, time_sep_empty, usecs"
            ,"08.01.2019t165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_t | dtf::time_sep_empty | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_t, time_sep_empty, nsecs"
            ,"08.01.2019t165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_colon | dtf::secs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_space, time_sep_colon, secs"
            ,"08.01.2019 16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_colon | dtf::msecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_space, time_sep_colon, msecs"
            ,"08.01.2019 16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_colon | dtf::usecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_space, time_sep_colon, usecs"
            ,"08.01.2019 16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_colon | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_space, time_sep_colon, nsecs"
            ,"08.01.2019 16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_point | dtf::secs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_space, time_sep_point, secs"
            ,"08.01.2019 16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_point | dtf::msecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_space, time_sep_point, msecs"
            ,"08.01.2019 16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_point | dtf::usecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_space, time_sep_point, usecs"
            ,"08.01.2019 16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_point | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_space, time_sep_point, nsecs"
            ,"08.01.2019 16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_empty | dtf::secs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_space, time_sep_empty, secs"
            ,"08.01.2019 165023"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_empty | dtf::msecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_space, time_sep_empty, msecs"
            ,"08.01.2019 165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_empty | dtf::usecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_space, time_sep_empty, usecs"
            ,"08.01.2019 165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_space | dtf::time_sep_empty | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_space, time_sep_empty, nsecs"
            ,"08.01.2019 165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_colon | dtf::secs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_underscore, time_sep_colon, secs"
            ,"08.01.2019_16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_colon | dtf::msecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_underscore, time_sep_colon, msecs"
            ,"08.01.2019_16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_colon | dtf::usecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_underscore, time_sep_colon, usecs"
            ,"08.01.2019_16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_colon | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_underscore, time_sep_colon, nsecs"
            ,"08.01.2019_16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_point | dtf::secs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_underscore, time_sep_point, secs"
            ,"08.01.2019_16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_point | dtf::msecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_underscore, time_sep_point, msecs"
            ,"08.01.2019_16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_point | dtf::usecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_underscore, time_sep_point, usecs"
            ,"08.01.2019_16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_point | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_underscore, time_sep_point, nsecs"
            ,"08.01.2019_16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_empty | dtf::secs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_underscore, time_sep_empty, secs"
            ,"08.01.2019_165023"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_empty | dtf::msecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_underscore, time_sep_empty, msecs"
            ,"08.01.2019_165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_empty | dtf::usecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_underscore, time_sep_empty, usecs"
            ,"08.01.2019_165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_underscore | dtf::time_sep_empty | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_underscore, time_sep_empty, nsecs"
            ,"08.01.2019_165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_colon | dtf::secs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_slash, time_sep_colon, secs"
            ,"08.01.2019/16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_colon | dtf::msecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_slash, time_sep_colon, msecs"
            ,"08.01.2019/16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_colon | dtf::usecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_slash, time_sep_colon, usecs"
            ,"08.01.2019/16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_colon | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_slash, time_sep_colon, nsecs"
            ,"08.01.2019/16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_point | dtf::secs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_slash, time_sep_point, secs"
            ,"08.01.2019/16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_point | dtf::msecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_slash, time_sep_point, msecs"
            ,"08.01.2019/16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_point | dtf::usecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_slash, time_sep_point, usecs"
            ,"08.01.2019/16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_point | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_slash, time_sep_point, nsecs"
            ,"08.01.2019/16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_empty | dtf::secs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_slash, time_sep_empty, secs"
            ,"08.01.2019/165023"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_empty | dtf::msecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_slash, time_sep_empty, msecs"
            ,"08.01.2019/165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_empty | dtf::usecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_slash, time_sep_empty, usecs"
            ,"08.01.2019/165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_point | dtf::dt_sep_slash | dtf::time_sep_empty | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_point, dt_sep_slash, time_sep_empty, nsecs"
            ,"08.01.2019/165023.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_empty | dtf::dt_sep_t | dtf::time_sep_colon | dtf::secs
            ,"dd_mm_yyyy, date_sep_empty, dt_sep_t, time_sep_colon, secs"
            ,"08012019t16:50:23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_empty | dtf::dt_sep_t | dtf::time_sep_colon | dtf::msecs
            ,"dd_mm_yyyy, date_sep_empty, dt_sep_t, time_sep_colon, msecs"
            ,"08012019t16:50:23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_empty | dtf::dt_sep_t | dtf::time_sep_colon | dtf::usecs
            ,"dd_mm_yyyy, date_sep_empty, dt_sep_t, time_sep_colon, usecs"
            ,"08012019t16:50:23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_empty | dtf::dt_sep_t | dtf::time_sep_colon | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_empty, dt_sep_t, time_sep_colon, nsecs"
            ,"08012019t16:50:23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_empty | dtf::dt_sep_t | dtf::time_sep_point | dtf::secs
            ,"dd_mm_yyyy, date_sep_empty, dt_sep_t, time_sep_point, secs"
            ,"08012019t16.50.23"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_empty | dtf::dt_sep_t | dtf::time_sep_point | dtf::msecs
            ,"dd_mm_yyyy, date_sep_empty, dt_sep_t, time_sep_point, msecs"
            ,"08012019t16.50.23.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_empty | dtf::dt_sep_t | dtf::time_sep_point | dtf::usecs
            ,"dd_mm_yyyy, date_sep_empty, dt_sep_t, time_sep_point, usecs"
            ,"08012019t16.50.23.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_empty | dtf::dt_sep_t | dtf::time_sep_point | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_empty, dt_sep_t, time_sep_point, nsecs"
            ,"08012019t16.50.23.006057057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_empty | dtf::dt_sep_t | dtf::time_sep_empty | dtf::secs
            ,"dd_mm_yyyy, date_sep_empty, dt_sep_t, time_sep_empty, secs"
            ,"08012019t165023"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_empty | dtf::dt_sep_t | dtf::time_sep_empty | dtf::msecs
            ,"dd_mm_yyyy, date_sep_empty, dt_sep_t, time_sep_empty, msecs"
            ,"08012019t165023.006"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_empty | dtf::dt_sep_t | dtf::time_sep_empty | dtf::usecs
            ,"dd_mm_yyyy, date_sep_empty, dt_sep_t, time_sep_empty, usecs"
            ,"08012019t165023.006057"
        )
        ,make_val(
            __LINE__
            ,dtf::dd_mm_yyyy | dtf::date_sep_empty | dtf::dt_sep_t | dtf::time_sep_empty | dtf::nsecs
            ,"dd_mm_yyyy, date_sep_empty, dt_sep_t, time_sep_empty, nsecs"
            ,"08012019t165023.006057057"
        )
    };

    {
        auto tt = dtf::to_time_t(ts);
        auto dwn = static_cast<std::uint32_t>(ts / 1000000000ull);
        assert(tt == dwn);
    }

    std::cout << "Tisting dtf::dump_flags()..." << std::flush;
    for ( const auto &it: vals ) {
        std::ostringstream os;
        dtf::dump_flags(os, it.flags);
        bool equal = os.str() == it.exp_flagsstr;
        if ( !equal ) {
            std::cout
                << std::endl
                << "LINE: " << it.line << std::endl
                << "expected: " << it.exp_flagsstr << std::endl
                << "got     : " << os.str() << std::endl
            ;
            assert(equal);
        }
    }
    std::cout << "DONE!" << std::endl;

    std::cout << "Testing dtf::get_flags()..." << std::flush;
    for ( const auto &it: vals ) {
        auto f = dtf::get_flags(it.exp_str, it.exp_len);
        assert(f != 0);
        bool equal = it.flags == f;
        if ( !equal ) {
            std::cout
                << std::endl
                << "LINE: " << it.line << std::endl
                << "expected: " << it.exp_flagsstr << std::endl
                << "got     : "
            ;
            dtf::dump_flags(std::cout, f) << std::endl;
            assert(equal);
        }
    }
    std::cout << "DONE!" << std::endl;

    std::cout << "Testing dtf::to_dt_chars()..." << std::flush;
    for ( const auto &it: vals ) {
        char buf[dtf::bufsize];
        auto r = test(buf, ts, it.flags, it.exp_flagsstr);
        assert(r.exp_len == it.exp_len);
        bool equal = std::strcmp(r.exp_str, it.exp_str) == 0;
        if ( !equal ) {
            std::cout
                << std::endl
                << "LINE: " << it.line << std::endl
                << "expected: " << it.exp_str << std::endl
                << "got     : " << r.exp_str << std::endl
            ;
            assert(equal);
        }
    }
    std::cout << "DONE!" << std::endl;

    return 0;
}

/*************************************************************************************************/
