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

int main(int argc, char *argv[]) {
    
    Person dave({ "David", "John" }, { "Meehan" }, { "Dave" });
    
    std::cout << dave << std::endl;
    
    auto other = std::move(dave);
    
    std::cout << dave << std::endl;
    
    std::cout << other << std::endl;
    
    
}