//
//  PollerSpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 21/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "Poller.h"
#include "Message.h"

#include <thread>

namespace Messaging { namespace Specs {
  
    describe(Poller, {

        context("default constructor", {
            
            Poller poller;
            
            it("has no sockets by default", {
                
                expect(poller.socketCount()).should.equal(0);
                
            });

            context("add socket", {
            
                Socket socket(Context(), Socket::Type::request);

                it("throws without events", {
                    
                    expect(theBlock({
                        
                        poller.observe(socket, { });
                        
                    })).should.raise<Exception>("no events specified");
                    
                });
                
                it("has one socket after add", {
                    
                    poller.observe(socket, { Poller::Event::Readable });
                    
                    expect(poller.socketCount()).should.equal(1);
                    
                });
                
                context("before polling", {
                    
                    it("is not readable", {
                        
                        expect(poller(socket).is(Poller::Event::Readable)).should.beFalse();
                        
                    });
                    
                    it("is not writable", {
                        
                        expect(poller(socket).is(Poller::Event::Writable)).should.beFalse();
                        
                    });

                    it("is not in error", {
                        
                        expect(poller(socket).is(Poller::Event::Error)).should.beFalse();
                        
                    });
                });
                
                
            });
            
        });
        
        context("events", {
           
            Poller::Events events;
            
            context("no events in constructor", {
                
                beforeEach({
                    
                    events = Poller::Events();
                    
                });
                
                it("is empty", {
                    
                    expect(events).should.equal({});
                    
                });
                
                it("has one item when one added", {
                    
                    events.emplace(Poller::Event::Readable);
                    
                    expect(events.size()).should.equal(1);
                    
                });
                
                it("can take all known events", {
                    
                    events.emplace(Poller::Event::Readable);
                    events.emplace(Poller::Event::Writable);
                    events.emplace(Poller::Event::Error);
                    
                    expect(events.size()).should.equal(3);
                    
                });
                
                context("event tests", {
                    
                    beforeEach({
                        
                        events.emplace(Poller::Event::Readable);
                        
                    });
                    
                    it("is readable", {
                        
                        expect(events.is(Poller::Event::Readable)).should.beTrue();
                        
                    });
                    
                    it("is not writable", {
                        
                        expect(events.is(Poller::Event::Writable)).should.beFalse();
                        
                    });
                    
                    it("is not error", {
                        
                        expect(events.is(Poller::Event::Error)).should.beFalse();
                        
                    });
                    
                });
            });
            
            context("three events in constructor", {
               
                Poller::Events events( { Poller::Event::Readable, Poller::Event::Writable, Poller::Event::Error });
                
                it("has 3 events", {
                    
                    expect(events.size()).should.equal(3);
                    
                });
                
                it("is readable", {
                    
                    expect(events.is(Poller::Event::Readable)).should.beTrue();
                    
                });
                
                it("is writable", {
                    
                    expect(events.is(Poller::Event::Writable)).should.beTrue();
                    
                });
                
                it("is error", {
                    
                    expect(events.is(Poller::Event::Error)).should.beTrue();
                    
                });
                
            });
            
        });
        
        context("functional", {

            Poller poller;
            Context context;
            Socket server(context, Socket::Type::pull);
            server.bind("inproc://test");
            
            Socket client(context, Socket::Type::push);
            client.connect("inproc://test");
            
            poller.observe(client, { Poller::Event::Writable });
            poller.observe(server, { Poller::Event::Readable });
            
            context("client writable", {
                
                
                it("should have one socket with event", {
                    
                    expect(poller.poll(0)).should.equal(1);
                    
                });
                
                it("should have writable event for client", {
                    
                    expect(poller(client).is(Poller::Event::Writable)).should.beTrue();
                    
                });
                
            });
            
            if (poller(client).is(Poller::Event::Writable)) {

                it("should raise when testing for client after removal", {
                    
                    poller.observe(client, { } );
                    
                    expect(theBlock({
                        
                        poller(client);
                        
                    })).should.raise<Exception>("socket not found");
                    
                });
                
                it("should have one socket left", {
                    
                    expect(poller.socketCount()).should.equal(1);
                    
                });
                
                context("server readable", {
                    
                    Frame frame("HELLO");
                    
                    frame.send(client, Frame::block::none, Frame::more::none);
                    
                    
                    it("should have one socket with event", {
                        
                        expect(poller.poll(0)).should.equal(1);
                        
                    });
                    
                    it("should have readable event for server", {
                        
                        expect(poller(server).is(Poller::Event::Readable)).should.beTrue();
                        
                    });
                    
                });

            }
            
        });
        
    });
    
} }