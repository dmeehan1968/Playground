//
//  PollerProxy.h
//  Messaging
//
//  Created by Dave Meehan on 22/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__PollerProxy__
#define __Messaging__PollerProxy__

#include "Poller.h"
#include "Message.h"

namespace Messaging {
    
    class PollerProxy {
        
    public:
        
        PollerProxy(Socket &frontend, Socket &backend)
        :
            _done(false),
            _frontend(frontend),
            _backend(backend),
            _poller(std::make_shared<Poller>())
        {
            _poller->observe(frontend, { Poller::Event::Writable });
            _poller->observe(backend, { Poller::Event::Writable });
        }
        
        void run(const long interval) {
            
            while (! _done) {
                runOnce(interval);
            }
            
        }
        
        bool runOnce(const long timeout) {
            
            if (_poller->poll(timeout)) {
                
                if (_poller->receivedEvents(_frontend).is(Poller::Event::Writable)) {
                    
                    _poller->observe(_frontend, { });
                    _poller->observe(_backend, { Poller::Event::Readable });
                    
                }
                
                if (_poller->receivedEvents(_backend).is(Poller::Event::Writable)) {
                    
                    _poller->observe(_backend, { });
                    _poller->observe(_frontend, { Poller::Event::Readable });
                    
                }
                
                if (_poller->receivedEvents(_frontend).is(Poller::Event::Readable)) {
                    
                    forward(_frontend, _backend);
                    
                    _poller->observe(_frontend, { });
                    _poller->observe(_backend, { Poller::Event::Writable });
                    
                }
                
                if (_poller->receivedEvents(_backend).is(Poller::Event::Readable)) {
                    
                    forward(_backend, _frontend);
                    
                    _poller->observe(_backend, { });
                    _poller->observe(_frontend, { Poller::Event::Writable });
                    
                }
                
                return true;
                
            }
            
            return false;
            
        }
        
        void stop() {
            _done = true;
        }
        
    protected:
        
        int forward(Socket &from, Socket &to) {
            
            int count = 0;
            
            do {
                
                Message msg;
                
                try {
                    
                    msg.receive(from, Message::block::none);
                    count++;
                    
                } catch( Exception &e ) {
                    
                    if (e.errorCode() == EAGAIN && count > 0) {
                        return count;
                    }
                    
                    throw;
                }
                
                msg.send(to, Message::block::blocking);
                
            } while (1);
            
            return 0;
            
        }
        
    private:
        
        bool _done;
        Socket _frontend;
        Socket _backend;
        std::shared_ptr<Poller> _poller;
        
    };
    

}
#endif /* defined(__Messaging__Proxy__) */
