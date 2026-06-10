
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
#include <iomanip>
#include <chrono>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>

#ifdef NDEBUG
#   error "This file MUST be compiled with NDEBUG undefined!"
#endif

/*************************************************************************************************/

template<typename T>
static inline void do_not_optimize(const T &v) {
    asm volatile("" : : "r,m"(v) : "memory");
}

template<typename F>
static double bench_ns(std::size_t iters, F &&f) {
    const auto beg = std::chrono::steady_clock::now();
    for ( std::size_t i = 0; i < iters; ++i ) {
        f(i);
    }
    const auto end = std::chrono::steady_clock::now();

    return std::chrono::duration<double, std::nano>(end - beg).count() / static_cast<double>(iters);
}

/*************************************************************************************************/

int main() {
    const std::uint64_t base = dtf::timestamp();
    const std::uint64_t base_sec = base - (base % 1000000000ull);
    const std::uint32_t flags =
         dtf::yyyy_mm_dd
        |dtf::date_sep_point
        |dtf::dt_sep_slash
        |dtf::time_sep_colon
        |dtf::secs
    ;

    std::time_t tt = dtf::to_time_t(base);
    struct tm tmv;
    ::gmtime_r(&tt, &tmv);

    const std::string r_dtf = dtf::to_dt_str(base, flags);

    char sbuf[64];
    std::strftime(sbuf, sizeof(sbuf), "%Y.%m.%d/%H:%M:%S", &tmv);
    const std::string r_strftime = sbuf;

    std::ostringstream oss;
    oss << std::put_time(&tmv, "%Y.%m.%d/%H:%M:%S");
    const std::string r_put_time = oss.str();

    std::cout
        << "dtf::to_dt_chars() : " << r_dtf << '\n'
        << "strftime()         : " << r_strftime << '\n'
        << "put_time()         : " << r_put_time << '\n'
    ;
    assert(r_dtf == r_strftime && r_strftime == r_put_time);

    constexpr std::size_t N_fast = 50000000; // dtf / strftime
    constexpr std::size_t N_slow =  5000000; // put_time (ostream-based, much slower)

    char buf[dtf::bufsize];

    const double dtf_hit = bench_ns(N_fast, [&](std::size_t i) {
        const std::size_t n = dtf::to_dt_chars(buf, base_sec + (i % 1000000000ull), flags);
        do_not_optimize(buf);
        do_not_optimize(n);
    });

    const double dtf_miss = bench_ns(N_fast, [&](std::size_t i) {
        const std::size_t n = dtf::to_dt_chars(buf, base + i * 1000000000ull, flags);
        do_not_optimize(buf);
        do_not_optimize(n);
    });

    const double strftime_ns = bench_ns(N_fast, [&](std::size_t) {
        std::strftime(buf, sizeof(buf), "%Y.%m.%d/%H:%M:%S", &tmv);
        do_not_optimize(buf);
    });

    const double put_time_ns = bench_ns(N_slow, [&](std::size_t) {
        std::ostringstream os;
        os << std::put_time(&tmv, "%Y.%m.%d/%H:%M:%S");
        const std::string s = os.str();
        do_not_optimize(s);
    });

    std::cout
        << std::fixed << std::setprecision(2)
        << "\n"
        << "dtf  (cache hit) : " << dtf_hit    << " ns/call\n"
        << "dtf  (cache miss): " << dtf_miss   << " ns/call\n"
        << "strftime         : " << strftime_ns << " ns/call\n"
        << "put_time         : " << put_time_ns << " ns/call\n"
    ;
}

/*************************************************************************************************/
