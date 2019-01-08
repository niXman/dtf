
#ifndef __dtf__dtf_hpp
#define __dtf__dtf_hpp

#include <cstdint>

namespace dtf {

/*************************************************************************************************/

// always in nanoseconds resolution
std::uint64_t timestamp(int offset = 0 /*in hours*/);

struct flags {
    enum: std::size_t {
         yyyy_mm_dd = 1u << 0u
        ,dd_mm_yyyy = 1u << 1u
        ,sep1       = 1u << 2u // 2018-12-11 13:58:56
        ,sep2       = 1u << 3u // 2018.12.11-13.58.59
        ,sep3       = 1u << 4u // 2018.12.11-13:58:59
        ,secs       = 1u << 5u // seconds resolution
        ,msecs      = 1u << 6u // milliseconds resolution
        ,usecs      = 1u << 7u // microseconds resolution
        ,nsecs      = 1u << 8u // nanoseconds resolution
    };
};

enum { bufsize = 32 };

// returns the num of bytes placed
std::size_t timestamp_to_chars(
     char *ptr // dst buf with at least 'bufsize' bytes
    ,std::uint64_t ts
    ,std::size_t f = flags::yyyy_mm_dd|flags::sep1|flags::msecs
);

/*************************************************************************************************/

} // ns dtf

#ifdef DTF_HEADER_ONLY
#   include "dtf.cpp"
#endif // DTF_HEADER_ONLY

#endif // __dtf__dtf_hpp
