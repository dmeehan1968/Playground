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
#include "Reactor.h"

/*
 
 <class name = "stream">

    <socket name = "frontend" codec = "stream" />
    <socket name = "backend" codec = "session" />
 
    <state name = "open">
 
        <event name = "DATA" next = "ready">
 
            <action name="is empty" />
 
        </event>
 
        <event name = "terminate" next = "close">
 
            <action name="send" message="" />
 
        </event>
 
    </state>
 
    <state name = "ready">
 
        <event name = "DATA" next = "ready">
 
            <action name = "is empty" />
 
        </event>
 
        <event name = "terminate" next = "close">
 
            <action name="send" message="" />
 
        </event>
 
    </state>
 
 </class>
 
 */


namespace StreamProtocol {
    
    class Event {
    
    public:
        
        Event(const Messaging::Frame &frame)
        :
            replyTo(frame)
        {}
        
        virtual ~Event() {}
        
        Messaging::Frame replyTo;
        
    };
    
    class DataEvent : public Event {
        
    public:
        
        DataEvent(const Event &event, const Messaging::Frame &frame)
        :
            Event(event),
            data(frame)
        {}
        
        Messaging::Frame data;
        
    };
    
    class TerminateEvent : public Event {
        
    public:
        
        TerminateEvent(const Event &event)
        :
            Event(event)
        {}
        
    };
    
    class Codec {

    public:
        
        enum class State {
          
            ReplyAddress,
            Data
            
        };
        
        Codec()
        :
            _state(State::ReplyAddress)
        {}
        
        std::shared_ptr<Event> decode(const Messaging::Frame &frame) {

            switch (_state) {
                    
                case State::ReplyAddress:
                    
                    _state = State::Data;
                    _event = std::make_shared<Event>(frame);
                    break;
                    
                case State::Data:
                    
                    _state = State::ReplyAddress;
                    _event = std::make_shared<DataEvent>(*_event, frame);
                    break;
                    
            }
            
            return frame.hasMore() ? nullptr : _event;
            
        }
        
    private:
        
        State _state;
        std::shared_ptr<Event> _event;
        
    };
    
    
}

#endif /* defined(__Messaging__StreamProtocol__) */
