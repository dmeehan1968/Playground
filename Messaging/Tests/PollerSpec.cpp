//
//  PollerSpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 21/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "Poller.h"
#include "Frame.h"

#include <thread>

namespace Messaging { namespace Specs {
  
    describe(Poller, {

        context("default constructor", {
            
            Poller poller;
            
            it("has no sockets by default", {
                
                expect(poller.socketCount()).should.equal(0);
                
            });

            context("add socket", {
            
                Socket socket(Context(), Socket::socket_type::request);

                it("throws without events", {
                    
                    expect(theBlock({
                        
                        poller.observe(socket, { });
                        
                    })).should.raise<Exception>("no events specified");
                    
                });
                
                it("has one socket after add", {
                    
                    poller.observe(socket, { Poller::event::readable });
                    
                    expect(poller.socketCount()).should.equal(1);
                    
                });
                
                context("before polling", {
                    
                    it("is not readable", {
                        
                        expect(poller.events(socket).readable).should.beFalse();
                        
                    });
                    
                    it("is not writable", {
                        
                        expect(poller.events(socket).writable).should.beFalse();
                        
                    });

                    it("is not in error", {
                        
                        expect(poller.events(socket).error).should.beFalse();
                        
                    });
                });
                
                
            });
            
        });
        
        context("functional", {

            Poller poller;
            Context context;
            Socket server(context, Socket::socket_type::pull);
            server.bind("inproc://test");
            
            Socket client(context, Socket::socket_type::push);
            client.connect("inproc://test");
            
            poller.observe(client, { Poller::event::writable });
            poller.observe(server, { Poller::event::readable });
            
            context("client writable", {
                
                
                it("should have one socket with event", {
                    
                    expect(poller.poll(0)).should.equal(1);
                    
                });
                
                it("should have writable event for client", {
                    
                    expect(poller.events(client).writable).should.beTrue();
                    
                });
                
            });
            
            if (poller.events(client).writable) {

                it("should raise when testing for client after removal", {
                    
                    poller.observe(client, { Poller::event::none });
                    
                    expect(theBlock({
                        
                        poller.events(client);
                        
                    })).should.raise<Exception>("socket not found");
                    
                });
                
                context("server readable", {
                    
                    Frame frame("HELLO");
                    
                    frame.send(client, Frame::block::none, Frame::more::none);
                    
                    
                    it("should have one socket with event", {
                        
                        expect(poller.poll(0)).should.equal(1);
                        
                    });
                    
                    it("should have readable event for server", {
                        
                        expect(poller.events(server).readable).should.beTrue();
                        
                    });
                    
                });

            }
            
        });
        
    });
    
} }