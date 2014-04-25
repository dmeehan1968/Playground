//
//  Proxy.h
//  Messaging
//
//  Created by Dave Meehan on 25/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__Proxy__
#define __Messaging__Proxy__

#include "Reactor.h"
#include "Message.h"

#include <functional>

using namespace std::placeholders;

namespace Messaging {
    
    class Proxy {
        
    public:
        
        Proxy(const Socket &frontend, const Socket &backend)
        :
            _reactor(std::make_shared<Reactor>()),
            _frontend(frontend),
            _backend(backend)
        {
            init();
        }
        
        Proxy(const std::shared_ptr<Reactor> &reactor, const Socket &frontend, const Socket &backend)
        :
            _reactor(reactor),
            _frontend(frontend),
            _backend(backend)
        {
            init();
        }
        
        bool runOnce(const long timeout) {
            
            return _reactor->runOnce(timeout);
            
        }
        
        void run(const long interval) {
            
            _reactor->run(interval);
            
        }
        
        void stop() {
            
            _reactor->stop();
            
        }
        
    protected:
        
        void init() {
            
            _frontendWriteObserver = std::bind(&Proxy::onFrontendWritable, this, _1, _2);
            _backendWriteObserver = std::bind(&Proxy::onBackendWritable, this, _1, _2);
            
            _frontendReadObserver = std::bind(&Proxy::onFrontendReadable, this, _1, _2);
            _backendReadObserver = std::bind(&Proxy::onBackendReadable, this, _1, _2);
            
            _reactor->addObserver(_frontend, Reactor::Event::Writable, _frontendWriteObserver);
            
            _reactor->addObserver(_backend, Reactor::Event::Writable, _backendWriteObserver);
            
        }
        
        void onFrontendWritable(Socket &socket, const Reactor::Event &event) {
         
            _reactor->removeObserver(_frontend, Reactor::Event::Writable);
            _reactor->addObserver(_backend, Reactor::Event::Readable, _backendReadObserver);
            
        }
        
        void onBackendWritable(Socket &socket, const Reactor::Event &event) {
          
            _reactor->removeObserver(_backend, Reactor::Event::Writable);
            _reactor->addObserver(_frontend, Reactor::Event::Readable, _frontendReadObserver);
            
        }
        
        void onFrontendReadable(Socket &socket, const Reactor::Event &event) {
            
            forward(_frontend, _backend);
            
            _reactor->removeObserver(_frontend, Reactor::Event::Readable);
            _reactor->addObserver(_backend, Reactor::Event::Writable, _backendWriteObserver);
            
        }
        
        void onBackendReadable(Socket &socket, const Reactor::Event &event) {

            forward(_backend, _frontend);
            
            _reactor->removeObserver(_backend, Reactor::Event::Readable);
            _reactor->addObserver(_frontend, Reactor::Event::Writable, _frontendWriteObserver);
            
        }
        
        void forward(Socket &reader, Socket &writer) {
            
            unsigned count = 0;
            
            do {
                
                Message msg;
                
                try {
                
                    msg.receive(reader, Message::block::none);
                    count++;
                    
                } catch (Exception &e) {
                    
                    if (e.errorCode() == EAGAIN && count > 0) {
                        return;
                    }
                    
                    throw;
                    
                }
                
                msg.send(writer, Message::block::blocking);
                
            } while (1);
            
        }
        
    private:

        std::shared_ptr<Reactor> _reactor;
        Socket _frontend;
        Socket _backend;
        
        Reactor::EventObserver _frontendWriteObserver;
        Reactor::EventObserver _backendWriteObserver;
        Reactor::EventObserver _frontendReadObserver;
        Reactor::EventObserver _backendReadObserver;
        
    };
    
}

#endif /* defined(__Messaging__Proxy__) */
