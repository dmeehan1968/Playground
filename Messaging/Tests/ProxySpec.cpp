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
        
        std::shared_ptr<Socket> requester;
        std::shared_ptr<Socket> replier;
        std::shared_ptr<Socket> router;
        std::shared_ptr<Socket> dealer;
        std::shared_ptr<Proxy> proxy;
        std::thread proxyThread;
        
        beforeEach({

            requester = std::make_shared<Socket>(context, Socket::Type::request);
            replier = std::make_shared<Socket>(context, Socket::Type::reply);
            router = std::make_shared<Socket>(context, Socket::Type::router);
            dealer = std::make_shared<Socket>(context, Socket::Type::dealer);
            
            router->bind("inproc://router");
            dealer->bind("inproc://dealer");
            proxy = std::make_shared<Proxy>(*router, *dealer);

            requester->setReceiveTimeout(100);
            requester->connect("inproc://router");
            
            replier->setReceiveTimeout(100);
            replier->connect("inproc://dealer");
            
            proxyThread = std::thread(&Proxy::run, proxy, 100);
            
        });

        afterEach({
            
            proxy->stop();
            proxyThread.join();

            proxy = nullptr;
            requester = nullptr;
            replier = nullptr;
            router = nullptr;
            dealer = nullptr;
            
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            
        });

        context("request", {
            
            std::string expectedMsg("HELLO");
            Frame request(expectedMsg);
            Frame receivedFrame;
            
            beforeEach({
                
                request.send(*requester, Frame::block::none, Frame::more::none);
            
                receivedFrame.receive(*replier, Frame::block::blocking);
                
            });

            it("receives request", {
                
                expect(receivedFrame.str()).should.equal(expectedMsg);
                
            });
            
            context("reply", {
                
                expectedMsg = "WORLD";
                Frame reply(expectedMsg);
                
                beforeEach({
                    
                    reply.send(*replier, Frame::block::none, Frame::more::none);
                    
                    receivedFrame.receive(*requester, Frame::block::blocking);
                    
                });
                
                it("receives reply", {
                    
                    expect(receivedFrame.str()).should.equal(expectedMsg);
                    
                });

            });

        });

        context("message intercept", {
            
            std::string expectedMsg("hello");
            Frame request("HELLO");
            Frame receivedFrame;
            
            beforeEach({
                
                proxy->onMessage = [&](const Socket &socket, Message &msg) {
                
                    Frame &frame = msg.back();
                    
                    for (int i=0 ; i < frame.size() ; i++) {
                        
                        frame.data<char>()[i] = std::tolower(frame.data<char>()[i]);
                        
                    }
                    
                };
                
                request.send(*requester, Frame::block::none, Frame::more::none);
                
                receivedFrame.receive(*replier, Frame::block::blocking);
                
            });
            
            it("converts to lowercase", {
                
                expect(receivedFrame.str()).should.equal(expectedMsg);
                
            });


        });
        
    });

} }
