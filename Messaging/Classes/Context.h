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

    class Context {
      
    public:
        
        Context() : _context(std::shared_ptr<void>(zmq_ctx_new(), &zmq_ctx_destroy)) {}
        
        void *get() const {
            
            return _context.get();
            
        };
        
        int getIOThreads() const {
            
            return zmq_ctx_get(_context.get(), ZMQ_IO_THREADS);
            
        }
        
    private:
        
        std::shared_ptr<void> _context;
        
    };
    
}

#endif /* defined(__Messaging__Context__) */
