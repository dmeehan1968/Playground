//
//  NomClient.h
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__NomClient__
#define __Messaging__NomClient__

#include "NomMessage.h"
#include "NomSocket.h"
#include "Dispatch.h"

#include <memory>

namespace Messaging { namespace NomProtocol {
  
    class NomClient {

    public:
        
        NomClient(const Context &context,
                  const std::string &endpoint = std::string("inproc://nom"))
        :
            _socket(context,
                    Socket::Type::dealer,
                    NomCodec::Address::Ignore,
                    NomCodec::Envelope::Use),
            _endpoint(endpoint),
            _state(State::OpenPeering)
        {
            _socket.connect(_endpoint);
        }
        
        std::shared_ptr<Msg> ohai() {
        
            return request(Ohai());
            
        }
        
        std::shared_ptr<Msg> iCanHaz() {

            return request(ICanHaz());
            
        }
        
        std::shared_ptr<Msg> hugz() {

            return request(Hugz());
            
        }
        
    protected:
        
        template <class T>
        std::shared_ptr<Msg> request(T &&msg) {
        
            switch (_state) {
                    
                case State::OpenPeering:
                {

                    if (std::is_base_of<Ohai, T>::value) {

                        _socket.send(std::forward<T>(msg));
                        
                        auto reply = _socket.receive();
                        
                        if (std::dynamic_pointer_cast<OhaiOk>(reply) ||
                            std::dynamic_pointer_cast<Wtf>(reply)) {
                            
                            _state = State::UsePeering;
                            return reply;
                            
                        }
                    }
                    break;
                }
                    
                case State::UsePeering:
                {
                    if (std::is_base_of<ICanHaz, T>::value) {
                        
                        _socket.send(std::forward<T>(msg));
                        
                        auto reply = _socket.receive();
                        
                        if (std::dynamic_pointer_cast<CheezBurger>(reply) ||
                            std::dynamic_pointer_cast<Wtf>(reply)) {
                            
                            return reply;
                        }
                        
                    } else if (std::is_base_of<Hugz, T>::value) {
                        
                        _socket.send(std::forward<T>(msg));
                        
                        auto reply = _socket.receive();
                        
                        if (std::dynamic_pointer_cast<HugzOk>(reply)) {
                            
                            return reply;
                            
                        }
                    }
                    break;
                }
                    
            }
            
            errno = EFSM;
            throw Exception("invalid reply for state");
            
        }
        
    private:

        enum class State {
        
            OpenPeering,
            UsePeering,
            
        };
        
        NomSocket _socket;
        std::string _endpoint;
        State _state;
        
        
    };
    
} }

#endif /* defined(__Messaging__NomClient__) */
