//
//  Person.h
//  Concurrency
//
//  Created by Dave Meehan on 31/03/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Concurrency__Person__
#define __Concurrency__Person__

#include "Names.h"

#include <string>
#include <vector>
#include <ostream>

class Person {

public:
    
    Person(const std::initializer_list<Name> &givenNames, const std::initializer_list<Name> &familyNames)
    :
        _givenNames(givenNames),
        _familyNames(familyNames)
    {}
    
    Names givenNames() const {
        return _givenNames;
    }
    
    Names familyNames() const {
        return _familyNames;
    }
    
private:
    
    Names _givenNames;
    Names _familyNames;
    
};

inline std::ostream &operator << (std::ostream &stream, const Person &person) {

    const auto given = person.givenNames();
    const auto family = person.familyNames();
    
    bool delim = given.size() > 0 && family.size() > 0;
    
    return stream << given << (delim ? " " : "") << family;
    
}

#endif /* defined(__Concurrency__Person__) */
