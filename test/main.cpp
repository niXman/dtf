
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

struct good_testvals {
    std::size_t case_;
    std::size_t flags;
    const char *exp_flagsstr;
    const char *exp_str;
    std::size_t exp_len;
};

template<std::size_t N>
constexpr good_testvals make_correct_val(std::size_t line, std::size_t flags, const char *flagsstr, const char (&expstr)[N]) {
    return {line, flags, flagsstr, expstr, N-1};
}

struct wrong_testvals {
    std::size_t case_;
    dtf::error expected_error;
    const char *str;
    std::size_t len;
};

template<std::size_t N>
constexpr wrong_testvals make_wrong_val(std::size_t case_, dtf::error err, const char (&str)[N]) {
    return {case_, err, str, N-1};
}

good_testvals test(char *buf, std::uint64_t ts, std::size_t f, const char *flagsstr) {
    auto n = dtf::to_dt_chars(buf, ts, f);
    buf[n] = 0;

    return {0, f, flagsstr, buf, n};
}

/*************************************************************************************************/

int main() {
    static constexpr auto ts = 1546966223006057057ull; // 2019-01-08 16:50:23.006057057
    static constexpr good_testvals good_vals[] = {
        #include "good.inc"
    };

    {
        auto tt = dtf::to_time_t(ts);
        auto dwn = static_cast<std::uint32_t>(ts / 1000000000ull);
        assert(tt == dwn);
    }

    std::cout << "Testing dtf::dump_flags()..." << std::flush;
    for ( const auto &it: good_vals ) {
        std::ostringstream os;
        dtf::dump_flags(os, it.flags);
        bool equal = os.str() == it.exp_flagsstr;
        if ( !equal ) {
            std::cout
                << std::endl
                << "case: " << it.case_ << std::endl
                << "expected: " << it.exp_flagsstr << std::endl
                << "got     : " << os.str() << std::endl
            ;
            assert(equal);
        }
    }
    std::cout << "DONE!" << std::endl;

    std::cout << "Testing dtf::get_flags() with correct DT string..." << std::flush;
    for ( const auto &it: good_vals ) {
        std::uint32_t flags{};
        auto err = dtf::get_flags(&flags, it.exp_str, it.exp_len);
        assert(err == dtf::error::ok);
        bool equal = it.flags == flags;
        if ( !equal ) {
            std::cout
                << std::endl
                << "case: " << it.case_ << std::endl
                << "expected: " << it.exp_flagsstr << std::endl
                << "got     : "
            ;
            dtf::dump_flags(std::cout, flags) << std::endl;
            assert(equal);
        }
    }
    std::cout << "DONE!" << std::endl;


    std::cout << "Testing dtf::get_flags() with wrong DT string..." << std::flush;
    static const wrong_testvals wrong_vals[] = {
        #include "wrong.inc"
    };

    for ( const auto &it: wrong_vals ) {
        std::uint32_t flags{};
        auto err = dtf::get_flags(&flags, it.str, it.len);
        bool equal = err == it.expected_error;
        if ( !equal ) {
            std::cout
                << std::endl
                << "case: " << it.case_ << std::endl
                << "str: \"" << it.str << "\"" << std::endl
                << "expected error: " << static_cast<unsigned>(it.expected_error) << std::endl
                << "got error     : " << static_cast<unsigned>(err) << std::endl
            ;
            assert(equal);
        }
    }
    std::cout << "DONE!" << std::endl;


    std::cout << "Testing dtf::to_dt_chars()..." << std::endl;
    for ( const auto &it: good_vals ) {
        char buf[dtf::bufsize];
        auto r = test(buf, ts, it.flags, it.exp_flagsstr);
        assert(r.exp_len == it.exp_len);
        bool equal = std::strcmp(r.exp_str, it.exp_str) == 0;
        if ( !equal ) {
            std::cout
                << std::endl
                << "case: " << it.case_ << std::endl
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
