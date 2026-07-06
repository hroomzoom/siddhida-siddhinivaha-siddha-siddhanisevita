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
    rHandle.getCandles("C:EURUSD", 1, "day", "2025-11-03", "2025-11-28", 100);
    rHandle.printCandles();
    return 0;
}
