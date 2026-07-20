#include "request.h"
#include <iostream>
#include <cstring>
#include <string>
#include <cstdlib>
#include <cstdint>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <unordered_set>
#include "timeFunctions.h"


using namespace std;
using json = nlohmann::json;

static unordered_set<string> timeSpanSet = {
    "second",
    "minute",
    "hour",
    "day",
    "week",
    "month",
    "quarter",
    "year"
};

// local functions declarations
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

// member functions
restAPIHandler::restAPIHandler()
{
    //insert api key here
    apiKey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

    // init curl module
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // get tickers
    getTickers(MAX_TICKERS);

}

restAPIHandler::~restAPIHandler()
{
    curl_global_cleanup();
}

string restAPIHandler::getRequest(const string url)
{
    std::string responseStr;

    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            std::cerr << "FAIL: " << curl_easy_strerror(res) << std::endl;

        curl_easy_cleanup(curl);
    }

    return responseStr;
}

void restAPIHandler::getTickers(int lim){
    
    assert(lim > 0 && lim <= MAX_TICKERS);
    
    int i = 0;
    char urlBuffer[500];
    string response;
    json jsonResponse;
    json::iterator it;
    string market, active, order, limit, sort;

    // set up query parameters
    market = "fx";
    active = "true";
    order = "asc";
    limit = to_string(lim);
    sort = "ticker";

    // build query url
    i += sprintf(urlBuffer, "%sv3/reference/tickers?", BASE_ENDPOINT);
    i += sprintf((urlBuffer + i), "market=%s&", market.c_str());
    i += sprintf((urlBuffer + i), "active=%s&", active.c_str());
    i += sprintf((urlBuffer + i), "order=%s&", order.c_str());
    i += sprintf((urlBuffer + i), "limit=%s&", limit.c_str());
    i += sprintf((urlBuffer + i), "sort=%s&", sort.c_str());
    i += sprintf((urlBuffer + i), "apiKey=%s&", apiKey.c_str());

    // get and parse json response
    response = restAPIHandler::getRequest(urlBuffer);
    jsonResponse = json::parse(response.c_str());
   
    // fill ticker set container
    for (it = jsonResponse["results"].begin(); it != jsonResponse["results"].end(); ++it) {
        tickers.insert((*it)["ticker"]); 
    }
}

void restAPIHandler::getCandles(const string forexTicker, 
                                int multipler, 
                                const string timespan, 
                                const string from, 
                                const string to,
                                int limit){

    int i = 0;
    string response;
    string open, close, high, low;
    json candleJson;
    json::iterator it;

    // ensure in ticker list
    assert(tickers.find(forexTicker) != tickers.end());
    assert(timeSpanSet.find(timespan) != timeSpanSet.end());

    // convert from and to times to timestamps
    int64_t fromTimeStamp =  utcToUnixMilliseconds(from);
    int64_t toTimeStamp =  utcToUnixMilliseconds(to);

    string urlString = BASE_ENDPOINT + (string)"v2/aggs/ticker/" +
                       forexTicker + '/' +                           
                       (string)"range/" + to_string(multipler) + '/' + timespan + '/' +
                       to_string(fromTimeStamp) + '/' + to_string(toTimeStamp) + '/' +
                       (string)"?adjusted=true&sort=asc&limit=" + to_string(limit) + '&' +
                       (string)"apiKey=" + apiKey ;

    // get and parse json response
    response = restAPIHandler::getRequest(urlString);
    candleJson = json::parse(response.c_str());
    candle currentCandle;

    for(auto &candle: candleJson["results"] ){

        currentCandle.open = candle["o"];
        currentCandle.high = candle["h"];
        currentCandle.low = candle["l"];
        currentCandle.close = candle["c"];

        candleData.push_back(currentCandle);
    }
}

void restAPIHandler::printCandles(void){

    for(auto &candle: candleData){
        cout << "open: " << candle.open << endl;
        cout << "high: " << candle.high << endl;
        cout << "low: " << candle.low << endl;
        cout << "close: " << candle.close << endl;
        cout << endl;
        cout << endl;
    }

}

// local function definitions

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{

    size_t totalSize = size * nmemb;
    std::string* response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), totalSize);
    return totalSize;

}

