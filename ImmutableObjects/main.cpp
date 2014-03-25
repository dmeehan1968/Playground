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

    Person other(dave.clone());
    
    std::cout << dave << std::endl;
    
    
}