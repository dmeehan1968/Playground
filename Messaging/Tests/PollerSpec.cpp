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
                        
                        poller.observe(socket, Poller::Events::None);
                        
                    })).should.raise<Exception>("no events specified");
                    
                });
                
                it("has one socket after add", {
                    
                    poller.observe(socket, Poller::Events::Readable);
                    
                    expect(poller.socketCount()).should.equal(1);
                    
                });
                
                context("before polling", {
                    
                    it("is not readable", {
                        
                        expect(poller(socket).isReadable()).should.beFalse();
                        
                    });
                    
                    it("is not writable", {
                        
                        expect(poller(socket).isWritable()).should.beFalse();
                        
                    });

                    it("is not in error", {
                        
                        expect(poller(socket).isError()).should.beFalse();
                        
                    });
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
            
            poller.observe(client, Poller::Events::Writable);
            poller.observe(server, Poller::Events::Readable);
            
            context("client writable", {
                
                
                it("should have one socket with event", {
                    
                    expect(poller.poll(0)).should.equal(1);
                    
                });
                
                it("should have writable event for client", {
                    
                    expect(poller(client).isWritable()).should.beTrue();
                    
                });
                
            });
            
            if (poller(client).isWritable()) {

                it("should raise when testing for client after removal", {
                    
                    poller.observe(client, Poller::Events::None);
                    
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
                        
                        expect(poller(server).isReadable()).should.beTrue();
                        
                    });
                    
                });

            }
            
        });
        
    });
    
} }