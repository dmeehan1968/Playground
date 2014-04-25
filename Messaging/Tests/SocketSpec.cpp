//
//  SocketSpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 21/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "Socket.h"

namespace Messaging { namespace Specs {
    
    describe(Socket, {
        
        Context ctx;
        auto expectedType = Socket::Type::request;
        
        Socket socket(ctx, expectedType);
        
        it("creates socket", {
            
            expect(socket.get() == nullptr).should.beFalse();
            
        });
        
        context("socket type", {
            
            it("returns expected type", {
                
                expect(socket.type() == expectedType).should.beTrue();
                
            });
            
            it("sets underlying type", {
               
                int type;
                size_t type_size = sizeof(type);
                
                zmq_getsockopt(socket.get(), ZMQ_TYPE, &type, &type_size);
                
                expect(type).should.equal((int)expectedType);
                
            });
            
        });
        
        it("closes socket in destructor", {
            
            socket.~Socket();
            
            expect(socket.get() == nullptr).should.beTrue();
            
        });
        
        context("bind", {
            
            it("throws on invalid protocol", {
                
                expect(theBlock({
                    
                    Socket socket(Context(), Socket::Type::request);
                    
                    socket.bind("xxx://xxx");
                    
                })).should.raise<Exception>("bind failed - Protocol not supported");
                
            });
            
            it("throws on malformed endpoint", {
                
                expect(theBlock({
                    
                    Socket socket(Context(), Socket::Type::request);
                    
                    socket.bind("inproc:/xxx");
                    
                })).should.raise<Exception>("bind failed - Invalid argument");
                
            });
            
        });
        
        context("connect", {
            
            it("throws on invalid protocol", {
                
                expect(theBlock({
                    
                    Socket socket(Context(), Socket::Type::request);
                    
                    socket.connect("xxx://xxx");
                    
                })).should.raise<Exception>("connect failed - Protocol not supported");
                
            });
            
            it("throws on malformed endpoint", {
                
                expect(theBlock({
                    
                    Socket socket(Context(), Socket::Type::request);
                    
                    socket.connect("inproc:/xxx");
                    
                })).should.raise<Exception>("connect failed - Invalid argument");
                
            });
            
        });
        
        context("socket options", {

            std::shared_ptr<Socket> socket;
            
            beforeEach({
                socket = std::make_shared<Socket>(Context(), Socket::Type::request);
            });

            it("sets send high water mark", {
                
                int expected = 10000;
                socket->setSendHighWaterMark(expected);
                
                int actual;
                auto size = sizeof(actual);
                zmq_getsockopt(socket->get(), ZMQ_SNDHWM, &actual, &size);
                
                expect(actual).should.equal(expected);
            });
            
            it("sets receive high water mark", {
                
                int expected = 10000;
                socket->setReceiveHighWaterMark(expected);
                
                int actual;
                auto size = sizeof(actual);
                zmq_getsockopt(socket->get(), ZMQ_RCVHWM, &actual, &size);
                
                expect(actual).should.equal(expected);
            });
            
            it("sets receive timeout", {
                
                int expected = 1000;
                socket->setReceiveTimeout(expected);
                
                int actual;
                auto size = sizeof(actual);
                zmq_getsockopt(socket->get(), ZMQ_RCVTIMEO, &actual, &size);
                
                expect(actual).should.equal(expected);
            });
            
        });

    });
    
} }
