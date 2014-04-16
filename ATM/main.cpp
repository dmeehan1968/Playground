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
    
    std::cout << "Starting" << std::endl;
    
    Messaging::Context context;
    
    std::vector<std::thread> threads;

    threads.emplace_back(ATM::Machine(context));
    
    Messaging::Socket atm(context, ZMQ_REQ);
    
    atm.bind("tcp://*:5555");
    
    atm.send(ATM::WithdrawlRequest());
    
    atm.receive<>();
    
    atm.send("ACCOUNT");
    
    atm.receive<>();
    
    atm.send("BYE");
    
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
    
    return 0;
    
}