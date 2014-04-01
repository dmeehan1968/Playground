//
//  People.h
//  Concurrency
//
//  Created by Dave Meehan on 31/03/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Concurrency__People__
#define __Concurrency__People__

#include "Person.h"

#include <functional>
#include <iostream>
#include <future>

class People : public std::vector<Person> {
    
public:
    
    People(const std::initializer_list<Person> &people)
    :
        std::vector<Person>(people)
    {}

};

inline std::ostream &operator << (std::ostream &stream, const People &people) {
    
    bool delim = false;
    
    std::for_each(people.begin(), people.end(), [&stream, &delim](const Person &person) {
        if (delim) {
            stream << ", " << std::endl;
        }
        delim = true;
        stream << person;
    });
    
    if (people.size() > 0) {
        stream << std::endl;
    }
    
    return stream;
    
}

#endif /* defined(__Concurrency__People__) */
