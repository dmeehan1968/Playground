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
    
    using ptr = std::shared_ptr<Person>;
    using const_ptr = std::shared_ptr<const Person>;
    using FirstName = std::shared_ptr<const std::string>;
    using LastName = std::shared_ptr<const std::string>;
    
    
    template <class S1, class S2>
    Person(S1 &&firstName, S2 &&lastName, const DateTime &dateOfBirth)
    :
        _firstName(std::make_shared<std::string>(std::forward<S1>(firstName))),
        _lastName(std::make_shared<std::string>(std::forward<S2>(lastName))),
        _dateOfBirth(dateOfBirth)
    {
        std::cout << "Person(" << this << ")" << std::endl;
    }
    
    ~Person() {
        std::cout << "~Person(" << this << ")" << std::endl;
    }
    
    template <typename... Args>
    static const_ptr const_create(Args&&...args) {
        return std::make_shared<const_ptr::element_type>(std::forward<Args>(args)...);
    }
    
    template <typename... Args>
    static ptr create(Args&&...args) {
        return std::make_shared<ptr::element_type>(std::forward<Args>(args)...);
    }
    
    FirstName firstName() const {
        return _firstName;
    }
    
    LastName lastName() const {
        return _lastName;
    }
    
    DateTime dateOfBirth() const {
        return _dateOfBirth;
    }
    
    template <class S>
    void setFirstName(S &&firstName) {
    
        _firstName = std::make_shared<const std::string>(std::forward<S>(firstName));
        
    }

    void setFirstName(const FirstName firstName) {
        _firstName = firstName;
    }
private:
    
    FirstName   _firstName;
    LastName    _lastName;
    DateTime    _dateOfBirth;
    
};

inline std::ostream &operator << (std::ostream &stream, const Person &person) {
    
    return stream
        << "Person (" << &person << "):" << std::endl
        << "\t" << "Firstname: " << *person.firstName() << std::endl
        << "\t" << "Lastname: " << *person.lastName() << std::endl
        << "\t" << "DOB: " << person.dateOfBirth() << std::endl;
    
}


#endif /* defined(__ImmutableObjects__Person__) */
