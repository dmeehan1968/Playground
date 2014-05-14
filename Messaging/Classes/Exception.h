//
//  Exception.h
//  Messaging
//
//  Created by Dave Meehan on 21/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__Exception__
#define __Messaging__Exception__

#include <stdexcept>
#include <string>
#include <errno.h>

#include <zmq.h>

namespace Messaging {
    
    class Exception : public std::runtime_error {
        
    public:
        Exception(const char *what, errno_t error = errno)
        :
            std::runtime_error(std::string(what) + (error > 0 ? (std::string(" - ") + zmq_strerror(error)) : "")),
            _error(error)
        {}
        
        errno_t errorCode() const {
            return _error;
        }
        
    private:
        
        errno_t _error;
        
    };

}

#endif /* defined(__Messaging__Exception__) */
