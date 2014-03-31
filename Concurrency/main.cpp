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

int main(int argc, char *argv[]) {

    Person dave({ "David", "John" }, { "Meehan" });
    
    People people({ dave });
    
    return 0;
    
}