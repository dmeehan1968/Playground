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
        {
            if (get() == nullptr) {
                throw Exception("socket creation failed");
            }
        }
        
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
        
        void unbind(const endpoint &endpoint) {
            
            auto rc = zmq_unbind(get(), endpoint.c_str());
            
            if (rc < 0) {
                throw Exception("unbind failed");
            }
            
        }
        
        void connect(const endpoint &endpoint) {
            
            auto rc = zmq_connect(get(), endpoint.c_str());
            
            if (rc < 0) {
                throw Exception("connect failed");
            }
            
        }
        
        void disconnect() {
            
            auto rc = zmq_close(get());
            
            if (rc < 0) {
                throw Exception("disconnect failed");
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
        
        void setBacklog(const int backlog) {
            
            auto rc = zmq_setsockopt(get(), ZMQ_BACKLOG, &backlog, sizeof(backlog));
            
            if (rc < 0) {
                throw Exception("set backlog failed");
            }
            
        }
        
        void setSendBufferSize(const int size) {
            
            auto rc = zmq_setsockopt(get(), ZMQ_SNDBUF, &size, sizeof(size));
            
            if (rc < 0) {
                throw Exception("set send buffer failed");
            }
            
        }
        
        void setReceiveBufferSize(const int size) {
            
            auto rc = zmq_setsockopt(get(), ZMQ_RCVBUF, &size, sizeof(size));
            
            if (rc < 0) {
                throw Exception("set receive buffer failed");
            }
            
        }

        Socket::Type getSocketType() const {
            
            int type;
            size_t size = sizeof(type);
            
            auto rc = zmq_getsockopt(get(), ZMQ_TYPE, &type, &size);
            
            if (rc < 0) {
                throw Exception("get socket type failed");
            }
            
            switch (type) {

                case ZMQ_REQ:
                    return Type::request;
                    
                case ZMQ_REP:
                    return Type::reply;
                    
                case ZMQ_PUSH:
                    return Type::push;
                    
                case ZMQ_PULL:
                    return Type::pull;
                    
                case ZMQ_PUB:
                    return Type::publisher;
                    
                case ZMQ_SUB:
                    return Type::subscriber;
                    
                case ZMQ_DEALER:
                    return Type::dealer;
                    
                case ZMQ_ROUTER:
                    return Type::router;
                    
                case ZMQ_STREAM:
                    return Type::stream;
                    
            }

            throw Exception("unknown socket type", 0);
        }
        
        std::string getIdentity() const {
            
            std::string identity(255, NULL);
            size_t size = identity.size();
            
            auto rc = zmq_getsockopt(_socket.get(), ZMQ_IDENTITY, (void *)identity.data(), &size);
            
            if (rc < 0) {
                throw Exception("cannot get identity");
            }
            
            identity.resize(size);
            
            return identity;
            
        }
        
        void setIdentity(const std::string &identity) {
            
            auto rc = zmq_setsockopt(_socket.get(), ZMQ_IDENTITY, identity.data(), identity.size());

            if (rc < 0) {
                throw Exception("cannot set identity");
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
