//
//  SingleVsMultiPartSpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 07/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "Frame.h"
#include <thread>

namespace Messaging { namespace Benchmark {
  
    class SinglePublisher {
        
    public:
        
        SinglePublisher(Context ctx, const std::chrono::seconds &duration)
        :
            _ctx(ctx),
            _push(_ctx, Socket::Type::push),
            _duration(duration),
            _frames(0)
        {}
        
        void operator()() {
            
            _push.bind("tcp://*:5555");
            
            auto endTime = std::chrono::high_resolution_clock::now() + _duration;
            
            std::string msg;
            msg.reserve(100);

            do {
                
                msg = "ID";
                msg += ",1";
                msg += ",FIELD_1";
                msg += ",FIELD_2";
                msg += ",FIELD_3";
                msg += ",FIELD_4";
                msg += ",FIELD_5";
                
                Frame(msg).send(_push, Frame::block::blocking, Frame::more::none);
                
                _frames++;
                
            } while(std::chrono::high_resolution_clock::now() < endTime);
            
        }
        
        size_t frames() const {
            return _frames;
        }
        
    private:
        
        Context _ctx;
        Socket _push;
        std::chrono::seconds _duration;
        size_t _frames;
        
    };
    
    class SingleSubscriber {
        
    public:
        
        SingleSubscriber(Context ctx)
        :
            _ctx(ctx),
            _pull(_ctx, Socket::Type::pull),
            _frames(0)
        {}
    
        void operator()() {
            
            _pull.connect("tcp://localhost:5555");
            
            _pull.setReceiveTimeout(500);
            
            std::string msg;
            
            do {
                
                Frame frame;
                
                try {
                    
                    frame.receive(_pull, Frame::block::blocking);
                    
                } catch (Exception &e) {
                    
                    if (e.errorCode() == EAGAIN) {
                        break;
                    }
                    throw;
                }
                
                _frames++;
                
                msg = frame.str();
                
                size_t pos;

                while ((pos=msg.find(',')) != SIZE_T_MAX) {
                    
                    auto field = msg.substr(0, pos);
                    msg = msg.substr(pos+1, msg.size() - (pos+1));
                    
                }
            
            } while (1);
            
        }
        
        size_t frames() const {
            return _frames;
        }
        
    private:
        
        Context _ctx;
        Socket _pull;
        size_t _frames;
        
    };
    
    class MultiPublisher {
        
    public:
        MultiPublisher(Context ctx, std::chrono::seconds &duration)
        :
            _ctx(ctx),
            _push(_ctx, Socket::Type::push),
            _duration(duration),
            _frames(0)
        {}
        
        void operator()() {
            
            _push.bind("tcp://*:5556");
            
            auto endTime = std::chrono::high_resolution_clock::now() + _duration;
            
            do {
                
                Frame("ID").send(_push, Frame::block::blocking, Frame::more::more);
                Frame("1").send(_push, Frame::block::blocking, Frame::more::more);
                Frame("FIELD_1").send(_push, Frame::block::blocking, Frame::more::more);
                Frame("FIELD_2").send(_push, Frame::block::blocking, Frame::more::more);
                Frame("FIELD_3").send(_push, Frame::block::blocking, Frame::more::more);
                Frame("FIELD_4").send(_push, Frame::block::blocking, Frame::more::more);
                Frame("FIELD_5").send(_push, Frame::block::blocking, Frame::more::none);

                _frames++;
                
            } while (std::chrono::high_resolution_clock::now() < endTime);
            
        }

        size_t frames() const {
            return _frames;
        }

    private:
        
        Context _ctx;
        Socket _push;
        std::chrono::seconds _duration;
        size_t _frames;
    };
    
    class MultiSubscriber{
        
    public:
        MultiSubscriber(Context ctx)
        :
            _ctx(ctx),
            _pull(_ctx, Socket::Type::pull),
            _frames(0)
        {}
        
        void operator()() {
            
            _pull.connect("tcp://localhost:5556");

            _pull.setReceiveTimeout(500);
            
            std::string msg;
            msg.reserve(100);

            do {
            
                try {
                    
                    Frame frame;
                    size_t frameCount = 0;
                    msg = "";
                    
                    do {
                        
                        frame.receive(_pull, Frame::block::blocking);
                        
                        msg += ((frameCount > 0) ? "," : "") + frame.str();
                        
                        frameCount++;

                    } while (frame.hasMore());
                    
                    if (frameCount != 7) throw "Incorrect frame count";
                    
                } catch (Exception &e) {
                    
                    if (e.errorCode() == EAGAIN) {
                        break;
                    }
                    throw;
                }
                
                _frames++;
                
            } while (1);
            
        }
        
        size_t frames() const {
            return _frames;
        }
        
    private:
        
        Context _ctx;
        Socket _pull;
        size_t _frames;
    };
    
    describe(SingleVsMultiPart, {

        Context ctx;
        
        std::chrono::seconds duration(0);

        if (duration.count() > 0) {
            
            context("single", {
                
                std::vector<std::thread> _threads;
                
                auto singlePub = SinglePublisher(ctx, duration);
                auto singleSub = SingleSubscriber(ctx);
                
                _threads.emplace_back(std::ref(singlePub));
                _threads.emplace_back(std::ref(singleSub));
                
                std::for_each(_threads.begin(), _threads.end(), std::mem_fn(&std::thread::join));
                
                std::cout << "    : " << (singleSub.frames() / duration.count()) << " Messages Per Second" << std::endl;
                
                it("receives whats sent", {
                    
                    expect(singlePub.frames()).should.equal(singleSub.frames());
                    
                });
                
            });
            
            context("multi", {
                
                std::vector<std::thread> _threads;
                
                auto multiPub = MultiPublisher(ctx, duration);
                auto multiSub = MultiSubscriber(ctx);
                
                _threads.emplace_back(std::ref(multiPub));
                _threads.emplace_back(std::ref(multiSub));
                
                std::for_each(_threads.begin(), _threads.end(), std::mem_fn(&std::thread::join));
                
                std::cout << "    : " << (multiSub.frames() / duration.count()) << " Messages Per Second" << std::endl;
                
                it("receives whats sent", {
                    
                    expect(multiPub.frames()).should.equal(multiSub.frames());
                    
                });
                
            });

        }

    });
    
} }
