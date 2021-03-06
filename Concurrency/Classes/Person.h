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
    
    Person(const std::initializer_list<Name> &givenNames, const std::initializer_list<Name> &familyNames, const std::string &email)
    :
        _givenNames(givenNames),
        _familyNames(familyNames),
        _email(email)
    {}
 
    template <class F>
    void givenNames(const F &functor) const {
        functor(_givenNames);
    }
    
    template <class F>
    void familyNames(const F &functor) const {
        functor(_familyNames);
    }
    
    template <class F>
    void email(const F &functor) const {
        functor(_email);
    }
    
private:
    
    Names _givenNames;
    Names _familyNames;
    std::string _email;
    
};

#endif /* defined(__Concurrency__Person__) */
