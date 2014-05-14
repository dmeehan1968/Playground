//
//  ReactorSpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 25/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "Reactor.h"
#include "Frame.h"

#include <thread>

namespace Messaging { namespace Specs {
  
    describe(Reactor, {

        Context context;
        
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

            it("throws if observer is nullptr", {
                
                Socket socket(context, Socket::Type::pull);

                expect(theBlock({
                    
                    reactor.addObserver(socket, Reactor::Event::Readable, nullptr);
                    
                })).should.raise<Exception>("observer cannot be null");
                
            });

        });

        context("removeObserver", {
            
            it("throws if socket is not observing event", {
                
                Socket socket(context, Socket::Type::pull);

                expect(theBlock({
                    
                    reactor.removeObserver(socket, Reactor::Event::Readable);
                    
                })).should.raise<Exception>("event is not observed");
                
            });

        });

        context("add readable observer", {
            
            Socket socket(context, Socket::Type::pull);

            beforeEach({
                
                reactor.addObserver(socket, Reactor::Event::Readable, [](Socket &socket, const Reactor::Event &event) {
                    
                });
                
            });
            
            it("has one polled socket", {
                
                expect(poller->socketCount()).should.equal(1);
                
            });
            
            context("same socket", {
                
                context("add writable observer", {
                    
                    beforeEach({
                        
                        reactor.addObserver(socket, Reactor::Event::Writable, [](Socket &socket, const Reactor::Event &event) {
                            
                        });
                        
                    });
                    
                    it("still has one polled socket", {
                        
                        expect(poller->socketCount()).should.equal(1);
                        
                    });
                    
                });
                
            });

            context("another socket", {
                
                Socket other(context, Socket::Type::pull);
                
                beforeEach({
                    
                    reactor.addObserver(other, Reactor::Event::Readable, [](Socket &, const Reactor::Event &) {
                        
                    });
                    
                });
                
                it("has two polled sockets", {
                    
                    expect(poller->socketCount()).should.equal(2);
                    
                });
                
            });
            
        });
        
        context("two events, same socket", {
            
            Socket socket(context, Socket::Type::pull);
            
            beforeEach({
                
                reactor.addObserver(socket, Reactor::Event::Readable, [](Socket &, const Reactor::Event &) {});
                
                reactor.addObserver(socket, Reactor::Event::Writable, [](Socket &, const Reactor::Event &) {});
                
            });

            context("remove readable", {
                
                beforeEach({
                
                    reactor.removeObserver(socket, Reactor::Event::Readable);
                    
                });

                it("has one polled socket", {
                    
                    expect(poller->socketCount()).should.equal(1);
                    
                });

                context("remove writable", {
                    
                    beforeEach({
                        
                        reactor.removeObserver(socket, Reactor::Event::Writable);
                        
                    });
                    
                    it("has no polled events", {
                        
                        expect(poller->socketCount()).should.equal(0);
                        
                    });

                });

            });

        });
        
        context("functional", {
            
            std::shared_ptr<Socket> client;
            std::shared_ptr<Socket> server;
            
            beforeEach({
        
                client = std::make_shared<Socket>(context, Socket::Type::push);
                server = std::make_shared<Socket>(context, Socket::Type::pull);
                
                server->bind("inproc://test");
                client->connect("inproc://test");
                
            });
            
            afterEach({
                client = nullptr;
                server = nullptr;
                
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                
            });

            context("writable", {
                
                bool didNotify = false;
                
                beforeEach({
                    
                    reactor.addObserver(*client, Reactor::Event::Writable, [&](Socket &socket, const Reactor::Event &event) {
                
                        didNotify = true;
                        
                    });
                    
                });
                
                it("notifies writable", {
                    
                    reactor.runOnce(100);
                    
                    expect(didNotify).should.beTrue();
                    
                });

            });

            context("readable", {
                
                bool didNotify = false;
                
                beforeEach({
                    
                    reactor.addObserver(*server, Reactor::Event::Readable, [&](Socket &socket, const Reactor::Event &event) {
                    
                        didNotify = true;
                        
                    });
                    
                    Frame frame("HELLO");
   
                    frame.send(*client, Frame::block::blocking, Frame::more::none);
                    
                });
                
                it("notifies readable", {
                    
                    reactor.runOnce(100);
                    
                    expect(didNotify).should.beTrue();
                    
                });

            });
            
        });
        
    });
    
} }