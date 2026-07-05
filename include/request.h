#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <curl/curl.h>
#include <unordered_set>

#define DATA_SIZE 1024
#define MAX_TICKERS 1000
#define BASE_ENDPOINT "https://api.massive.com/"

using namespace std;

typedef struct candle
{
    float open;
    float high;
    float low;
    float close;

}candle;

class restAPIHandler
{
    public:
        restAPIHandler();
        ~restAPIHandler();
        void getTickers(int lim);
        string getRequest(const string url);
        //string postRequest(const string url);
        
    private:
        CURL *curl;
        CURLcode res;
        string apiKey;
        candle candleData[DATA_SIZE];
        unordered_set<string> tickers;
};

#endif


