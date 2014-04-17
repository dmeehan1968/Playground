//
//  Exception.h
//  ATM
//
//  Created by Dave Meehan on 16/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ATM__Exception__
#define __ATM__Exception__

#include <stdexcept>
#include <string>

#include <errno.h>

#include <zmq.h>

namespace Messaging {
    
    class Exception : public std::runtime_error {
        
    public:
        
        Exception(const std::string &what, const errno_t error_number = errno)
        :
        std::runtime_error(what + std::string(" - ") + std::string(zmq_strerror(error_number)))
        {}
        
    };
    
}
#endif /* defined(__ATM__Exception__) */
