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
    
    const auto pGod = Person::const_create("God", "", DateTime(0,0,0));
    
    std::cout << *pGod << std::endl;
    
    auto pDave = Person::const_create("Dave", "Meehan", DateTime(1968,8,14));
    
    std::cout << *pDave << std::endl;

    std::cout << "Mutable copy" << std::endl;
    
    auto pMutableDave = Person::create(*pDave);
    
    pMutableDave->setFirstName("David");
    
    std::cout << "Replace immutable" << std::endl;

    pDave = Person::const_create(std::move(*pMutableDave));
    
    std::cout << *pDave << std::endl;
    
    pMutableDave->setFirstName(pDave->lastName());
    
    std::cout << *pMutableDave << std::endl;
    
    std::cout << *pDave << std::endl;
    
}