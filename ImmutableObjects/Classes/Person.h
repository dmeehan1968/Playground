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
        Person(std::string(firstName == nullptr ? "" : firstName),
               std::string(lastName == nullptr ? "" : lastName), dateOfBirth)
    {}
    
    template <class String>
    Person firstName(String &&firstName) const {
        
        Person person(*this);
        
        person._firstName = std::make_shared<String>(firstName);
        
        return person;
        
    }
    
    template <class String>
    Person lastName(String &&lastName) const {
        
        Person person(*this);
        
        person._lastName = std::make_shared<String>(lastName);
        
        return person;
        
    }
    
    Person dateOfBirth(const DateTime &dateOfBirth) {
        
        Person person(*this);
        
        person._dateOfBirth = dateOfBirth;
        
        return person;
        
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
