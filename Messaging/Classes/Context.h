//
//  Context.h
//  Messaging
//
//  Created by Dave Meehan on 18/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__Context__
#define __Messaging__Context__

#include <zmq.h>
#include <memory>

namespace Messaging {

    namespace Specs {
        class ContextSpec;
    }
    
    class Context {
      
    public:
        
        Context() : _context(std::shared_ptr<void>(zmq_ctx_new(), &zmq_ctx_destroy)) {}
        
        ~Context() {
            _context = nullptr;
        }
        
        bool operator == (const Context &other) const {
            
            return _context.get() == other._context.get();
            
        }
        
        int getIOThreads() const {
            
            return zmq_ctx_get(_context.get(), ZMQ_IO_THREADS);
            
        }
        
        void setIOThreads(const int optval) {
            
            zmq_ctx_set(_context.get(), ZMQ_IO_THREADS, optval);
            
        }
        
        int maxSockets() const {
            
            return zmq_ctx_get(_context.get(), ZMQ_MAX_SOCKETS);
            
        }
        
        void setMaxSockets(const int optval) {
            
            zmq_ctx_set(_context.get(), ZMQ_MAX_SOCKETS, optval);
            
        }

    protected:

        friend class Specs::ContextSpec;
        friend class Socket;
        
        void *get() const {
            
            return _context.get();
            
        };
        
    private:
        
        std::shared_ptr<void> _context;
        
    };
    
}

#endif /* defined(__Messaging__Context__) */
