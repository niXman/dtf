# dtf
Super fast date-time string formating functions used in [YAL](https://github.com/niXman/yal) logger, without supporting time-zones, but you can specify the required offset in hours.

# Example
```cpp
#include "dtf.hpp"

char buf[dtf::bufsize];

// current timestamp with nanoseconds resolution.
// as arg can pass offset in hours in form '+2'/'-2'
auto t = dtf::timestamp();

// avail flags:
// yyyy_mm_dd // 2018-12-11
// dd_mm_yyyy // 11-12-2018
// sep1       // 2018-12-11 13:58:56
// sep2       // 2018.12.11-13.58.59
// sep3       // 2018.12.11-13:58:59
// secs       // seconds resolution
// msecs      // milliseconds resolution
// usecs      // microseconds resolution
// nsecs      // nanoseconds resolution
auto f = dtf::flags::yyyy_mm_dd|dtf::flags::nsecs|dtf::flags::sep3;

// formating
auto n = dtf::timestamp_to_chars(buf, t, f);
buf[n] = 0;

std::cout << buf << std::endl; // 2014.12.11-13:58:59.006057557
```
