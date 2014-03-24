//
//  main.cpp
//  ImmutableObjects
//
//  Created by Dave Meehan on 24/03/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include "Person.h"

#include <iostream>
#include <memory>

#include <stdio.h>

int main(int argc, char *argv[]) {
    
    using _s = std::string;

    auto pGod = std::make_shared<const Person>(_s("God"), _s(""), DateTime(0,0,0));
    
    std::cout << *pGod << std::endl;
    
    std::shared_ptr<const Person> pDave = std::make_shared<const Person>(_s("Dave"), _s("Meehan"), DateTime(1968,8,14));
    
    std::cout << *pDave << std::endl;

    std::cout << "Mutable copy" << std::endl;
    
    std::shared_ptr<Person> pMutableDave = std::make_shared<Person>(*pDave);
    
    pMutableDave->setFirstName(std::make_shared<std::string>("David"));
    
    std::cout << "Replace immutable" << std::endl;

    pDave = std::make_shared<const Person>(*pMutableDave);
    
    std::cout << *pDave << std::endl;
    
    pMutableDave->setFirstName(std::make_shared<std::string>("Satan"));
    
    std::cout << *pDave << std::endl;
    
    
    
//    const Person God("God", nullptr, DateTime(0,0,0));
//    
//    std::cout << God << std::endl;
//    
//    Person daveMeehan(_s("dave"), _s("meehan"), DateTime(1968,8,15));
//
//    std::cout << daveMeehan << std::endl;
//
//    daveMeehan = daveMeehan.firstName(_s("Dave"));
//    
//    std::cout << daveMeehan << std::endl;
//
//    daveMeehan = MutablePerson(daveMeehan)
//                    .lastName(_s("Meehan"))
//                    .dateOfBirth(DateTime(1968,8,14));
// 
//    std::cout << daveMeehan << std::endl;

}