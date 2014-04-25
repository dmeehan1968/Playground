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
#include "Exception.h"

#include <zmq.h>

namespace Messaging {
    
    namespace Specs {
        class SocketSpec;
    }

    class Socket {
        
    public:
        
        enum class Type {
            
            request     = ZMQ_REQ,
            reply       = ZMQ_REP,
            publisher   = ZMQ_PUB,
            subscriber  = ZMQ_SUB,
            push        = ZMQ_PUSH,
            pull        = ZMQ_PULL,
            dealer      = ZMQ_DEALER,
            router      = ZMQ_ROUTER,
            stream      = ZMQ_STREAM
            
        };
        
        Socket(const Context &ctx, const Type type)
        :
            _ctx(ctx),
            _socket(std::shared_ptr<void>(zmq_socket(_ctx.get(), (int)type), &zmq_close))
        {}
        
        ~Socket() {
            _socket = nullptr;
        }
        
        bool operator == (const Socket &other) const {
            return _ctx == other._ctx && _socket == other._socket;
        }
        
        bool operator < (const Socket &other) const {
            return _ctx == other._ctx && _socket < other._socket;
        }
        
        Type type() const {
            
            int type;
            size_t type_size = sizeof(type);
            
            if (zmq_getsockopt(get(), ZMQ_TYPE, &type, &type_size) < 0) {
                throw Exception("Get socket option");
            }
            
            return (Type) type;
        }

        using endpoint = std::string;
        
        void bind(const endpoint &endpoint) {
    
            auto rc = zmq_bind(get(), endpoint.c_str());
            
            if (rc < 0) {
                throw Exception("bind failed");
            }
        
        }
        
        void connect(const endpoint &endpoint) {
        
            auto rc = zmq_connect(get(), endpoint.c_str());
            
            if (rc < 0) {
                throw Exception("connect failed");
            }
            
        }
        
        void setSendHighWaterMark(const int hwm) {
            
            auto rc = zmq_setsockopt(get(), ZMQ_SNDHWM, &hwm, sizeof(hwm));
            
            if (rc < 0) {
                throw Exception("set send high water mark failed");
            }
            
        }
        
        void setReceiveHighWaterMark(const int hwm) {
            
            auto rc = zmq_setsockopt(get(), ZMQ_RCVHWM, &hwm, sizeof(hwm));
            
            if (rc < 0) {
                throw Exception("set receive high water mark failed");
            }
            
        }
        
        void setThreadAffinity(const uint64_t affinity) {
            
            auto rc = zmq_setsockopt(get(), ZMQ_AFFINITY, &affinity, sizeof(affinity));
            
            if (rc < 0) {
                throw Exception("set receive high water mark failed");
            }
            
        }
        
        void addSubscribeFilter(const void *data, const size_t len) {
            
            auto rc = zmq_setsockopt(get(), ZMQ_SUBSCRIBE, data, len);

            if (rc < 0) {
                throw Exception("add subscribe filter failed");
            }
            
        }
        
        void removeSubscribeFilter(const void *data, const size_t len) {

            auto rc = zmq_setsockopt(get(), ZMQ_UNSUBSCRIBE, data, len);
            
            if (rc < 0) {
                throw Exception("remove subscribe filter failed");
            }
            
        }
        
        void setReceiveTimeout(const int timeout) {
            
            auto rc = zmq_setsockopt(get(), ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
            
            if (rc < 0) {
                throw Exception("set receive timeout failed");
            }
            
        }
        
        void setSendTimeout(const int timeout) {
            
            auto rc = zmq_setsockopt(get(), ZMQ_SNDTIMEO, &timeout, sizeof(timeout));
            
            if (rc < 0) {
                throw Exception("set send timeout failed");
            }
            
        }
        
    protected:

        friend class Specs::SocketSpec;
        friend class Frame;
        friend class Poller;
        
        void *get() const {
            return _socket.get();
        }

    private:
        
        Context                 _ctx;
        std::shared_ptr<void>   _socket;
        
    };
    
}

#endif /* defined(__Messaging__Socket__) */
