# dtf
Fast, header-only, date-time formating and validating functions for C++11

# Example
```cpp
#include <dtf/dtf.hpp>

char buf[dtf::bufsize];

// current timestamp with nanoseconds resolution.
// the required offset in hours can be passed as an argument in form `+2`/`-2`
auto t = dtf::timestamp();

// avail flags:
// dtf::yyyy_mm_dd        // yyyy-mm-dd
// dtf::dd_mm_yyyy        // dd-mm-yyyy
// dtf::date_sep_dash     // 2018-12-11/yyyy-mm-dd
// dtf::date_sep_point    // 2018.12.11/yyyy.mm.dd
// dtf::date_sep_empty    // 20181211/yyyymmdd
// dtf::dt_sep_T          // 20181211T13:58:59 (required when `yyyy_mm_dd | date_sep_empty`)
// dtf::dt_sep_t          // 11122018t13:58:59 (required when `dd_mm_yyyy | date_sep_empty`)
// dtf::dt_sep_space      // 2018-12-11 13:58:59
// dtf::dt_sep_underscore // 2018-12-11_13:58:59
// dtf::dt_sep_slash      // 2018-12-11/13:58:59
// dtf::time_sep_colon    // 13:58:59/hh:mm:ss
// dtf::time_sep_point    // 13.58.59/hh.mm.ss
// dtf::time_sep_empty    // 135859/hhmmss
// dtf::secs              // seconds resolution
// dtf::msecs             // milliseconds resolution
// dtf::usecs             // microseconds resolution
// dtf::nsecs             // nanoseconds resolution

constexpr auto flags = dtf::yyyy_mm_dd|dtf::date_sep_empty|dtf::dt_sep_T|dtf::time_sep_empty|dtf::nsecs;

// formating
auto str = dtf::dt_str(flags);

std::cout << str << std::endl;

// ...

// validating
auto f = dtf::get_flags(str);
assert(f != 0); // wrong string or not supported
assert(f == flags);
```
# Benchmark
```
dtf time     : 752177494 ns
strftime time: 1548738218 ns
put_time time: 4430988336 ns

```
