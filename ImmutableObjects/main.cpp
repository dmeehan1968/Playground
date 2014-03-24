//
//  main.cpp
//  ImmutableObjects
//
//  Created by Dave Meehan on 24/03/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <stdio.h>

#include "Person.h"

int main(int argc, char *argv[]) {
    
    using _s = std::string;
    
    Person daveMeehan(_s("dave"), _s("meehan"), DateTime(1968,8,14));
 
    const Person God("God", nullptr, DateTime(0,0,0));
    
}