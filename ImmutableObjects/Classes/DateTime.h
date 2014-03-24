//
//  DateTime.h
//  ImmutableObjects
//
//  Created by Dave Meehan on 24/03/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ImmutableObjects__DateTime__
#define __ImmutableObjects__DateTime__

#include <ostream>
#include <ctime>

class DateTime {
    
public:
    
    DateTime(const unsigned year, const unsigned month, const unsigned day, const unsigned hour = 0, const unsigned minute = 0, const unsigned second = 0) {
        
        struct tm tm {};
        
        tm.tm_year = year - 1900;
        tm.tm_mon = month - 1;
        tm.tm_mday = day;
        tm.tm_hour = hour;
        tm.tm_min = minute;
        tm.tm_sec = second;
        
        _time = mktime(&tm);
        
    }
    
    std::string toString() const {
    
        char buf[128];
        
        strftime(buf, sizeof(buf), "%c", localtime(&_time));
        
        return std::string(buf);
        
    }
    
private:
    
    time_t _time;
    
};

inline std::ostream &operator << (std::ostream &stream, const DateTime &dateTime) {

    stream << dateTime.toString();
    
    return stream;
}

#endif /* defined(__ImmutableObjects__DateTime__) */
