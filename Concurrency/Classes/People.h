//
//  People.h
//  Concurrency
//
//  Created by Dave Meehan on 31/03/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Concurrency__People__
#define __Concurrency__People__

#include "Person.h"

#include <functional>
#include <iostream>
#include <future>

class People {
    
public:
    
    People(const std::initializer_list<Person> &people)
    :
        _people(people)
    {}

    void for_each(std::function<void(const Person &)> action) const {
        
        std::for_each(_people.begin(), _people.end(), action);
        
    }
    
private:
    
    std::vector<Person> _people;
    
};

class PersonDecorator {
    
public:
    
    PersonDecorator(std::ostream &stream)
    :
        _stream(stream),
        _count(0)
    {}
    
    void operator() (const Names &names) {
        
        if (_count++ > 0) {
            _stream << " ";
        }
        
        _stream << names;
        
    }
    
private:
    
    std::ostream &_stream;
    unsigned _count;
    
};

template <class PersonDecorator>
class PeopleDecorator {
    
public:

    PeopleDecorator(std::ostream &stream)
    :
        _stream(stream),
        _count(0)
    {}
    
    void operator() (const Person &person) {
        
        if (_count++ > 0) {
            _stream << "," << std::endl;
        }
        
        PersonDecorator _personDecorator(_stream);
        
        person.givenNames(_personDecorator);
        person.familyNames(_personDecorator);
        
    }
    
private:
    
    std::ostream &_stream;
    unsigned _count;
    
};

inline std::ostream &operator << (std::ostream &stream, const People &people) {
    
    people.for_each(PeopleDecorator<PersonDecorator>(stream));
    
    return stream << std::endl;
    
}

#endif /* defined(__Concurrency__People__) */
