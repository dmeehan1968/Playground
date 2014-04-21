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
        Socket::socket_type expectedType = Socket::socket_type::request;
        
        Socket socket(ctx, expectedType);
        
        it("creates socket", {
            
            expect(socket == nullptr).should.beFalse();
            
        });
        
        context("socket type", {
            
            it("returns expected type", {
                
                expect(socket.type() == expectedType).should.beTrue();
                
            });
            
            it("sets underlying type", {
               
                int type;
                size_t type_size = sizeof(type);
                
                zmq_getsockopt(socket, ZMQ_TYPE, &type, &type_size);
                
                expect(type).should.equal((int)expectedType);
                
            });
            
        });
        
        it("closes socket in destructor", {
            
            socket.~Socket();
            
            expect(socket == nullptr).should.beTrue();
            
        });
        
        context("bind", {
            
            it("throws on invalid protocol", {
                
                expect(theBlock({
                    
                    Socket socket(Context(), Socket::socket_type::request);
                    
                    socket.bind("xxx://xxx");
                    
                })).should.raise<Exception>("bind failed - Protocol not supported");
                
            });
            
            it("throws on malformed endpoint", {
                
                expect(theBlock({
                    
                    Socket socket(Context(), Socket::socket_type::request);
                    
                    socket.bind("inproc:/xxx");
                    
                })).should.raise<Exception>("bind failed - Invalid argument");
                
            });
            
        });
        
        context("connect", {
            
            it("throws on invalid protocol", {
                
                expect(theBlock({
                    
                    Socket socket(Context(), Socket::socket_type::request);
                    
                    socket.connect("xxx://xxx");
                    
                })).should.raise<Exception>("connect failed - Protocol not supported");
                
            });
            
            it("throws on malformed endpoint", {
                
                expect(theBlock({
                    
                    Socket socket(Context(), Socket::socket_type::request);
                    
                    socket.connect("inproc:/xxx");
                    
                })).should.raise<Exception>("connect failed - Invalid argument");
                
            });
            
        });
        
    });
    
} }
