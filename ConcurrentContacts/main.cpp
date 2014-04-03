//
//  main.cpp
//  ConcurrentContacts
//
//  Created by Dave Meehan on 03/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include "Contact.h"
#include "ContactStreamFormatter.h"
#include <iostream>
#include <stdio.h>

int main(int argc, char *argv[]) {
    
    Contact first({"David", "John"}, {"Meehan"}, {"dave.meehan@test.com"});
    
    std::cout << ContactStreamFormatter(first) << std::endl;
    
    return 0;
    
}