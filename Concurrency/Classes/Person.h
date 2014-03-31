//
//  Person.h
//  Concurrency
//
//  Created by Dave Meehan on 31/03/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Concurrency__Person__
#define __Concurrency__Person__

#include <string>
#include <vector>

class Person {

public:
    
    using Name = std::string;
    using Names = std::vector<Name>;
    
    Person(const std::initializer_list<Name> &givenNames, const std::initializer_list<Name> &familyNames)
    :
        _givenNames(givenNames),
        _familyNames(familyNames)
    {}
    
private:
    
    Names _givenNames;
    Names _familyNames;
    
};

#endif /* defined(__Concurrency__Person__) */
