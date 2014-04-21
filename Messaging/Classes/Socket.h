//
//  Socket.h
//  Messaging
//
//  Created by Dave Meehan on 21/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__Socket__
#define __Messaging__Socket__

#include "Context.h"

#include <zmq.h>
#include <stdexcept>
#include <string>


namespace Messaging {
    
    namespace Specs {
        class SocketSpec;
    }

    class Exception : public std::runtime_error {
    
    public:
        Exception(const char *what, errno_t error = errno)
        :
            std::runtime_error(std::string(what) + zmq_strerror(error))
        {}
        
    };
    
    class Socket {
        
    public:
        
        enum class socket_type {
            request = ZMQ_REQ,
            reply = ZMQ_REP
        };
        
        Socket(Context &ctx, const socket_type type)
        :
            _ctx(ctx),
            _socket(std::shared_ptr<void>(zmq_socket(_ctx, (int)type), &zmq_close))
        {}
        
        ~Socket() {
            _socket = nullptr;
        }
        
        socket_type type() const {
            
            int type;
            size_t type_size = sizeof(type);
            
            if (zmq_getsockopt(*this, ZMQ_TYPE, &type, &type_size) < 0) {
                throw Exception("Get socket option");
            }
            
            return (socket_type) type;
        }

        using endpoint = std::string;
        
        void bind(const endpoint &endpoint) {
    
            auto rc = zmq_bind(*this, endpoint.c_str());
            
            if (rc < 0) {
                throw Exception("bind failed");
            }
        
        }
        
        void connect(const endpoint &endpoint) {
        
            auto rc = zmq_connect(*this, endpoint.c_str());
            
            if (rc < 0) {
                throw Exception("connect failed");
            }
            
        }
        
    protected:

        friend class Specs::SocketSpec;
        friend class Frame;
        
        operator void *() const {
            return _socket.get();
        }

    private:
        
        Context                 _ctx;
        std::shared_ptr<void>   _socket;
        
    };
    
}

#endif /* defined(__Messaging__Socket__) */
