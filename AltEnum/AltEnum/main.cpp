//
//  main.cpp
//  AltEnum
//
//  Created by Dave Meehan on 23/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <iostream>
#include <unordered_set>

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

enum class Event {
    Readable,
    Writable,
    Error
};

namespace std {
    template <> struct hash<Event> {
        size_t operator()(const Event &event) const {
            return (size_t)event;
        }
    };
}

class Events : public std::unordered_set<Event> {

public:
    Events(const std::initializer_list<Event> &events) : std::unordered_set<Event>(events) {}
    
    bool is(const Event &event) {
        return (std::unordered_set<Event>::find(event) != std::unordered_set<Event>::end());
    }
};


inline std::ostream & operator << (std::ostream &stream, const Event &event) {

    switch (event) {
        case Event::Readable:
            stream << "Readable";
            break;
        case Event::Writable:
            stream << "Writable";
            break;
        case Event::Error:
            stream << "Error";
            break;
            
        default:
            break;
    }
    return stream;
}

inline std::ostream & operator << (std::ostream &stream, const Events &events) {

    int count = 0;
    for (auto &event : events) {
        stream << (count++ > 0 ? " " : "") << event;
    }
    return stream;
}

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
    
    Events events({ Event::Readable });
    
    std::cout << "Events: " << events << std::endl;
    
    events.emplace(Event::Writable);
    
    std::cout << "Events: " << events << std::endl;

    std::cout << "It is " << (events.is(Event::Readable) ? "" : "NOT ") << Event::Readable << std::endl;
    
    std::cout << "It is " << (events.is(Event::Error) ? "" : "NOT ") << Event::Error << std::endl;
    
    return 0;
    
    
}

