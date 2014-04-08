//
//  main.cpp
//  ATM
//
//  Created by Dave Meehan on 08/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include "ATM.h"

#include <future>

#include <stdio.h>

int main(int argc, char *argv[]) {

    ATM atm;
    
    atm.run();
    
    atm << ATM::insert_card();
    
    atm << ATM::digit(1) << ATM::digit(2);
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    return 0;
    
}