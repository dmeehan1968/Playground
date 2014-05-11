//
//  StreamProtocol.h
//  Messaging
//
//  Created by Dave Meehan on 09/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__StreamProtocol__
#define __Messaging__StreamProtocol__

#include "Message.h"

namespace Messaging {
    
    class Event {
        
    public:
        
        Event(const Messaging::Frame &frame)
        :
        replyTo(frame)
        {}
        
        virtual ~Event() {}
        
        Messaging::Frame replyTo;
        
    };
    
    class AbstractCodec {
        
    public:
        
        virtual void reset() = 0;
        virtual std::shared_ptr<Messaging::Event> decode(const Messaging::Frame &frame) = 0;
        
    };
}

namespace StreamProtocol {
    
    class RawEvent : public Messaging::Event {
        
    public:
        
        RawEvent(const Event &event, const Messaging::Frame &frame)
        :
            Event(event),
            data(frame)
        {}
        
        Messaging::Frame data;
        
    };
    
    class TerminateEvent : public Messaging::Event {
        
    public:
        
        TerminateEvent(const Messaging::Event &event)
        :
            Event(event)
        {}
        
    };
    
    class Codec : public Messaging::AbstractCodec {

    public:
        
        enum class State {
          
            ReplyAddress,
            Data,
            Complete
            
        };
        
        Codec() {
            
            reset();
            
        }
        
        void reset() {
            
            _state = State::ReplyAddress;
            _event = nullptr;
            
        }
        
        std::shared_ptr<Messaging::Event> decode(const Messaging::Frame &frame) {

            switch (_state) {
                    
                case State::ReplyAddress:
                    
                    _state = State::Data;
                    _event = std::make_shared<Messaging::Event>(frame);
                    break;
                    
                case State::Data:
                    
                    _state = State::Complete;
                    _event = std::make_shared<RawEvent>(*_event, frame);
                    break;
                    
                case State::Complete:

                    break;
            }
            
            return frame.hasMore() ? nullptr : _event;
            
        }
        
    private:
        
        State _state;
        std::shared_ptr<Messaging::Event> _event;
        
    };
    
    
}

#endif /* defined(__Messaging__StreamProtocol__) */
