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
        
        class MessageIterator {
            
        public:
            MessageIterator(const std::shared_ptr<void> &socket, const int flags = 0)
            :
                _socket(socket),
                _flags(flags),
                _valid(false),
                _close_msg(false)
            {
                _valid = receive();
            }
            
            MessageIterator(const MessageIterator &) = delete;
            
            ~MessageIterator() {
                
                if (_valid) {
                    while (more()) {
                        // dump remainder of message
                    }
                }
                
                if (_close_msg) {
                    zmq_msg_close(&_msg);
                }
                
            }
            
            const void *data() {
                
                if (_valid) {
                    return zmq_msg_data(&_msg);
                }
                
                return nullptr;
                
            }
            
            const size_t size() {
                
                if (_valid) {
                    return zmq_msg_size(&_msg);
                }
                
                return 0;
                
            }
            
            operator bool() const {
                
                return _valid;
                
            }
            
            bool more() {
                
                _valid = receive();
                
                return _valid;
                
            }
            
        protected:
            
            bool receive() {
                
                if (_close_msg) {
                    
                    auto more = zmq_msg_more(&_msg);
                    
                    zmq_msg_close(&_msg);
                    
                    _close_msg = false;
                    
                    if ( ! more) {
                        return more;
                    }
                }
                
                zmq_msg_init(&_msg);
                _close_msg = true;
                
                auto len = zmq_msg_recv(&_msg, _socket.get(), _flags);
                
                if (len < 0) {
                    if (len == EAGAIN) {
                        return false;
                    }
                    
                    throw Exception("Receiving message part");
                }
                
                //                std::cout << std::this_thread::get_id() << ": [" << std::string((char*)zmq_msg_data(&_msg),zmq_msg_size(&_msg)) << "]" << std::endl;
                
                return true;
                
            }
            
        private:
            std::shared_ptr<void> _socket;
            zmq_msg_t _msg;
            int _flags;
            bool _valid;
            bool _close_msg;
            
        };
        
        template <class T>
        class Dispatcher {
            
        public:
            Dispatcher(const std::shared_ptr<T> msg)
            :
                _msg(msg),
                _handled(false)
            {}
            
            template <class M>
            Dispatcher &handle(const std::function<bool(const M&)> &func) {
                
                if ( ! _handled) {
                    
                    auto specific = std::dynamic_pointer_cast<M>(_msg);
                    
                    if (specific) {
                        _handled = func(*specific);
                    }
                    
                }
                
                return *this;
            }
            
            Dispatcher &handle(const std::function<bool()> &func) {
                
                if (!_handled) {
                    _handled = func();
                }
                return *this;
                
            }
            
        private:
            
            std::shared_ptr<T> _msg;
            bool _handled;
            
        };
        
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
