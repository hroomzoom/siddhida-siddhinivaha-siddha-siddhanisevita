#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <curl/curl.h>
#include <unordered_set>
#include <vector>
#include <cstdint>

#define DATA_SIZE 1024
#define MAX_TICKERS 1000
#define BASE_ENDPOINT "https://api.massive.com/"

using namespace std;

typedef struct candle{
    int64_t time;
    float open;
    float high;
    float low;
    float close;
    float volume;

}candle;

class restAPIHandler
{
    public:
        restAPIHandler(string &apiKeyStr);
        ~restAPIHandler();
        void getTickers(void);
        void writeForexInstrToFile(void);
        void writeCrytoInstrToFile(void);
        bool validForexInstrument(const string &fxInstrument);
        void getCandles(const string forexTicker, 
                        int multipler, 
                        const string timespan, 
                        const string from, 
                        const string to,
                        int limit);
        candle getSingleCandle(const string forexTicker, 
                             int multipler, 
                             const string timespan, 
                             const string from, 
                             const string to);
        string getRequest(const string url);
        void setApiKey(const string apiKeyStr);
        void printCandles(void);
        //string postRequest(const string url);
        
    private:
        CURL *curl;
        CURLcode res;
        string apiKey;
        vector<candle> candleData;
        unordered_set<string> fxTickers;
        unordered_set<string> cryptoTickers;
};

void printCandle(candle &c);

#endif


