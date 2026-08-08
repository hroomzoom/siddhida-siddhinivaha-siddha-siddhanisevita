#include "timeFunctions.h"
#include <unordered_map>

using namespace std;


void clockTickerMode(restAPIHandler *rHandle, 
                     const string &forexTicker, 
                     int multiplier, 
                     const string &timeSpan){

    time_t endTime;
    time_t startTime;
    tm* endLocal;
    tm* startLocal;
    uint32_t totalMultiply = 1;

    if(timeSpan == "hour"){
        totalMultiply *= (60 * multiplier);
    }    
    else if(timeSpan == "minute"){
        totalMultiply *= multiplier;
    }
    else{
        cout << "Invalid time span\n";
        return;
    }

    cout << "syncing to minute boundary ...\n";

    do{
        endTime = std::time(nullptr);
        endLocal = std::localtime(&endTime);
    } while (endLocal->tm_sec != 0);

    cout << "... clock synced\n";

    while (true){
        
        // Current UTC time
        endTime = std::time(nullptr);
        // Five minutes earlier
        startTime = endTime - (time_t)( totalMultiply * 60);

        endLocal = std::localtime(&endTime);
        // cout << formatTime(endLocal) << endl;
        startLocal = std::localtime(&startTime);
        // cout << formatTime(startLocal) << endl;

        string fromTimeStr = formatTime(startLocal);
        string toTimeStr = formatTime(endLocal);

        rHandle->getSingleCandle(forexTicker, multiplier, timeSpan, fromTimeStr, toTimeStr); 
        
        if(timeSpan == "hour"){
            std::this_thread::sleep_for(std::chrono::hours(totalMultiply));  
        }    
        else if(timeSpan == "minute"){
            totalMultiply = 10;
            std::this_thread::sleep_for(std::chrono::seconds(totalMultiply));
        }
    }
 
}

int64_t utcToUnixMilliseconds(const string& utcDateTime){
    
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

string formatTime(const tm* t){
    ostringstream oss;
    oss << std::put_time(t, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

