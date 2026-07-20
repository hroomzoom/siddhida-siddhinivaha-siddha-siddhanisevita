#include <stdio.h>
#include <curl/curl.h>
#include "request.h"
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;
using namespace std;


int main(void)
{
    restAPIHandler rHandle;
    rHandle.getCandles("C:EURUSD", 1, "minute", "2026-07-14 09:30:00", "2026-07-14 09:45:00", 100);
    rHandle.printCandles();
    return 0;
}
