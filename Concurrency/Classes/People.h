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

class People {
    
public:
    
    People(const std::initializer_list<Person> &people)
    :
        _people(people)
    {}
    
private:
    
    std::vector<Person> _people;
    
};

#endif /* defined(__Concurrency__People__) */
