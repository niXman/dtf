
// Copyright (c) 2019 niXman (i dotty nixman doggy gmail dotty com)
// All rights reserved.
//
// This file is part of DTF(https://github.com/niXman/dtf) project.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright notice, this
//   list of conditions and the following disclaimer in the documentation and/or
//   other materials provided with the distribution.
//
//   Neither the name of the {organization} nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
    auto n = dtf::timestamp_to_chars(buf, ts, f);
    buf[n] = 0;

    return {f, n, buf};
}

/*************************************************************************************************/

int main() {
    {
        auto v = dtf::timestamp();
        char buf[dtf::bufsize];
        auto n = dtf::timestamp_to_chars(buf, v, dtf::flags::yyyy_mm_dd|dtf::flags::sep1|dtf::flags::msecs);
        buf[n] = 0;

        //std::cout << buf << std::endl;
    }
    static const auto ts = 1546966223006057057ull; // 2019-01-08 16:50:23.006057557
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
        char buf[dtf::bufsize];
        auto r = test(buf, ts, it.flags);
        assert(r.exlen == it.exlen);
        assert(std::strcmp(r.exstr, it.exstr) == 0);

        std::cout << r.exstr << std::endl;
    }

    return 0;
}

/*************************************************************************************************/
