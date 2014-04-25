//
//  ReactorSpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 25/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "Reactor.h"

namespace Messaging { namespace Specs {
  
    describe(Reactor, {

        Context context;
        Socket socket(context, Socket::Type::pull);
        
        std::shared_ptr<Poller> poller;
        
        Reactor reactor;
        
        beforeEach({
            
            poller = std::make_shared<Poller>();
            reactor = Reactor(poller);
            
        });
        
        it("exists", {
        
            expect(poller.get()).shouldNot.beNil();
            
        });
        
        it("has no polled sockets", {
            
            expect(poller->socketCount()).should.equal(0);
            
        });
        
        context("addObserver", {
            
            beforeEach({
                
                reactor.addObserver(socket, Reactor::Event::Readable, [](Socket &socket, const Reactor::Event &event) {
                    
                });
                
            });
            
            it("throws if observer is nullptr", {
                
                expect(theBlock({
                    
                    reactor.addObserver(socket, Reactor::Event::Readable, nullptr);
                    
                })).should.raise<Exception>("observer cannot be null");
                
            });
            
            it("has one polled socket after addObserver for readable", {
                
                expect(poller->socketCount()).should.equal(1);
                
            });
            
            context("add same socket, different event", {
                
                beforeEach({
                    
                    reactor.addObserver(socket, Reactor::Event::Writable, [](Socket &socket, const Reactor::Event &event) {
                        
                    });
                    
                });
                
                it("has one polled socket", {
                    
                    expect(poller->socketCount()).should.equal(1);
                    
                });

                context("remove writable observer", {
                    
                    beforeEach({
                        
                        reactor.removeObserver(socket, Reactor::Event::Writable);
                        
                    });
                    
                    it("has one polled socket", {
                        
                        expect(poller->socketCount()).should.equal(1);
                        
                    });
                    
                    context("remove readable observer", {
                        
                        beforeEach({
                            
                            reactor.removeObserver(socket, Reactor::Event::Readable);
                            
                        });

                        it("has no polled sockets", {
                            
                            expect(poller->socketCount()).should.equal(0);
                            
                        });

                    });

                });
                
            });
            
        });
        
        
    });
    
} }