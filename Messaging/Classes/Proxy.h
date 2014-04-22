//
//  Proxy.h
//  Messaging
//
//  Created by Dave Meehan on 22/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__Proxy__
#define __Messaging__Proxy__

#include "Poller.h"
#include "Message.h"

namespace Messaging {
    
    class Proxy {
        
    public:
        
        Proxy(Socket &frontend, Socket &backend)
        :
            _done(false),
            _frontend(frontend),
            _backend(backend)
        {
            _poller.observe(frontend, Poller::Event().write());
            _poller.observe(backend, Poller::Event().write());
        }
        
        void run() {
            
            while (! _done) {
                runOnce(1000);
            }
            
        }
        
        bool runOnce(const long timeout) {
            
            if (_poller.poll(timeout)) {
                
                if (_poller(_frontend).isWritable()) {
                    
                    _poller.observe(_frontend, Poller::Event().none());
                    _poller.observe(_backend, Poller::Event().read());
                    
                }
                
                if (_poller(_backend).isWritable()) {
                    
                    _poller.observe(_backend, Poller::Event().none());
                    _poller.observe(_frontend, Poller::Event().read());
                    
                }
                
                if (_poller(_frontend).isReadable()) {
                    
                    forward(_frontend, _backend);
                    
                    _poller.observe(_frontend, Poller::Event().none());
                    _poller.observe(_backend, Poller::Event().write());
                    
                }
                
                if (_poller(_backend).isReadable()) {
                    
                    forward(_backend, _frontend);
                    
                    _poller.observe(_backend, Poller::Event().none());
                    _poller.observe(_frontend, Poller::Event().write());
                    
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
        Poller _poller;
        
    };
    

}
#endif /* defined(__Messaging__Proxy__) */
