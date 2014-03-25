//
//  Person.h
//  ImmutableObjects
//
//  Created by Dave Meehan on 24/03/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ImmutableObjects__Person__
#define __ImmutableObjects__Person__

#include <string>
#include <vector>
#include <ostream>

class Person {

public:
    
    using Name = std::string;
    using Names = std::vector<const Name>;
    
    using GivenNames = Names;
    using FamilyNames = Names;
    using Aliases = Names;
    
    Person(const std::initializer_list<const Name> &givenNames,
           const std::initializer_list<const Name> &familyNames,
           const std::initializer_list<const Name> &aliases)
    :
        _givenNames(givenNames),
        _familyNames(familyNames),
        _aliases(aliases)
    {}
    
    const GivenNames &givenNames() const {
        return _givenNames;
    }
    
    const FamilyNames &familyNames() const {
        return _familyNames;
    }
    
    const Aliases &aliases() const {
        return _aliases;
    }
    
private:
    
    GivenNames _givenNames;
    FamilyNames _familyNames;
    Aliases _aliases;
    
};

inline std::ostream & operator << (std::ostream &stream, const Person &person) {
    
    return stream << "Person:";
    
}

#endif /* defined(__ImmutableObjects__Person__) */
