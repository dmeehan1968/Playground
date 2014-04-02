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

class PeopleNewlineDecorator {
    
public:
    
    PeopleNewlineDecorator(std::ostream &stream) : _stream(stream) {}
    
    void before() const {}
    
    void between() const {
        _stream << ", " << std::endl;
    }
    
    void after() const {
        _stream << std::endl;
    }
    
    template <class Iterator>
    void decorate(const Iterator &begin, const Iterator &end) const {
        
        int count = 0;
        
        if (end > begin) {
            before();
        }
        
        std::for_each(begin, end, [&](const Person &person) {
            
            if (count++ > 0) {
                between();
            }
            
            decorate(person);
            
        });
        
        if (count) {
            after();
        }

    }
    
    void decorate(const Person &person) const {
        _stream << person;
    }
    
private:
    
    std::ostream &_stream;
    
};

class People {
    
public:
    
    People(const std::initializer_list<Person> &people)
    :
        _people(people)
    {}

    void decorate(const PeopleNewlineDecorator &decorator) const {
        
        decorator.decorate(_people.begin(), _people.end());

    }
    
private:
    
    std::vector<Person> _people;
    
};

inline std::ostream &operator << (std::ostream &stream, const People &people) {
    
    people.decorate(PeopleNewlineDecorator(stream));
    
    return stream;
    
}

#endif /* defined(__Concurrency__People__) */
