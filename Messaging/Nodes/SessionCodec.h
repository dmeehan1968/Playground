//
//  SessionCodec.h
//  Messaging
//
//  Created by Dave Meehan on 12/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__SessionCodec__
#define __Messaging__SessionCodec__

#include "Frame.h"

#include <deque>

/*
 
 <header>
 
    <field name = "id" />
 
 </header>
 
 <message name = "abc" id = "abc">
 
 </message>
 
 <message name = "def" id = "def">
 
    <field name = "d" type = "frame" />
    <field name = "e" type = "frame" />
    <field name = "f" type = "frame" />
 
 </message>
 
 */

namespace Messaging { namespace Nodes {

    class AbstractMsg {
    
    public:
        
        using Frame = Messaging::Frame;
        using String = std::string;
        using Number = int;
        
        AbstractMsg()
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
        
        virtual AbstractMsg *clone() const {
            return new AbstractMsg(*this);
        };
        
        virtual std::deque<Frame> encode() const {
            
            std::deque<Frame> frames;

            frames.emplace_back(address);
            frames.emplace_back(Frame());
            frames.emplace_back(identity);
            
            return frames;
            
        }
        
        Frame address;
        Frame identity;
        
    private:
        
        enum class State {
            
            Address,
            Delimiter,
            Identity,
            _Final
            
        };
        
        State _state;
        
    };
    
    class AbcMsg : public AbstractMsg {
        
    public:
        
        AbcMsg() {}
        
        AbcMsg(const AbstractMsg &msg)
        :
            AbstractMsg(msg)
        {}
        
        virtual bool isFinal() const override {
            return true;
        }
        
        virtual AbcMsg *clone() const override {
            return new AbcMsg(*this);
        }
        
        virtual std::deque<Frame> encode() const override {
        
            return AbstractMsg::encode();
            
        }
    };
    
    class DefMsg : public AbstractMsg {
        
    public:
        
        DefMsg() {}
        
        DefMsg(const AbstractMsg &msg)
        :
            AbstractMsg(msg),
            _state(State::d)
        {}
        
        virtual bool isFinal() const override {
            return _state == State::_Final;
        }
        
        virtual bool decode(const Frame &frame) override {
        
            switch (_state) {
                    
                case State::d:
                {
                    d = frame;
                    _state = State::e;
                    break;
                }
                    
                case State::e:
                {
                    e = frame;
                    _state = State::f;
                    break;
                }
                    
                case State::f:
                {
                    f = frame;
                    _state = State::_Final;
                    break;
                }
                    
                default:
                {
                    throw std::runtime_error("invalid frame for DefMsg");
                    break;
                }
            }
            
            return isFinal();
        }
        
        virtual std::deque<Frame> encode() const override {

            auto frames = AbstractMsg::encode();
            
            frames.emplace_back(d);
            frames.emplace_back(e);
            frames.emplace_back(f);
            
            return frames;
            
        }
        
        Frame d;
        Frame e;
        Frame f;
        
    private:
        
        enum class State {
            
            d,
            e,
            f,
            _Final
            
        };
        
        State _state;
        
    };
    
    class SessionCodec {
    
    public:
        
        SessionCodec() {
            
            reset();
            
        }
        
        void reset() {
            
            _msgType = MsgType::AbstractMsg;
            _msg = std::make_shared<AbstractMsg>();
            
        }
        
        std::shared_ptr<AbstractMsg> decode(const Frame &frame) {
        
            switch (_msgType) {
                    
                case MsgType::AbstractMsg:
                {
                    if (_msg->decode(frame)) {
                        
                        // detect message types and transform
                        
                        if (_msg->identity == "ABC") {
                            
                            _msg = std::make_shared<AbcMsg>(*_msg);
                            _msgType = MsgType::_Final;
                            
                        } else if (_msg->identity == "DEF") {

                            _msg = std::make_shared<DefMsg>(*_msg);
                            _msgType = MsgType::DefMsg;

                        } else {
                         
                            throw std::runtime_error("unexpected identity");
                            
                        }
                        
                    }
                    break;
                }
                    
                case MsgType::DefMsg:
                // Repeat for other message types
                {
                    // specific message decode handles error

                    _msg->decode(frame);
                    break;
                }
                    
                default:
                {
                    throw std::runtime_error("invalid message type");
                    break;
                }
            
            }
            
            return _msg->isFinal() ? _msg : nullptr;
            
        }
        
    private:
        
        enum class MsgType {
            
            AbstractMsg,
            
            // insert message types here
            AbcMsg,
            DefMsg,
            
            _Final
            
        };
        
        MsgType _msgType;
        std::shared_ptr<AbstractMsg> _msg;
        
    };
    
} }

#endif /* defined(__Messaging__SessionCodec__) */
