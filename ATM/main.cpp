//
//  main.cpp
//  ATM
//
//  Created by Dave Meehan on 08/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include "ATM.h"

#include <thread>

#include <stdio.h>

int main(int argc, char *argv[]) {

    Messaging::Context ctx;
    
    auto atm = std::thread(ATM(ctx));
    
    Messaging::REQ q(ctx, "shmem://atm");
    
    q.send(ATM::card("0123456789"));
    
    q.send(ATM::pin("1234"));
    
    atm.join();
    
    return 0;
    
}