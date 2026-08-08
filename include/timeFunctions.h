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
#include <thread>
#include "request.h"

using namespace std;

// 2026-07-16 20:30:00
// xxxx-xx-xx xx:xx:xx
// yyyy-mm-dd hh:mm:ss

string formatTime(const tm* t);
int64_t utcToUnixMilliseconds(const string& utcDateTime);
void clockTickerMode(restAPIHandler *rHandle, const string &forexTicker, int multiplier, const string &timeSpan);

#endif