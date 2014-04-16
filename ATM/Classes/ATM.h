//
//  ATM.h
//  ATM
//
//  Created by Dave Meehan on 08/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ATM__ATM__
#define __ATM__ATM__

#include <string>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <zmq.h>
#include <functional>

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
    
    class Exception : public std::runtime_error {
        
    public:
        
        Exception(const std::string &what, const errno_t error_number = errno)
        :
            std::runtime_error(what + std::string(" - ") + std::string(zmq_strerror(error_number)))
        {}
        
    };
    
    class Socket;
    
    class ISerialisable {
        
    public:
        
        virtual int serialise(Socket &socket, const int flags = 0) const = 0;

    };
    
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
    
        int send(const std::string &data, const int flags = 0) const {
            
            return zmq_send(_socket.get(), data.c_str(), data.size(), flags);
            
        }
        
        int send(const ISerialisable &message) {
           
            return message.serialise(*this);
            
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

namespace ATM {

    class WithdrawlRequest : public Messaging::ISerialisable {
        
    public:
        
        enum class State {
            Hello,
            Account,
            Pin,
            Amount
        };
        
        WithdrawlRequest()
        :
        _account(""),
        _pin(""),
        _amount(0),
        _state(State::Hello)
        {}
        
    private:
        
        std::string _account;
        std::string _pin;
        unsigned _amount;
        State _state;
        
    };

    class Machine {
    
    public:
        
        using state = void (Machine::*)();
        
        Machine(const Messaging::Context &context)
        :
            _customer(context, ZMQ_REP)
        {}
        
        void operator()() {
        
            _customer.connect("tcp://localhost:5555");
            
            state state = &Machine::wait_hello;
            
            while (state) {
                
                (this->*state)();

            }
        }
        
        class Message {
        
        public:
            virtual ~Message() {}
        };
        
        class hello : public Message, public Messaging::ISerialisable {
        
        public:

            static const std::string ident() {
                static const std::string hello("HELLO");
                return hello;
            }
            
            static std::shared_ptr<hello> deserialise(Messaging::Socket::MessageIterator &iterator) {
                
                std::string string((char*)iterator.data(), iterator.size());
                
                if (string == ident()) {
                    
                    return std::make_shared<hello>();
                    
                }

                return nullptr;
            }
        
            int serialise(Messaging::Socket &socket, const int flags) const override {
                
                socket.send(ident(), flags);
                
                return 0;
                
            }
        };
        
        class account : public Message, public Messaging::ISerialisable {
            
        public:
            
            static const std::string ident() {
                static const std::string account("ACCOUNT");
                return account;
            }
            
            static std::shared_ptr<account> deserialise(Messaging::Socket::MessageIterator &iterator) {
                
                std::string string((char*)iterator.data(), iterator.size());
                
                if (string == ident()) {
                    
                    return std::make_shared<account>();
                    
                }
                
                return nullptr;
                
            }

            int serialise(Messaging::Socket &socket, const int flags) const override {
                
                socket.send(ident(), flags);
                
                return 0;
                
            }
        };

        class MessageFactory {
            
        public:
            
            using value_type = Message;
            using pointer_type = std::shared_ptr<Message>;
          
            static pointer_type create(Messaging::Socket::MessageIterator &iterator) {
                
                pointer_type msg;
                
                if ((msg = hello::deserialise(iterator))) {
                    return msg;
                }
                
                if ((msg = account::deserialise(iterator))) {
                    return msg;
                }
                    
                return std::make_shared<Message>();
                
            }
            
        };
        
        void wait_hello() {
            
            _customer.receive<MessageFactory>()
            .handle<hello>([&](const hello &h) {
                
                std::cout << std::this_thread::get_id() << ": Hello!" << std::endl;
                
                _customer.send("OK");
                
                return true;
                
            }).handle<account>([&](const account &a) {
              
                std::cout << std::this_thread::get_id() << ": Account" << std::endl;
                
                _customer.send("OK");
                
                return true;
                
            }).handle<Message>([&](const Message &) {
                
                std::cout << std::this_thread::get_id() << ": Unrecognised Response" << std::endl;
                
                _customer.send("FAIL");
                
                return true;
                
            });
            
        }
        
    private:
        
        Messaging::Socket _customer;
        
        
    };
    
}

#endif /* defined(__ATM__ATM__) */
