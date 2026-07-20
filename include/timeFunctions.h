#ifndef TIMEFUNCTIONS_H
#define TIMEFUNCTIONS_H

#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace std;

// 2026-07-16 20:30:00
// xxxx-xx-xx xx:xx:xx

inline int64_t utcToUnixMilliseconds(const string& utcDateTime)
{
    tm timeInfo{};

    istringstream stream(utcDateTime);
    stream >> std::get_time(&timeInfo, "%Y-%m-%d %H:%M:%S");

    if (stream.fail()) {
        throw std::runtime_error("Invalid datetime format");
    }

    // macOS/Linux: interprets the tm structure as UTC.
    time_t unixSeconds = timegm(&timeInfo);

    if (unixSeconds == static_cast<time_t>(-1)) {
        throw std::runtime_error("Unable to convert datetime");
    }

    return static_cast<int64_t>(unixSeconds) * 1000;
}

#endif