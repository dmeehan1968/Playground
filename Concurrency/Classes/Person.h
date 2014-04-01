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
    
    const Names &givenNames() const {
        return _givenNames;
    }
    
    const Names &familyNames() const {
        return _familyNames;
    }
    
private:
    
    Names _givenNames;
    Names _familyNames;
    
};

inline std::ostream &operator << (std::ostream &stream, const Person &person) {

    std::string fullName;
    
    std::for_each(person.givenNames().begin(), person.givenNames().end(), [&fullName](const Name &name) {
        
        if (name.size() < 1) {
            return;
        }
        
        if (fullName.size() > 0) {
            fullName += " ";
        }
        
        fullName += name;
        
    });

    std::for_each(person.familyNames().begin(), person.familyNames().end(), [&fullName](const Name &name) {
        
        if (name.size() < 1) {
            return;
        }
        
        if (fullName.size() > 0) {
            fullName += " ";
        }
        
        fullName += name;
        
    });
    
    return stream << fullName;
    
}

#endif /* defined(__Concurrency__Person__) */
