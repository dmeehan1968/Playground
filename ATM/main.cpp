//
//  main.cpp
//  ATM
//
//  Created by Dave Meehan on 08/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include "ATM.h"

#include <thread>
#include <vector>

#include <stdio.h>

int main(int argc, char *argv[]) {
    
    std::cout << "Starting" << std::endl;
    
    Messaging::Context context;
    
    std::vector<std::thread> threads;

    threads.emplace_back(ATM::Machine(context));
    
    Messaging::Socket atm(context, ZMQ_REQ);
    
    atm.bind("tcp://*:5555");
    
    atm.send(ATM::WithdrawlRequest());
    
    atm.receive<ATM::MessageFactory>()
    .handle<ATM::WithdrawlRequest>([&](const ATM::WithdrawlRequest &request) {
        
        auto response = request;
        
        response._account = "0123456789";
        
        atm.send(response);
        
        return true;
        
    });
    
    atm.receive<ATM::MessageFactory>()
    .handle<ATM::WithdrawlRequest>([&](const ATM::WithdrawlRequest &request) {
        
        auto response = request;
        
        response._pin = "1234";
        
        atm.send(response);
        
        return true;
        
    });
    
    atm.receive<ATM::MessageFactory>()
    .handle<ATM::WithdrawlRequest>([&](const ATM::WithdrawlRequest &request) {
        
        auto response = request;
        
        response._amount = 25;
        
        atm.send(response);
        
        return true;
        
    });
    
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
    
    return 0;
    
}