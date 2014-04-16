//
//  Socket.h
//  ATM
//
//  Created by Dave Meehan on 16/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ATM__Socket__
#define __ATM__Socket__

#include "Context.h"
#include "Exception.h"
#include "ISerialisable.h"
#include "MessageIterator.h"
#include "Dispatcher.h"

#include <string>
#include <functional>

namespace Messaging {
    
    class Socket {
        
    public:
        
        Socket(const Context &context, const int type)
        :
            _socket(std::shared_ptr<void>(zmq_socket(context.get(), type), &zmq_close))
        {}
        
        void bind(const std::string &address) const {
            
            auto rc = zmq_bind(_socket.get(), address.c_str());
            
            if (rc != 0) {
                throw Exception("Binding to '" + address + "'");
            }
            
        }
        
        void connect(const std::string &address) const {
            
            auto rc = zmq_connect(_socket.get(), address.c_str());
            
            if (rc != 0) {
                throw Exception("Connecting to '" + address + "'");
            }
            
        }
        
        int send(const char ch, const int flags = 0) const {
            
            auto len = zmq_send(_socket.get(), &ch, sizeof(char), flags);
            
            if (len < 0) {
                throw Exception("Sending byte");
            }
            
            return len;
            
        }
        
        int send(const std::string &data, const int flags = 0) const {
            
            auto len = zmq_send(_socket.get(), data.c_str(), data.size(), flags);
            
            if (len < 0) {
                throw Exception("Sending string");
            }
            
            return len;
            
        }
        
        int send(const ISerialisable &message) {
            
            auto len = message.serialise(*this);
            
            if (len < 0) {
                throw Exception("Sending serialisable");
            }
            
            return len;
            
        }
        
        class NilFactory {
            
        public:
            using value_type = void;
            using pointer_type = std::shared_ptr<void>;
            
            static pointer_type create(MessageIterator &) {
                return pointer_type(nullptr);
            }
            
        };
        
        template <class Factory = NilFactory>
        Dispatcher<typename Factory::value_type> receive(const int flags = 0) const {
            
            MessageIterator iterator(_socket, flags);
            
            if (iterator) {
                
                return Factory::create(iterator);
                
            }
            
            return Dispatcher<typename Factory::value_type>(nullptr);
            
        }
        
    private:
        
        std::shared_ptr<void> _socket;
        
    };

}

#endif /* defined(__ATM__Socket__) */
