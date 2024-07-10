
// MIT License
//
// Copyright (c) 2024 niXman (i dotty nixman doggy gmail dotty com)
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

#include "dtf.hpp"

#include <iostream>
#include <cassert>
#include <cstring>

/*************************************************************************************************/

struct testvals {
    std::size_t flags;
    std::size_t exlen;
    const char *exstr;
};

testvals test(char *buf, std::uint64_t ts, std::size_t f) {
    auto n = dtf::timestamp_to_dt_chars(buf, ts, f);
    buf[n] = 0;

    return {f, n, buf};
}

/*************************************************************************************************/

int main() {
    static const testvals vals[] = {
         {dtf::flags::yyyy_mm_dd|dtf::flags::sep1|dtf::flags::secs , 19, "2019-01-08 16:50:23"}
        ,{dtf::flags::yyyy_mm_dd|dtf::flags::sep1|dtf::flags::msecs, 23, "2019-01-08 16:50:23.006"}
        ,{dtf::flags::yyyy_mm_dd|dtf::flags::sep1|dtf::flags::usecs, 26, "2019-01-08 16:50:23.006057"}
        ,{dtf::flags::yyyy_mm_dd|dtf::flags::sep1|dtf::flags::nsecs, 29, "2019-01-08 16:50:23.006057057"}
        ,{dtf::flags::yyyy_mm_dd|dtf::flags::sep2|dtf::flags::secs , 19, "2019.01.08-16.50.23"}
        ,{dtf::flags::yyyy_mm_dd|dtf::flags::sep2|dtf::flags::msecs, 23, "2019.01.08-16.50.23.006"}
        ,{dtf::flags::yyyy_mm_dd|dtf::flags::sep2|dtf::flags::usecs, 26, "2019.01.08-16.50.23.006057"}
        ,{dtf::flags::yyyy_mm_dd|dtf::flags::sep2|dtf::flags::nsecs, 29, "2019.01.08-16.50.23.006057057"}
        ,{dtf::flags::yyyy_mm_dd|dtf::flags::sep3|dtf::flags::secs , 19, "2019.01.08-16:50:23"}
        ,{dtf::flags::yyyy_mm_dd|dtf::flags::sep3|dtf::flags::msecs, 23, "2019.01.08-16:50:23.006"}
        ,{dtf::flags::yyyy_mm_dd|dtf::flags::sep3|dtf::flags::usecs, 26, "2019.01.08-16:50:23.006057"}
        ,{dtf::flags::yyyy_mm_dd|dtf::flags::sep3|dtf::flags::nsecs, 29, "2019.01.08-16:50:23.006057057"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep1|dtf::flags::secs , 19, "08-01-2019 16:50:23"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep1|dtf::flags::msecs, 23, "08-01-2019 16:50:23.006"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep1|dtf::flags::usecs, 26, "08-01-2019 16:50:23.006057"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep1|dtf::flags::nsecs, 29, "08-01-2019 16:50:23.006057057"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep2|dtf::flags::secs , 19, "08.01.2019-16.50.23"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep2|dtf::flags::msecs, 23, "08.01.2019-16.50.23.006"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep2|dtf::flags::usecs, 26, "08.01.2019-16.50.23.006057"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep2|dtf::flags::nsecs, 29, "08.01.2019-16.50.23.006057057"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep3|dtf::flags::secs , 19, "08.01.2019-16:50:23"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep3|dtf::flags::msecs, 23, "08.01.2019-16:50:23.006"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep3|dtf::flags::usecs, 26, "08.01.2019-16:50:23.006057"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep3|dtf::flags::nsecs, 29, "08.01.2019-16:50:23.006057057"}
    };

    for ( const auto &it: vals ) {
        static const auto ts = 1546966223006057057ull; // 2019-01-08 16:50:23.006057557
        char buf[dtf::bufsize];
        auto r = test(buf, ts, it.flags);
        assert(r.exlen == it.exlen);
        assert(std::strcmp(r.exstr, it.exstr) == 0);

        auto f = dtf::dt_str_flags(it.exstr, it.exlen);
        assert(it.flags == f);

        //std::cout << r.exstr << std::endl;
    }

    {
        auto v = dtf::timestamp();
        char buf[dtf::bufsize];
        auto n = dtf::timestamp_to_dt_chars(buf, v, dtf::flags::yyyy_mm_dd|dtf::flags::sep1|dtf::flags::msecs);
        buf[n] = 0;
        std::cout << "1: " << buf << std::endl;
        std::cout << "2: " << dtf::timestamp_dt_str() << std::endl;
        std::cout << "4: " << dtf::timestamp_str() << std::endl;
    }
    {
        char buf[dtf::bufsize];
        std::memset(buf, 'F', dtf::bufsize);
        static const auto ts = 1546966223000000000ull; // 2019-01-08 16:50:23.000000000
        auto n = dtf::timestamp_to_dt_chars(buf, ts, dtf::flags::yyyy_mm_dd|dtf::flags::sep1|dtf::flags::msecs);
        buf[n] = 0;
        std::cout << "5: " << buf << std::endl;
    }

    return 0;
}

/*************************************************************************************************/
