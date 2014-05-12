//
//  StreamServerSpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 11/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "StreamServer.h"

#include "Frame.h"

namespace Messaging { namespace Nodes { namespace StreamProtocol {
    
    using namespace Messaging;
    
    describe(StreamServer, {

        Context context;
        Socket serverSocket(context, Socket::Type::router);
        serverSocket.bind("inproc://test");
        
        StreamServer server(serverSocket);
        
        beforeEach({
            
            server.run();
            
        });
        
        context("before client connect", {
            
            it("fails to send", {
                
                expect(theBlock({
                    
                    server.send(Frame(), Frame("12345"));
                    
                })).should.raise();
                
            });

        });
        
        context("after client connect", {
            
            Socket clientSocket(context, Socket::Type::dealer);
            clientSocket.connect("inproc://test");
            
            Frame().send(clientSocket, Frame::block::blocking, Frame::more::none);
            
            it("sends", {
                
                server.send(Frame({ 0x00, (char)0xa3, 0x77, 0x6a, (char)0x9b }), Frame("12345"));
                
                Frame reply;
                reply.receive(clientSocket, Frame::block::none);
                
                expect(reply).should.equal(Frame("12345"));
                
            });

        });


        
        
    });
    
} } }