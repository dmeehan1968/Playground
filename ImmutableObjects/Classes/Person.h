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
#include <ostream>
#include <iostream>

class Person {
    
public:
    
    Person(const std::string &firstName, const std::string &lastName, const DateTime &dateOfBirth)
    :
    _firstName(std::make_shared<std::string>(firstName)),
    _lastName(std::make_shared<std::string>(lastName)),
    _dateOfBirth(dateOfBirth)
    {
        std::cout << "Person(" << this << ")" << std::endl;
    }

    Person(const Person &other)
    :
        _firstName(other._firstName),
        _lastName(other._lastName),
        _dateOfBirth(other._dateOfBirth)
    {
        std::cout << "copy Person(" << this << ")" << std::endl;
    }
    
    ~Person() {
        std::cout << "~Person(" << this << ")" << std::endl;
    }
    
    std::shared_ptr<std::string> firstName() const {
        return _firstName;
    }
    
    std::shared_ptr<std::string> lastName() const {
        return _lastName;
    }
    
    DateTime dateOfBirth() const {
        return _dateOfBirth;
    }
    
    void setFirstName(const std::shared_ptr<std::string> &firstName) {
    
        _firstName = firstName;
        
    }
    
private:
    
    std::shared_ptr<std::string>    _firstName;
    std::shared_ptr<std::string>    _lastName;
    DateTime                        _dateOfBirth;
    
};

inline std::ostream &operator << (std::ostream &stream, const Person &person) {
    
    return stream
        << "Person (" << &person << "):" << std::endl
        << "\t" << "Firstname: " << *person.firstName() << std::endl
        << "\t" << "Lastname: " << *person.lastName() << std::endl
        << "\t" << "DOB: " << person.dateOfBirth() << std::endl;
    
}


#endif /* defined(__ImmutableObjects__Person__) */
