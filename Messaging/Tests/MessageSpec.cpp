//
//  MessageSpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 21/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "Message.h"

#include <thread>

namespace Messaging { namespace Specs {
  
    describe(Message, {
    
        context("default constructor", {
            
            Message msg;

            it("has no frames", {
                
                expect(msg.size()).should.equal(0);
                
            });
            
            it("raises if sent", {
            
                Socket socket(Context(), Socket::Type::push);
                socket.bind("inproc://test");
                
                expect(theBlock({
                    
                    msg.send(socket, Message::block::none);
                    
                })).should.raise<Exception>("cannot send an empty message");
                
            });
            
        });
        
        context("functional", {
        
            Context ctx;

            std::shared_ptr<Socket> client;
            std::shared_ptr<Socket> server;
            
            beforeEach({
                
                client = std::make_shared<Socket>(ctx, Socket::Type::request);
                server = std::make_shared<Socket>(ctx, Socket::Type::reply);
                
                server->bind("inproc://test");
                client->connect("inproc://test");

            });
            
            afterEach({
            
                client = nullptr;
                server = nullptr;

                // allow for shutdown of sockets before reuse
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                
            });
            
            context("receiving", {
                
                Message msg;
                size_t size;
                
                beforeEach({
                
                    msg.clear();
                    
                    Frame f1("HELLO");
                    Frame f2("WORLD");
                    
                    Frame().send(*client, Frame::block::none, Frame::more::more);
                    f1.send(*client, Frame::block::none, Frame::more::more);
                    f2.send(*client, Frame::block::none, Frame::more::none);
                    
                    size = msg.receive(*server, Message::block::none);
                    
                });
                
                it("has expected byte count", {
                    
                    expect(size).should.equal(10);
                    
                });
                
                it("has two frames", {
                    
                    expect(msg.size()).should.equal(2);
                    
                });
                
                it("has HELLO for first frame", {
                    
                    std::string data(msg.front().data<char>(), msg.front().size());
                    
                    expect(data).should.equal("HELLO");
                    
                });
                
                it("has WORLD for second frame", {
                    
                    std::string data(msg.back().data<char>(), msg.back().size());
                    
                    expect(data).should.equal("WORLD");
                    
                });

            });
            
            context("sending", {

                beforeEach({
                    
                    Message msg;
                    
                    msg.emplace_back("HELLO");
                    msg.emplace_back("WORLD");

                    msg.send(*client, Message::block::blocking);
                    
                });
                
                context("delimiter", {
                    
                    Frame delim;

                    beforeEach({

                        delim.receive(*server, Frame::block::none);
                        
                    });

                    it("has delimiter", {
                        
                        expect(delim.size()).should.equal(0);
                        
                    });

                    context("first frame", {
                        
                        Frame first;
                        
                        beforeEach({
                            
                            first.receive(*server, Frame::block::none);
                            
                        });
                        
                        it("gets HELLO for first frame", {
                            
                            expect(first.str()).should.equal("HELLO");
                            
                        });
                        
                        context("second frame", {

                            Frame second;
                            
                            beforeEach({
                                
                                second.receive(*server, Frame::block::none);
                                
                            });
                            
                            it("gets WORLD for second frame", {
                                
                                expect(second.str()).should.equal("WORLD");
                                
                            });
                            
                        });
                        
                    });
                    
                });

            });
            
        });
        
        
    });
    
} }