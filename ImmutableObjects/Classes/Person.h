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
#include <string>

class Person {
    
public:
    
    template <class String>
    Person(String &&firstName, String &&lastName, const DateTime &dateOfBirth) :
        _firstName(std::make_shared<String>(std::forward<String>(firstName))),
        _lastName(std::make_shared<String>(std::forward<String>(lastName))),
        _dateOfBirth(dateOfBirth)
    {}
    
    Person(const char * const firstName, const char * const lastName, const DateTime &dateOfBirth)
    :
        Person(std::string(firstName), std::string(lastName), dateOfBirth)
    {}
    
private:
    
    std::shared_ptr<std::string>    _firstName;
    std::shared_ptr<std::string>    _lastName;
    DateTime                        _dateOfBirth;
    
};


#endif /* defined(__ImmutableObjects__Person__) */
