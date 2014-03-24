//
//  Person.h
//  ImmutableObjects
//
//  Created by Dave Meehan on 24/03/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ImmutableObjects__Person__
#define __ImmutableObjects__Person__

#include "DateTime.h"

#include <functional>

class Person {
    
public:
    
    template <class String>
    Person(String &&firstName, String &&lastName, const DateTime &dateOfBirth) :
        _firstName(std::forward<String>(firstName)),
        _lastName(std::forward<String>(lastName)),
        _dateOfBirth(dateOfBirth)
    {}
    
private:
    
    std::shared_ptr<std::string>    _firstName;
    std::shared_ptr<std::string>    _lastName;
    DateTime                        _dateOfBirth;
    
};


#endif /* defined(__ImmutableObjects__Person__) */
