//
//  ATMExample.cpp
//  Messaging
//
//  Created by Dave Meehan on 25/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "Customer.h"

#include "Context.h"

#include <thread>

namespace ATM {
    
    using namespace Messaging;
    
    describe(ATMExample, {

        Context ctx;
        
        ATMAgent atmAgent(ctx);
        
        std::thread atmAgentThread(std::ref(atmAgent));
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        std::vector<std::thread> _customers;
        
        size_t done = 0;
        auto completion = [&]{
            done++;
        };
        
        for (int customerCount=0 ; customerCount < 10 ; customerCount++) {
            
            _customers.emplace_back(Customer(), ctx, 25, completion);
            
        }
        
        std::for_each(_customers.begin(), _customers.end(), [](std::thread &thread) {
            if (thread.joinable()) {
                thread.join();
            }
        });

        it("completes all customer threads", {
            
            expect(done).should.equal(_customers.size());
            
        });

        atmAgent.stop();
        atmAgentThread.join();
        
    });
    
}
