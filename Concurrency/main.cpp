//
//  main.cpp
//  Concurrency
//
//  Created by Dave Meehan on 31/03/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include "Person.h"
#include "People.h"

#include <stdio.h>
#include <iostream>

int main(int argc, char *argv[]) {

    People people({
        Person({ "David", "John" }, { "Meehan" }, "dave_meehan@replicated.co.uk"),
        Person({ "Elvis", "Aaron" }, { "Presley" }, "elvis@graceland.com"),
        Person({ "Madonna"}, {}, "madonna@virgin.net") }
    );
    
    std::cout << people;
    
    return 0;
    
}