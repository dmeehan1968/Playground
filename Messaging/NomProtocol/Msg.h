//
//  Msg.h
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__Msg__
#define __Messaging__Msg__


#include "Frame.h"

#include <ostream>
#include <deque>

namespace Messaging { namespace Protocol {
  
    class Msg {
        
    public:
        
        Msg()
        :
            _state(State::Address)
        {}
        
        virtual bool decode(const Frame &frame) {
            
            switch (_state) {
                    
                case State::Address:
                    
                    address = frame;
                    _state = State::Delimiter;
                    break;
                    
                case State::Delimiter:
                    
                    _state = State::Identity;
                    break;
                    
                case State::Identity:
                    
                    identity = frame;
                    _state = State::_Final;
                    break;
                    
                case State::_Final:
                    
                    throw std::runtime_error("invalid frame for AbstractMsg");
                    
            }
            
            return isFinal();
            
        }
        
        virtual bool isFinal() const {
            
            return _state == State::_Final;
            
        }
        
        virtual Msg *clone() const {
            return new Msg(*this);
        };
        
        virtual std::deque<Frame> encode(Socket * const socket = nullptr, const Frame::more more_type = Frame::more::none) {
            
            if (socket == nullptr) {
                
                std::deque<Frame> frames;
                
                frames.emplace_back(address);
                frames.emplace_back(Frame());
                frames.emplace_back(identity);
                
                return frames;
            }
            
            address.send(*socket, Frame::block::blocking, Frame::more::more);
            Frame().send(*socket, Frame::block::blocking, Frame::more::more);
            identity.send(*socket, Frame::block::blocking, more_type);
            
            return {};
        }
        
        Frame address;
        Frame identity;
        
        virtual bool operator == (const Msg &msg) const {
            
            return address == msg.address && identity == msg.identity;
            
        }
        
        virtual bool operator != (const Msg &msg) const {
            
            return ! operator == (msg);
        }
        
        template <class T>
        bool isa() {
            
            try {
              
                (void)dynamic_cast<T*>(this);
                
            } catch (std::bad_cast &) {
                
                return false;
                
            }
            
            return true;
            
        }
        
        void reply(const Msg &msg) {
            
            address = msg.address;

            if ( ! _socket) {
                errno = ENOTSOCK;
                throw Exception("no source socket assigned");
            }
            
            Socket socket = msg.socket();
            encode(&socket, Frame::more::none);
            
        }
        
        const Socket &socket() const {
            return *_socket;
        }
        
        void setSocket(const Socket &socket) {
            
            _socket = std::make_shared<Socket>(socket);
            
        }
        
    private:
        
        enum class State {
            
            Address,
            Delimiter,
            Identity,
            _Final
            
        };
        
        State _state;
        mutable std::shared_ptr<Socket> _socket;
        
    };
    
    inline std::ostream &operator << (std::ostream &stream, const Msg &msg) {

        return stream << "Msg";
        
    }
} }

#endif /* defined(__Messaging__Msg__) */
