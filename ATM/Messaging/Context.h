//
//  Context.h
//  ATM
//
//  Created by Dave Meehan on 16/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ATM__Context__
#define __ATM__Context__

#include <zmq.h>
#include <memory>

namespace Messaging {
    
    class Context {
        
    public:
        
        Context()
        :
            _context(std::shared_ptr<void>(zmq_ctx_new(),&zmq_ctx_destroy))
        {}
        
        void *get() const {
            return _context.get();
        }
        
    private:
        
        std::shared_ptr<void> _context;
        
    };
    
}


#endif /* defined(__ATM__Context__) */
