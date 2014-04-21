//
//  MessageSpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 21/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "Message.h"

namespace Messaging { namespace Specs {
  
    describe(Message, {
    
        context("default constructor", {
            
            Message msg;

            it("has no frames", {
                
                expect(msg.frames()).should.equal(0);
                
            });
            
            it("raises if sent", {
            
                Socket socket(Context(), Socket::socket_type::push);
                socket.bind("inproc://test");
                
                expect(theBlock({
                    
                    msg.send(socket, Message::block::none);
                    
                })).should.raise<Exception>("cannot send an empty message");
                
            });
            
        });
        
    });
    
} }