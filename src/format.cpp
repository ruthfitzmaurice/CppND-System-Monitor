#include <string>
#include <cmath>
#include <iostream>
#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS


/* 
Example of how to convert 26,453 seconds to format HH::MM:SS:

1. Calculate whole number of hours: # seconds / 3600 = 7.348 hours ---> 7 hours with 0.348 hours remaining
2. Calculate whole number of minutes: # hours remaining * 60 = 20.88 minutes ----> 20 minutes with 0.88 minutes remaining
3. Calculate whole number of seconds: # minutes remaining * 60 = 52 seconds
4. Answer: 07: 20: 52
Note: the c++ floor command will be used to grab the whole number of hours/minutes/seconds

*/

string Format::ElapsedTime(long seconds) { 

    float hours = ((float)seconds)/3600;
    int whole_hours = floor(hours);
    float minutes = (hours - whole_hours)*60; 
    int whole_minutes = floor(minutes);
    float s = (minutes - whole_minutes) * 60;
    int whole_seconds = floor(s);
    std::string padded_hours = std::to_string(whole_hours);
    std::string padded_minutes = std::to_string(whole_minutes);
    std::string padded_seconds = std::to_string(whole_seconds);

    if (whole_hours < 10){
        padded_hours = "0"+std::to_string(whole_hours);
    }

    if (whole_minutes < 10){
        padded_minutes = "0"+std::to_string(whole_minutes);
    }
    if (whole_seconds < 10){
        padded_seconds = "0"+std::to_string(whole_seconds);
    }

    string format_string = padded_hours +":"+padded_minutes+":"+padded_seconds;

    return format_string; 
    }