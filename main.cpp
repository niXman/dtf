
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

        std::cout << buf << std::endl;
    }
    static const auto ts = 1546966223006057557ull; // 2019-01-08 16:50:23.006057557
    static const testvals vals[] = {
         {dtf::flags::yyyy_mm_dd|dtf::flags::sep1|dtf::flags::secs , 19, "2019-01-08 16:50:23"}
        ,{dtf::flags::yyyy_mm_dd|dtf::flags::sep1|dtf::flags::msecs, 23, "2019-01-08 16:50:23.006"}
        ,{dtf::flags::yyyy_mm_dd|dtf::flags::sep1|dtf::flags::usecs, 26, "2019-01-08 16:50:23.006057"}
        ,{dtf::flags::yyyy_mm_dd|dtf::flags::sep1|dtf::flags::nsecs, 29, "2019-01-08 16:50:23.006057557"}
        ,{dtf::flags::yyyy_mm_dd|dtf::flags::sep2|dtf::flags::secs , 19, "2019.01.08-16.50.23"}
        ,{dtf::flags::yyyy_mm_dd|dtf::flags::sep2|dtf::flags::msecs, 23, "2019.01.08-16.50.23.006"}
        ,{dtf::flags::yyyy_mm_dd|dtf::flags::sep2|dtf::flags::usecs, 26, "2019.01.08-16.50.23.006057"}
        ,{dtf::flags::yyyy_mm_dd|dtf::flags::sep2|dtf::flags::nsecs, 29, "2019.01.08-16.50.23.006057557"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep1|dtf::flags::secs , 19, "08-01-2019 16:50:23"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep1|dtf::flags::msecs, 23, "08-01-2019 16:50:23.006"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep1|dtf::flags::usecs, 26, "08-01-2019 16:50:23.006057"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep1|dtf::flags::nsecs, 29, "08-01-2019 16:50:23.006057557"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep2|dtf::flags::secs , 19, "08.01.2019-16.50.23"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep2|dtf::flags::msecs, 23, "08.01.2019-16.50.23.006"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep2|dtf::flags::usecs, 26, "08.01.2019-16.50.23.006057"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep2|dtf::flags::nsecs, 29, "08.01.2019-16.50.23.006057557"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep3|dtf::flags::secs , 19, "08-01-2019 16:50:23"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep3|dtf::flags::msecs, 23, "08-01-2019 16:50:23.006"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep3|dtf::flags::usecs, 26, "08-01-2019 16:50:23.006057"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep3|dtf::flags::nsecs, 29, "08-01-2019 16:50:23.006057557"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep3|dtf::flags::secs , 19, "08.01.2019-16:50:23"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep3|dtf::flags::msecs, 23, "08.01.2019-16:50:23.006"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep3|dtf::flags::usecs, 26, "08.01.2019-16:50:23.006057"}
        ,{dtf::flags::dd_mm_yyyy|dtf::flags::sep3|dtf::flags::nsecs, 29, "08.01.2019-16:50:23.006057557"}
    };

    for ( const auto &it: vals ) {
        char buf[dtf::bufsize];
        auto r = test(buf, ts, it.flags);
        assert(r.exlen == it.exlen);
        assert(std::strcmp(r.exstr, buf) == 0);

        std::cout << r.exstr << std::endl;
    }

    return 0;
}

/*************************************************************************************************/
