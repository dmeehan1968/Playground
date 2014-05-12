//
//  StreamServer.h
//  Messaging
//
//  Created by Dave Meehan on 11/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__StreamServer__
#define __Messaging__StreamServer__

#include "Frame.h"

#include <map>
#include <deque>
#include <iostream>

namespace Messaging { namespace Nodes { namespace StreamProtocol {

    class Event {
        
    public:
    
        Event(const Frame &frame)
        :
            address(frame)
        {}
        
        virtual ~Event() {}
        
        Frame address;
        
    };
    
    class RawEvent : public Event {
        
    public:
        
        RawEvent(const Event &event, const Frame &frame)
        :
            Event(event),
            data(frame)
        {}
        
        Frame data;
        
    };
    
    class ControlEvent : public Event {
    
    public:
        
        ControlEvent(const RawEvent &event)
        :
            Event(event),
            data(event.data)
        {}
        
        Frame address;
        Frame data;
    
    };
    
    class DataEvent : public Event {
    
    public:
        
        DataEvent(const RawEvent &event)
        :
            Event(event),
            data(event.data)
        {}
        
        Frame address;
        Frame data;
        
    };
    
    class OutboundDataEvent : public Event {
        
    public:
        
        OutboundDataEvent(const Frame &address, const Frame &data)
        :
            Event(address),
            data(data)
        {}
        
        Frame data;
        
    };
    
    class EventFactory {
    
    public:
        
        EventFactory() {
        
            reset();
            
        }
        
        void reset() {
            
            _state = State::Address;
            _event = nullptr;
            
        }
        
        enum class State {
            
            Address,
            Data,
            Complete
            
        };
        
        std::shared_ptr<Event> decode(const Frame &frame) {
            
            switch (_state) {
                    
                case State::Address:
                    
                    _event = std::make_shared<Event>(frame);
                    _state = State::Data;
                    break;
                    
                case State::Data:
                    
                    _event = std::make_shared<RawEvent>(*_event, frame);
                    _state = State::Complete;
                    break;
                    
                case State::Complete:
                    break;
                    
            }
            
            return _state == State::Complete ? _event : nullptr;
            
        }
        
    private:
        
        State _state;
        std::shared_ptr<Event> _event;
        
    };
    
    class Dispatch {
        
    public:
        
        Dispatch(const Event &event)
        :
            _event(event),
            _handled(false)
        {}
    
        template <class T>
        Dispatch &handle(const std::function<void(const T&)> &functor) {
            
            if (_handled || ! functor) {
                return *this;
            }
            
            try {
                
                auto specific = dynamic_cast<const T&>(_event);
                functor(specific);
                _handled = true;
                
            } catch (std::bad_cast &) {
                
                // pass to next handler, if any
                
            }
            
            return *this;
        }
        
    private:
        
        const Event &_event;
        bool _handled;
        
    };
    
    class ServerSession {
      
    public:
        
        ServerSession(const Socket &socket)
        :
            _socket(socket),
            _state(State::Open)
        {}
        
        enum class State {
            
            _Terminate = -1,
            Open,
            Ready
            
        };
        
        void dispatch(const std::shared_ptr<Event> &event) {
            
            _events.emplace_back(event);
            
            dispatchEvents();
            
        }
        
        void dispatchEvents() {

            while (_events.size()) {
                
                auto event = _events.front();
                
                if (event) {
                    
                    _events.pop_front();
                    
                }
                
                try {
                    
                    switch (_state) {
                            
                        case State::Open:
                            
                            Dispatch(*event).handle<RawEvent>([&](const RawEvent &event) {
                                
                                if (event.data.size() == 0) {
                                    
                                    _events.emplace_back(std::make_shared<ControlEvent>(event));
                                    throw "";
                                    
                                }
                                
                                _events.emplace_back(std::make_shared<DataEvent>(event));
                                throw;
                                
                            }).handle<ControlEvent>([&](const ControlEvent &event) {
                                
                                _state = State::Ready;
                                
                            });
                            
                            break;
                            
                        case State::Ready:
                            
                            Dispatch(*event).handle<OutboundDataEvent>([&](const OutboundDataEvent &event) {
                            
                                Frame(event.address).send(_socket, Frame::block::none, Frame::more::more);
                                Frame(event.data).send(_socket, Frame::block::none, Frame::more::none);
                            });
                            
                            break;
                            
                        default:
                            
                            break;
                            
                    }
                    
                } catch (...) {
                    
                }
                
            }

        }
        
    private:
        
        Socket _socket;
        State _state;
        std::deque<std::shared_ptr<Event>> _events;
        
    };
    
    class StreamServer {
        
    public:
        
        StreamServer(const Socket &socket)
        :
            _socket(socket)
        {}
        
        void send(const Frame &address, const Frame &data) {
        
            auto found = _sessions.find(address);
    
            if (found == _sessions.end()) {
                
                errno = ENOTSOCK;
                throw Exception("invalid address");
                
            }
            
            auto &session = found->second;
    
            session.dispatch(std::make_shared<OutboundDataEvent>(address, data));
            
        }
        
        void run() {

            std::shared_ptr<Event> event;
            
            do {
                
                Frame frame;
                
                try {
                
                    frame.receive(_socket, Frame::block::none);
                    
                } catch (Exception &e) {
                    
                    if (e.errorCode() == EAGAIN) {
                        return;
                    }
                    throw;
                }
                
                event = _eventFactory.decode(frame);
                
            } while ( ! event );
            
            auto found = _sessions.find(event->address);
            
            if (found == _sessions.end()) {
                
                found = _sessions.emplace(event->address, ServerSession(_socket)).first;
                
            }
            
            auto &session = found->second;
            
            session.dispatch(event);
            
        }
        
    private:
        
        Socket _socket;
        std::map<Frame, ServerSession> _sessions;
        EventFactory _eventFactory;
        
    };
    
    /*
     
     stream-protocol:   open *ready close
     
     open:              connect
     
     connect:           address empty
     
     ready:             c:data s:data
     
     data:              address non-empty
     
     close:             address empty
     
     */
    
} } }

#endif /* defined(__Messaging__StreamServer__) */
