#include "request.h"
#include <iostream>
#include <fstream>
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
restAPIHandler::restAPIHandler(string &apiKeyStr)
: apiKey(apiKeyStr)
{
    //curl_global_init(CURL_GLOBAL_DEFAULT);
    getTickers();
}

restAPIHandler::~restAPIHandler()
{
    curl_global_cleanup();
}

void restAPIHandler::setApiKey(const string apiKeyStr){

    apiKey = apiKeyStr;
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

void restAPIHandler::getTickers(void){
    
    
    int i = 0;
    char urlBuffer[500];
    string response;
    json jsonResponse;
    json::iterator it;

    // Get Fx fxTickers
    // get and parse json response
    string requestURL = "https://api.massive.com/v3/reference/tickers?market=fx&active=true&order=asc&limit=1000&sort=ticker&apiKey=" + apiKey;
    response = restAPIHandler::getRequest(requestURL);
    jsonResponse = json::parse(response.c_str());
   
    // fill ticker set container
    for (it = jsonResponse["results"].begin(); it != jsonResponse["results"].end(); ++it) {
        fxTickers.insert((*it)["ticker"]); 
    }

    // Get Cryto fxTickers
    // get and parse json response
    response = restAPIHandler::getRequest("https://api.massive.com/v3/reference/tickers?market=crypto&active=true&order=asc&limit=1000&sort=ticker&apiKey=" + apiKey);
    jsonResponse = json::parse(response.c_str());
   
    // fill ticker set container
    for (it = jsonResponse["results"].begin(); it != jsonResponse["results"].end(); ++it) {
        cryptoTickers.insert((*it)["ticker"]); 
    }
}

void restAPIHandler::writeCrytoInstrToFile(void){

    std::ofstream writeFile("cryptos.txt");

    if (!writeFile.is_open()) {
        cerr << "Error: Could not open the file!" << std::endl;
        return;
    }

    for(auto &instr: cryptoTickers){
        writeFile << instr << endl;
    }
}

void restAPIHandler::writeForexInstrToFile(void){

    std::ofstream writeFile("instruments.txt");

    if (!writeFile.is_open()) {
        cerr << "Error: Could not open the file!" << std::endl;
        return;
    }

    for(auto &instr: fxTickers){
        writeFile << instr << endl;
    }
}

bool restAPIHandler::validForexInstrument(const string &fxInstrument){
    return fxTickers.find(fxInstrument) != fxTickers.end();
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

        currentCandle.time = candle["t"];
        currentCandle.open = candle["o"];
        currentCandle.high = candle["h"];
        currentCandle.low = candle["l"];
        currentCandle.close = candle["c"];
        currentCandle.volume = candle["v"];

        candleData.push_back(currentCandle);
    }
}

candle restAPIHandler::getSingleCandle(const string forexTicker, 
                                int multipler, 
                                const string timespan, 
                                const string from, 
                                const string to){

    int i = 0;
    string response;
    string open, close, high, low;
    json candleJson;
    json::iterator it;

    // convert from and to times to timestamps
    int64_t fromTimeStamp =  utcToUnixMilliseconds(from);
    int64_t toTimeStamp =  utcToUnixMilliseconds(to);

    string urlString = BASE_ENDPOINT + (string)"v2/aggs/ticker/" +
                       forexTicker + '/' +                           
                       (string)"range/" + to_string(multipler) + '/' + timespan + '/' +
                       to_string(fromTimeStamp) + '/' + to_string(toTimeStamp) + '/' +
                       (string)"?adjusted=true&sort=asc&" +
                       (string)"apiKey=" + apiKey ;

    // get and parse json response
    response = restAPIHandler::getRequest(urlString);
    candleJson = json::parse(response.c_str());
    candle currentCandle;

    for(auto &candle: candleJson["results"] ){

        currentCandle.time = candle["t"];
        currentCandle.open = candle["o"];
        currentCandle.high = candle["h"];
        currentCandle.low = candle["l"];
        currentCandle.close = candle["c"];
        currentCandle.volume = candle["v"];
    }

    return currentCandle;
}

void printCandle(candle &c){
    
    cout << "time: " << c.time << endl;
    cout << "open: " << c.open << endl;
    cout << "high: " << c.high << endl;
    cout << "low: " << c.low << endl;
    cout << "close: " << c.close << endl;
    cout << "volume: " << c.volume << endl;

}

void restAPIHandler::printCandles(void){

    for(auto &candle: candleData){
        cout << "time: " << candle.time << endl;
        cout << "open: " << candle.open << endl;
        cout << "high: " << candle.high << endl;
        cout << "low: " << candle.low << endl;
        cout << "close: " << candle.close << endl;
        cout << "volume: " << candle.volume << endl;
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

