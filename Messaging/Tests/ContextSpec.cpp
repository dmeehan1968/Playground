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
                
                expect(context.getIOThreads()).should.equal(ZMQ_IO_THREADS_DFLT);
                
            });
            
        });
        
    });
    
} }
