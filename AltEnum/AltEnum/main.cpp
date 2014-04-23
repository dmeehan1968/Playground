//
//  main.cpp
//  AltEnum
//
//  Created by Dave Meehan on 23/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <iostream>

class Week {
    
public:
    
    class Day {
        
    public:
        
        Day operator++(int) {
            auto oldday = _day;
            ++_day;
            if (*this > Sun) {
                _day = Mon._day;
            }
            
            return Day(oldday);
        }
        
        static const Day Mon;
        static const Day Tue;
        static const Day Wed;
        static const Day Thu;
        static const Day Fri;
        static const Day Sat;
        static const Day Sun;
        
        operator std::string () const {
            switch (_day) {
                case 0:
                    return "Monday";
                    break;
                case 1:
                    return "Tuesday";
                    break;
                case 2:
                    return "Wednesday";
                    break;
                case 3:
                    return "Thursday";
                    break;
                case 4:
                    return "Friday";
                    break;
                case 5:
                    return "Saturday";
                    break;
                case 6:
                    return "Sunday";
                    break;
                    
            }
            
            return "Unknown";
            
        }
        
        bool operator==(const Day &other) {
            return _day == other._day;
        }
        
        bool operator>(const Day &other) {
            return _day > other._day;
        }
        
    protected:
        
        constexpr Day(const int &day) : _day(day) {}
        
    private:
        int _day;
        
    };
    
};

constexpr Week::Day Week::Day::Mon(0);
constexpr Week::Day Week::Day::Tue(1);
constexpr Week::Day Week::Day::Wed(2);
constexpr Week::Day Week::Day::Thu(3);
constexpr Week::Day Week::Day::Fri(4);
constexpr Week::Day Week::Day::Sat(5);
constexpr Week::Day Week::Day::Sun(6);

int main(int argc, const char * argv[])
{
    Week::Day day(Week::Day::Sun);
    
    std::cout << (std::string)day << std::endl;
    day++;
    std::cout << (std::string)day << std::endl;
    
    if (day == Week::Day::Mon) {
        std::cout << "Yes, its " << (std::string)day << std::endl;
    } else {
        std::cout << "EEK! It's not Monday" << std::endl;
    }
    
    return 0;
    
    
}

