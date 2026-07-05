#include "request.h"
#include <iostream>
#include <cstring>
#include <string>
#include <cstdlib>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;


// local functions declarations
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

// member functions
restAPIHandler::restAPIHandler()
{
    // get local api key from env
    apiKey = getenv("API_KEY");

    // init curl module
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    // get tickers
    getTickers(MAX_TICKERS);

    //init candle stick data
    // memset(candleData, 0, sizeof(candleData)); 
}

restAPIHandler::~restAPIHandler()
{
    curl_global_cleanup();
}

string restAPIHandler::getRequest(const string url)
{
    std::string responseStr;

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);

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

    for(auto &t: tickers){
        cout << t << '\n';
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

