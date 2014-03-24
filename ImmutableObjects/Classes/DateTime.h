//
//  DateTime.h
//  ImmutableObjects
//
//  Created by Dave Meehan on 24/03/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ImmutableObjects__DateTime__
#define __ImmutableObjects__DateTime__

#include <ctime>

class DateTime {
    
public:
    
    DateTime(const unsigned year, const unsigned month, const unsigned day, const unsigned hour = 0, const unsigned minute = 0, const unsigned second = 0) {
        
        struct tm tm;
        
        tm.tm_year = year;
        tm.tm_mon = month;
        tm.tm_mday = day;
        tm.tm_hour = hour;
        tm.tm_min = minute;
        tm.tm_sec = second;
        
        _time = mktime(&tm);
        
    }
    
private:
    
    time_t _time;
    
};

#endif /* defined(__ImmutableObjects__DateTime__) */
