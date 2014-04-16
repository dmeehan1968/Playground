//
//  ATM.h
//  ATM
//
//  Created by Dave Meehan on 08/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ATM__ATM__
#define __ATM__ATM__

#include "Messaging.h"

#include <sstream>
#include <iostream>
#include <thread>

namespace ATM {

    class WithdrawlRequest : public Messaging::Message, public Messaging::ISerialisable {
        
    public:
        
        enum class State {
            Hello,
            Account,
            Pin,
            Amount,
            Authorise
        };
        
        WithdrawlRequest()
        :
            _account(""),
            _pin(""),
            _amount(0),
            _state(State::Hello)
        {}
        
        virtual int serialise(Messaging::Socket &socket, const int flags) const {
        
            auto sent = socket.send("WITHDRAWL", flags | ZMQ_SNDMORE);
            sent += socket.send((char)_state, flags | ZMQ_SNDMORE);
            sent += socket.send(_account, flags | ZMQ_SNDMORE);
            sent += socket.send(_pin, flags | ZMQ_SNDMORE);
            std::ostringstream os;
            os << _amount;
            sent += socket.send(os.str(), flags);
            
            return sent;
        
        }
        
        static std::shared_ptr<WithdrawlRequest> deserialise(Messaging::MessageIterator &iterator) {
        
            if (memcmp(iterator.data(), "WITHDRAWL", 9) == 0) {
        
                auto request = std::make_shared<WithdrawlRequest>();
                
                iterator.more();
                
                request->_state = (State) * ((char *)iterator.data());
                
                iterator.more();
                
                request->_account = std::string((char*)iterator.data(), iterator.size());
                
                iterator.more();
                
                request->_pin = std::string((char*)iterator.data(), iterator.size());
                
                iterator.more();
                
                std::string amount((char*)iterator.data(), iterator.size());
                
                std::istringstream is(amount);
                
                is >> request->_amount;
                
                return request;
                
            }
            return nullptr;
            
        }
        
        std::string _account;
        std::string _pin;
        unsigned _amount;
        State _state;
        
    };

    class MessageFactory {
        
    public:
        
        using value_type = Messaging::Message;
        using pointer_type = std::shared_ptr<value_type>;
        
        static pointer_type create(Messaging::MessageIterator &iterator) {
            
            pointer_type msg;
            
            if ((msg = WithdrawlRequest::deserialise(iterator))) {
                return msg;
            }
            
            return nullptr;
            
        }
        
    };
    
    class Machine {
    
    public:
        
        using state = void (Machine::*)();
        
        Machine(const Messaging::Context &context)
        :
            _customer(context, ZMQ_REP)
        {}
        
        void operator()() {
        
            _customer.connect("inproc://atm");
            
            state state = &Machine::wait_message;
            
            while (state) {
                
                (this->*state)();

            }
        }
        
        void wait_message() {
            
            _customer.receive<MessageFactory>()
            .handle<WithdrawlRequest>([&](const WithdrawlRequest &request) {
                
                auto response = request;
                
                switch (request._state) {

                    case WithdrawlRequest::State::Hello:
                        
                        std::cout << std::this_thread::get_id() << ": Please Insert Your Card" << std::endl;
                        
                        response._state = WithdrawlRequest::State::Account;
                        break;
                    
                    case WithdrawlRequest::State::Account:
                        
                        if (request._account.size() == 10) {
                            
                            std::cout << std::this_thread::get_id() << ": Please Input Your PIN" << std::endl;
                            
                            response._state = WithdrawlRequest::State::Pin;
                        }
                        
                    case WithdrawlRequest::State::Pin:
                        
                        if (request._pin.size() == 4) {
                            
                            std::cout << std::this_thread::get_id() << ": Please Input the Amount to Withdraw" << std::endl;
                            
                            response._state = WithdrawlRequest::State::Amount;
                        }
                        
                    case WithdrawlRequest::State::Amount:
                        
                        if (request._amount > 0) {
                            
                            std::cout << std::this_thread::get_id() << ": Please Wait, Authorising..." << std::endl;
                            
                            response._state = WithdrawlRequest::State::Authorise;
                        }
                        
                    default:
                        break;
                }
                
                _customer.send(response);
                
                return true;
                
            }).handle<Messaging::Message>([&](const Messaging::Message &msg) {
                
                std::cout << std::this_thread::get_id() << ": Invalid!" << std::endl;

                _customer.send("FAIL");
                
                return true;
                
            }).handle([&]() {
                
                _customer.send("");
                return true;
                
            });
        }
        
    private:
        
        Messaging::Socket _customer;
        
        
    };
    
}

#endif /* defined(__ATM__ATM__) */
