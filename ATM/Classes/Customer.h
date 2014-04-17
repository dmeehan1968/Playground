//
//  Customer.h
//  ATM
//
//  Created by Dave Meehan on 17/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ATM__Customer__
#define __ATM__Customer__

#include "Messaging.h"

#include <memory>
#include <functional>
#include <map>

namespace Messaging {
    
    class MapMessage : public Message, public ISerialisable {
        
    public:
        
        virtual int serialise(Socket &socket, const int flags = 0) const {

            auto iter = _map.begin();
            
            while (iter != _map.end()) {
            
                socket.send(iter->first, flags | ZMQ_SNDMORE);
                socket.send(iter->second, flags | ZMQ_SNDMORE);
                
                iter++;
                
            }
            
            socket.send("", flags);
            
            return 0;
            
        }
        
    private:
        
        std::map<std::string, std::string> _map;
        
    };
    
}

namespace ATM {
    
    class WithdrawalRequest : public Messaging::MapMessage {
        
    public:
        
        virtual int serialise(Messaging::Socket &socket, const int flags = 0) const {
         
            auto size = Messaging::MapMessage::serialise(socket, flags);
            
            return size;
            
        }
    };
    
    class Customer {
        
    public:

        using State = void(Customer::*)();

        Customer(const Messaging::Context &context)
        :
            _atm(context, ZMQ_REQ),
            _cash(0)
        {}
        
        unsigned getCash(const unsigned &amount) {
        
            _nextState = &Customer::connect;
            
            while (_nextState) {
                (this->*_nextState)();
            }
            
            return _cash;
            
        }

    protected:
        
        void connect() {

            _atm.bind("inproc://atm");
            _nextState = &Customer::sendRequest;
            
        }
        
        void sendRequest() {
            
            _atm.send(ATM::WithdrawlRequest());
            
        }
        
    private:
        
        Messaging::Socket _atm;
        State _nextState;
        unsigned _cash;
        
    };
}
#endif /* defined(__ATM__Customer__) */
