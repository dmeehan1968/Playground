//
//  main.cpp
//  ImmutableObjects
//
//  Created by Dave Meehan on 24/03/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include "Person.h"

#include <iostream>

#include <stdio.h>

class MutablePerson : public Person {

public:
    
    MutablePerson(const Person &person) : Person(person) {}
    
};

int main(int argc, char *argv[]) {
    
    using _s = std::string;
    
    const Person God("God", nullptr, DateTime(0,0,0));
    
    std::cout << God << std::endl;
    
    Person daveMeehan(_s("dave"), _s("meehan"), DateTime(1968,8,15));

    std::cout << daveMeehan << std::endl;

    daveMeehan = daveMeehan.firstName(_s("Dave"));
    
    std::cout << daveMeehan << std::endl;

    daveMeehan = MutablePerson(daveMeehan)
                    .lastName(_s("Meehan"))
                    .dateOfBirth(DateTime(1968,8,14));
 
    std::cout << daveMeehan << std::endl;

}