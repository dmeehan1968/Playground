//
//  PubSubMultipartSpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 07/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "Frame.h"

#include <thread>

namespace Messaging { namespace Examples {
    
    describe(PubSubMultipart, {

        Context ctx;
        std::unique_ptr<Socket> pub;
        std::unique_ptr<Socket> sub;
        
        beforeEach({
        
            pub = std::unique_ptr<Socket>(new Socket(ctx, Socket::Type::publisher));
            pub->bind("inproc://pub");
            
            sub = std::unique_ptr<Socket>(new Socket(ctx, Socket::Type::subscriber));
            sub->connect("inproc://pub");
            sub->addSubscribeFilter("BAL", 3);
            
        });

        afterEach({
            
            pub = nullptr;
            sub = nullptr;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
        });

        context("matching first part", {
            
            Frame msg;
            
            beforeEach({

                Frame("BALLS").send(*pub, Frame::block::blocking, Frame::more::more);
                Frame("A PLENTY").send(*pub, Frame::block::blocking, Frame::more::none);
            
                msg.receive(*sub, Frame::block::none);
                
            });
            
            it("receives", {
                
                expect(msg.str()).should.equal("BALLS");
                
            });
            
            it("has more", {
                
                expect(msg.hasMore()).should.beTrue();
                
            });

            context("second part", {
                
                beforeEach({
                    
                    msg.receive(*sub, Frame::block::none);
                    
                });

                it("receives", {
                
                    expect(msg.str()).should.equal("A PLENTY");
                    
                });

                it("has no more", {
                    
                    expect(msg.hasMore()).should.beFalse();
                    
                });
                
                
            });

        });

        context("non-matching first part", {
            
            beforeEach({
                
                Frame("BATS").send(*pub, Frame::block::blocking, Frame::more::more);
                Frame("AND BALLS").send(*pub, Frame::block::blocking, Frame::more::none);
                
            });

            it("fails", {
                
                Frame received;
                
                expect(theBlock({
                    
                    received.receive(*sub, Frame::block::none);
                    
                })).should.raise<Exception>("receive failed - Resource temporarily unavailable");
                
            });
        });

        
    });
    
} }
