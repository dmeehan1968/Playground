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
            
            proxyThread = std::thread(&Proxy::run, proxy, 10);
            
        });

        afterEach({
            
            proxy->stop();
            proxyThread.join();

            proxy = nullptr;
            requester = nullptr;
            replier = nullptr;
            router = nullptr;
            dealer = nullptr;
            
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            
        });

        context("request", {
            
            std::string expectedMsg("HELLO");
            Message requestMsg;
            Message receivedMsg;
            
            beforeEach({
            
                requestMsg.clear();

                requestMsg.emplace_back(expectedMsg);
                requestMsg.send(*requester);

                receivedMsg.clear();
                receivedMsg.receive(*replier);
                
            });

            it("receives request", {
                
                expect(receivedMsg.back().str()).should.equal(expectedMsg);
                
            });
            
            context("reply", {
                
                expectedMsg = "WORLD";
                Message replyMsg;
                Message receivedMsg;
                
                beforeEach({
                
                    replyMsg.clear();
                    
                    replyMsg.emplace_back(expectedMsg);
                    replyMsg.send(*replier);
                
                    receivedMsg.clear();
                    receivedMsg.receive(*requester);
                    
                });
                
                it("receives reply", {
                    
                    expect(receivedMsg.back().str()).should.equal(expectedMsg);
                    
                });

            });

        });

        context("message intercept", {
            
            std::string expectedMsg("hello");
            Message request;
            Message receivedMsg;
            
            beforeEach({
                
                proxy->onMessage = [&](const Socket &socket, Message &msg) {
                
                    Frame &frame = msg.back();
                    
                    for (int i=0 ; i < frame.size() ; i++) {
                        
                        frame.data<char>()[i] = std::tolower(frame.data<char>()[i]);
                        
                    }
                    
                };

                request.clear();
                request.emplace_back("HELLO");
                request.send(*requester);
            
                receivedMsg.clear();
                receivedMsg.receive(*replier);
                
            });
            
            it("converts to lowercase", {
                
                expect(receivedMsg.back().str()).should.equal(expectedMsg);
                
            });


        });
        
    });

} }
