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
        
        std::function<void(const Socket &socket, Message &msg)> onMessage;
        
    protected:
        
        void init() {
            
            using namespace std::placeholders;
            
            _frontendWriteObserver = std::bind(&Proxy::onFrontendWritable, this, _1, _2, _3);
            _backendWriteObserver = std::bind(&Proxy::onBackendWritable, this, _1, _2, _3);
            
            _frontendReadObserver = std::bind(&Proxy::onFrontendReadable, this, _1, _2, _3);
            _backendReadObserver = std::bind(&Proxy::onBackendReadable, this, _1, _2, _3);
            
            _reactor->addObserver(_frontend, Reactor::Event::Writable, _frontendWriteObserver);
            
            _reactor->addObserver(_backend, Reactor::Event::Writable, _backendWriteObserver);
            
        }
        
        void onFrontendWritable(const Socket &socket, const Reactor::Event &event, const bool didTimeout) {
         
            _reactor->removeObserver(_frontend, Reactor::Event::Writable);
            _reactor->addObserver(_backend, Reactor::Event::Readable, _backendReadObserver);
            
        }
        
        void onBackendWritable(const Socket &socket, const Reactor::Event &event, const bool didTimeout) {
          
            _reactor->removeObserver(_backend, Reactor::Event::Writable);
            _reactor->addObserver(_frontend, Reactor::Event::Readable, _frontendReadObserver);
            
        }
        
        void onFrontendReadable(const Socket &socket, const Reactor::Event &event, const bool didTimeout) {
            
            forward(_frontend, _backend);
            
            _reactor->removeObserver(_frontend, Reactor::Event::Readable);
            _reactor->addObserver(_backend, Reactor::Event::Writable, _backendWriteObserver);
            
        }
        
        void onBackendReadable(const Socket &socket, const Reactor::Event &event, const bool didTimeout) {

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
                
                if (onMessage) {
                    onMessage(reader, msg);
                }
                
                msg.send(writer, Message::block::blocking);
                
            } while (1);
            
        }
        
    private:

        std::shared_ptr<Reactor> _reactor;
        Socket _frontend;
        Socket _backend;
        
        Reactor::Observer _frontendWriteObserver;
        Reactor::Observer _backendWriteObserver;
        Reactor::Observer _frontendReadObserver;
        Reactor::Observer _backendReadObserver;
        
    };
    
}

#endif /* defined(__Messaging__Proxy__) */
