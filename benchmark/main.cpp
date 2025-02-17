
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
#include <vector>
#include <algorithm>
#include <numeric>

#ifdef NDEBUG
#   error "This file MUST be compiled with NDEBUG undefined!"
#endif

/*************************************************************************************************/

std::string dtf_format(const void *ptr) {
    return dtf::to_dt_str(*static_cast<const std::uint64_t *>(ptr), dtf::yyyy_mm_dd|dtf::date_sep_point|dtf::dt_sep_slash|dtf::time_sep_colon|dtf::secs);
}

std::string strftime_format(const void *ptr) {
    char buf[256];

    const auto *tm = static_cast<const struct tm *>(ptr);
    std::strftime(buf, sizeof(buf), "%Y.%m.%d/%H:%M:%S", tm);

    return {buf};
}

std::string put_time_format(const void *ptr) {
    const auto *tm = static_cast<const struct tm *>(ptr);

    std::ostringstream os;
    os << std::put_time(tm, "%Y.%m.%d/%H:%M:%S");

    return os.str();
}

/*************************************************************************************************/

struct result {
    std::string str;
    std::uint64_t time;
};

template<typename F>
result measure(F &&f) {
    auto beg = static_cast<std::uint64_t>(
        std::chrono::duration_cast<
            std::chrono::nanoseconds
        >(std::chrono::system_clock::now().time_since_epoch()).count()
    );

    result res;
    res.str = std::forward<F>(f)();

    auto end = static_cast<std::uint64_t>(
        std::chrono::duration_cast<
            std::chrono::nanoseconds
        >(std::chrono::system_clock::now().time_since_epoch()).count()
    );
    res.time = end - beg;

    return res;
}

/*************************************************************************************************/

std::uint64_t avg_time(const std::vector<result> &v) {
    return std::accumulate(
         v.begin()
        ,v.end()
        ,std::uint64_t(0)
        ,[](std::uint64_t init, const result &v){ return init + v.time; }
    );
}

/*************************************************************************************************/

int main() {
    auto ts = dtf::timestamp();
    auto tt = dtf::to_time_t(ts);
    const auto *tm = std::gmtime(&tt);

    auto r0 = dtf_format(&ts);
    auto r1 = strftime_format(tm);
    auto r2 = put_time_format(tm);

    std::cout
        << "dtf_format()     : " << r0 << std::endl
        << "strftime_format(): " << r1 << std::endl
        << "put_time_format(): " << r2 << std::endl
    ;
    assert(r0 == r1 && r1 == r2);

    auto iterations = 1000u*1000u*10u;
    std::vector<result> dtf_res, strftime_res, put_time_res;
    dtf_res.reserve(iterations);
    strftime_res.reserve(iterations);
    put_time_res.reserve(iterations);

    for (  auto idx = 0u; idx != iterations; ++idx ) {
        dtf_res.push_back(measure([ts](){ return dtf_format(&ts); }));
        strftime_res.push_back(measure([tm](){ return strftime_format(tm); }));
        put_time_res.push_back(measure([tm](){ return put_time_format(tm); }));
    }

    auto pred = [&r0](const result &v){ return r0 == v.str; };
    assert(static_cast<std::size_t>(std::count_if(dtf_res.begin(), dtf_res.end(), pred)) == iterations);
    assert(static_cast<std::size_t>(std::count_if(strftime_res.begin(), strftime_res.end(), pred)) == iterations);
    assert(static_cast<std::size_t>(std::count_if(put_time_res.begin(), put_time_res.end(), pred)) == iterations);

    auto dtf_time = avg_time(dtf_res);
    auto strftime_time = avg_time(strftime_res);
    auto put_time_time = avg_time(put_time_res);

    std::cout
        << "dtf time     : " << dtf_time << " ns" << std::endl
        << "strftime time: " << strftime_time << " ns" << std::endl
        << "put_time time: " << put_time_time << " ns" << std::endl
    ;
}

/*************************************************************************************************/
