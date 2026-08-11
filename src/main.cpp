#include <stdio.h>
#include "request.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <thread>
#include "timeFunctions.h"

using namespace std;

// local functions
static string getApiKey(void);
static void waitUntil(int hour, int minute, int second);
static candle getLatestCandle(restAPIHandler &rHandler, 
                              const string &forexTicker, 
                              int multiplier, 
                              const string &timeSpan);


int main(int argc, char **argv){

    // args ./rest -t <HH:mm:ss> -i C:<FxPair> -m <Multipier> -s <TimeScale>
    // i.e. ./rest -t 09:30:00 -i C:EURUSD -m 5 -s minute
    
    
    bool inValidArgs = false;

    // validate arg count
    if(argc != 11){
        cout << "Invalid arguments.\n";
        cout << "Usage: ./rest -t <HH:mm:ss> -i C:<FxPair> -m <Multipier> -s <TimeScale> -a <apiKey>\n";
        return EXIT_FAILURE;
    }

    // validate arg flags
    if(strcmp(argv[1], "-t") != 0 ||
       strcmp(argv[3], "-i") != 0 ||
       strcmp(argv[5], "-m") != 0 ||
       strcmp(argv[7], "-s") != 0  ){

        inValidArgs = true;

    }

    // validate time format
    if(argv[2][2] != ':' || 
       argv[2][5] != ':' || 
       sizeof(argv[2]) != 8){

        inValidArgs = true;
    }

    if(inValidArgs){
        cout << "Invalid arguments.\n";
        cout << "Usage: ./rest -t <HH:mm:ss> -i C:<FxPair> -m <Multipier> -s <TimeScale>\n";
        return EXIT_FAILURE;
    }

    // rest api variables
    //string apiKey = getApiKey();
    string apiKey = argv[10];
    restAPIHandler rHandler(apiKey);

    rHandler.writeForexInstrToFile();
    rHandler.writeCrytoInstrToFile();
    
    
    // arg variables 
    string timeArg = argv[2];
    string fxPairArg = argv[4];
    string multiplierArg = argv[6];
    string timeScaleArg = argv[8];
    
    // candle variables
    candle fiveMinuteCandle, currentMinuteCandle;
    float lowerBound, upperBound;

    // validate forex instruments
    // if(rHandler.validForexInstrument(fxPairArg)){
    //     cout << "Instrument " << fxPairArg << "not found\n";
    //     cout << "Format: C:<FxPair>\n";
    //     return EXIT_FAILURE;
    // }

    // parse time arg
    int hour = stoi(timeArg.substr(0,2));
    int minute = stoi(timeArg.substr(3,2));
    int second = stoi(timeArg.substr(6));

    cout << "waiting ...\n";
    waitUntil(hour, minute, second);
    cout << " ... complete\n";
    

    fiveMinuteCandle = getLatestCandle(rHandler, fxPairArg, stoi(multiplierArg), timeScaleArg);
    cout << "low: " << fiveMinuteCandle.low << endl;
    cout << "high: " << fiveMinuteCandle.high << endl; 

    
    return 0;
}

static string getApiKey(void){

    char value[256];
    FILE *fileHandler = fopen("apiKey.txt", "r");
    string apiKey = "";

    if(!fileHandler){
        perror("fopen");
    }
    if (fgets(value, sizeof(value), fileHandler) != NULL)
    {
        apiKey = (string)(value);
    }
    fclose(fileHandler);

    return apiKey;
}

static candle getLatestCandle(restAPIHandler &rHandler, 
                              const string &forexTicker, 
                              int multiplier, 
                              const string &timeSpan){


    time_t currentTime = std::time(nullptr);
    tm localTime = *std::localtime(&currentTime);

    if(timeSpan == "minute"){
        if(localTime.tm_min % multiplier != 0){
            localTime.tm_min -= (localTime.tm_min % multiplier);
            currentTime = mktime(&localTime);
            localTime = *std::localtime(&currentTime);
        }
        localTime.tm_sec = 0;
    }
    else if(timeSpan == "hour"){
        if(localTime.tm_hour % multiplier != 0){
            localTime.tm_hour -= (localTime.tm_hour % multiplier);
            currentTime = mktime(&localTime);
            localTime = *std::localtime(&currentTime);
        }
        localTime.tm_sec = 0;
        localTime.tm_min = 0;
    }

    string timeStr = formatTime(&localTime);

    return rHandler.getSingleCandle(forexTicker, 5, "minute", timeStr, timeStr);

}

static void waitUntil(int hour, int minute, int second)
{
    using Clock = std::chrono::system_clock;

    if(hour >= 24 || minute >= 60 || second >= 60){
        cout << "Invalid time format.\n";
        return;
    }

    std::time_t nowTime = Clock::to_time_t(Clock::now());
    std::tm targetTime = *std::localtime(&nowTime);

    // Set the desired clock time
    targetTime.tm_hour = hour;
    targetTime.tm_min  = minute;
    targetTime.tm_sec  = second;

    std::time_t target = std::mktime(&targetTime);

    if(nowTime > target){
        cout << "Target time has passed, set new time.\n";
        return;
    }

    std::this_thread::sleep_until(Clock::from_time_t(target));
}





