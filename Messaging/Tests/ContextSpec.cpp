//
//  ContextSpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 18/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "Context.h"

namespace Messaging { namespace Specs {
  
    describe(Context, {
       
        Context context;
        
        it("returns a void pointer to a context", {
            
            expect(context.get() != nullptr).should.beTrue();
            
        });
        
        it("reuses context pointer on copy", {
            
            auto other = context;
            
            expect(other.get() == context.get()).should.beTrue();
            
        });
       
        context("io threads", {
            
            it("should equal ZMQ default", {
            
                auto expected = zmq_ctx_get(context.get(), ZMQ_IO_THREADS);
                expect(context.getIOThreads()).should.equal(expected);
                
            });
            
            context("can be changed", {
                
                auto expected = ZMQ_IO_THREADS_DFLT == 1 ? 2 : 1;
                
                context.setIOThreads(expected);
                
                it("gets the expected value", {
                    
                    expect(context.getIOThreads()).should.equal(expected);

                });
                
                it("sets the ZMQ context option", {
                    
                    expect(zmq_ctx_get(context.get(), ZMQ_IO_THREADS)).should.equal(expected);
                    
                });
                
            });
            
        });
        
        it("destroys context in destructor", {
           
            context.~Context();
            
            expect(context.get() == nullptr).should.beTrue();
            
        });
        
    });
    
} }
