//
//  ProxySpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 25/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "Proxy.h"

#include <thread>

namespace Messaging { namespace Specs {
   
    describe(Proxy, {
       
        Context context;
        Socket requester(context, Socket::Type::request);
        Socket replier(context, Socket::Type::reply);
        Socket router(context, Socket::Type::router);
        Socket dealer(context, Socket::Type::dealer);
        
        router.bind("inproc://router");
        dealer.bind("inproc://dealer");
        Proxy proxy(router, dealer);
        
        requester.setReceiveTimeout(100);
        requester.connect("inproc://router");

        replier.setReceiveTimeout(100);
        replier.connect("inproc://dealer");
        
        std::thread proxyThread = std::thread(&Proxy::run, proxy, 100);

        bool doReply = false;
        
        it("requests", {
            
            Frame request("HELLO");
            
            request.send(requester, Frame::block::none, Frame::more::none);
        
            Frame inboundRequest;
            
            inboundRequest.receive(replier, Frame::block::blocking);

            std::string msg(inboundRequest.data<char>(), inboundRequest.size());
            
            expect(msg).should.equal("HELLO");
            
            doReply = true;
            
        });

        if (doReply) {
            
            it("replies", {
                
                Frame reply("WORLD");
                
                reply.send(replier, Frame::block::none, Frame::more::none);
                
                Frame inboundReply;
                
                inboundReply.receive(requester, Frame::block::blocking);
                
                std::string msg(inboundReply.data<char>(), inboundReply.size());
                
                expect(msg).should.equal("WORLD");
                
            });
            
        }

        proxy.stop();
        proxyThread.join();

    });
    
} }
