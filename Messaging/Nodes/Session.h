//
//  Session.h
//  Messaging
//
//  Created by Dave Meehan on 12/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__Session__
#define __Messaging__Session__

#include "Socket.h"
#include "Frame.h"

#include <iostream>
#include <ostream>
#include <map>

/*

 session-manager:       ready
 
 ready:                 *(get / set)
 
 get:                   c:get-request (s:get-reply / s:error)
 
 set:                   c:set-request (s:ok / s:error)
 
 get-request:           msg-id      ; must be "GET"
                        session-id
 
 get-reply:             msg-id      ; must be "SESSION"
                        session
 
 set-request:           msg-id      ; must be "SET"
                        session-id
                        session
 
 ok:                    FRAME       ; must be "OK"
 
 error:                 FRAME       ; must be "ERROR"
 
 session:               data
 
 cmd:                   FRAME

 msg-id:                FRAME
 
 session-id:            FRAME

 data:                  FRAME
 
 
 <state name="ready">
 
    <event name="get">
 
        <action name="send session" />
 
    </event>
 
    <event name="set">
 
        <action name="save session />
 
    </event>
 
    <header>
    
        <field name="id" type="string" />
 
    </header>
 
    <message name="get" id="GET">
 
        <field name="session id" type="string" />
 
    </message>
 
    <message name="set" id="SET">
 
        <field name="session id" type="string />
        <field name="data" type="string" />
 
    </message>
 
    <message name="session" id="SESSION">
 
        <field name="data" type="string" />
 
    </message>
 
    <message name="ok" id="OK" />
 
    <message name="error" id="ERROR" />
 
 <state>
 
 */

namespace Messaging { namespace SessionManager {
    
    class Session {
      
    public:
        
        bool operator != (const Session &other) const {
            return _data != other._data;
        }
        
        std::string data() const {
            return _data;
        }
        
        void setData(const std::string &data) {
            _data = data;
        }
        
    private:
        
        std::string _data;
        
    };
    
    inline std::ostream &operator << (std::ostream &stream, const Session &state) {
        return stream << "Session: data=" << state.data();
    }
    
    using SessionId = std::string;

    class SessionStore {
        
    public:
        
        Session &get(const SessionId &id) {
            
            return _sessions[id];
            
        }
        
        void set(const SessionId &id, const Session &session) {
            
            _sessions[id] = session;
            
        }
        
    private:
        
        std::map<SessionId, Session> _sessions;
        
    };
    
    class Client {
        
    public:
        
        Client(const Socket &socket)
        :
            _socket(socket)
        {}
        
        std::shared_ptr<Session> get(const SessionId &id) {

            Frame("GET").send(_socket, Frame::block::blocking, Frame::more::more);
            Frame(id).send(_socket, Frame::block::blocking, Frame::more::none);
            
            Frame msgId;
            msgId.receive(_socket, Frame::block::blocking);
            
            if (msgId.str() != "SESSION") {
                return nullptr;
            }
            
            Frame data;
            data.receive(_socket, Frame::block::blocking);
            
            auto state = std::make_shared<Session>();
            state->setData(data.str());
            
            return state;
            
        }
        
        void set(const SessionId &id, const Session &session) {
            
            Frame("SET").send(_socket, Frame::block::blocking, Frame::more::more);
            Frame(id).send(_socket, Frame::block::blocking, Frame::more::more);
            Frame(session.data()).send(_socket, Frame::block::blocking, Frame::more::none);
            
            Frame reply;
            
            reply.receive(_socket, Frame::block::blocking);
            
            if (reply.str() != "OK") {
                errno = EINVAL;
                throw Exception("invalid response");
            }
            
        }
        
    private:
        
        Socket _socket;
        
    };
    
    class Event {
    
    public:
        
        Event(const Frame &address)
        :
            address(address)
        {}
        
        virtual ~Event() {}
        
        Frame address;
        
    };
    
    class GetEvent : public Event {
        
    public:
        
        GetEvent(const Frame &address)
        :
            Event(address)
        {}
        
        Frame sessionId;
        
    };
    
    class SetEvent : public Event {
        
    public:
        
        SetEvent(const Frame &address)
        :
            Event(address)
        {}
        
        Frame sessionId;
        Frame data;
        
    };
    
    class SessionEvent : public Event {
        
    public:
        
        SessionEvent(const Frame &address)
        :
            Event(address)
        {}
        
        Frame data;
        
    };
    
    class OkEvent : public Event {
    
    public:
        
        OkEvent(const Frame &address)
        :
            Event(address)
        {}
        
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
    
    class EventFactory {
    
    public:
        
        enum class State {
            
            Address,
            Delimiter,
            Id,
            GetSessionId,
            SetSessionId,
            SetSessionData,
            SessionData,
            
            FinalState
            
        };
        
        EventFactory(const bool useAddress)
        :
            _useAddress(useAddress)
        {
            reset();
        }
        
        void reset() {
            _state = _useAddress ? State::Address : State::Id;
        }
        
        std::shared_ptr<Event> decode(const Frame &frame) {
            
            switch (_state) {
                    
                case State::Address:
                {
                    _address = frame;
                    _state = State::Delimiter;
                }
                    break;
                    
                case State::Delimiter:
                {
                    _state = State::Id;
                }
                    break;
                    
                case State::Id:
                {
                    auto id = frame.str();
                    
                    if (id == "GET") {
                        
                        _event = std::make_shared<GetEvent>(_address);
                        _state = State::GetSessionId;
                        
                    } else if (id == "SET") {
                        
                        _event = std::make_shared<SetEvent>(_address);
                        _state = State::SetSessionId;
                        
                    }
                }
                    break;
                    
                case State::GetSessionId:
                {
                    auto getEvent = std::dynamic_pointer_cast<GetEvent>(_event);
                    getEvent->sessionId = frame;
                    _state = State::FinalState;
                }
                    break;
                    
                case State::SetSessionId:
                {
                    auto setEvent = std::dynamic_pointer_cast<SetEvent>(_event);
                    setEvent->sessionId = frame;
                    _state = State::SetSessionData;
                }
                    break;
                    
                case State::SetSessionData:
                {
                    auto setEvent = std::dynamic_pointer_cast<SetEvent>(_event);
                    setEvent->data = frame;
                    _state = State::FinalState;
                }
                default:
                    break;
            }
            
            if (_state == State::FinalState) {
                
                reset();
                return _event;
            }
            
            return nullptr;
            
        }
        
        void encode(const std::shared_ptr<Event> &event, Socket &socket) {

            while (_state != State::FinalState) {

                switch (_state) {
                        
                    case State::Address:
                        
                        event->address.send(socket, Frame::block::blocking, Frame::more::more);
                        
                        _state = State::Delimiter;
                        break;
                        
                    case State::Delimiter:
                        
                        Frame().send(socket, Frame::block::blocking, Frame::more::more);
                        _state = State::Id;
                        break;
                        
                    case State::Id:

                        Dispatch(*event).handle<SessionEvent>([&](const SessionEvent &event) {
                           
                            Frame("SESSION").send(socket, Frame::block::blocking, Frame::more::more);
                            
                            _state = State::SessionData;
                            
                        }).handle<OkEvent>([&](const OkEvent &event) {
                            
                            Frame("OK").send(socket, Frame::block::blocking, Frame::more::none);
                            
                            _state = State::FinalState;
                            
                        });
                        
                        break;
                        
                    case State::SessionData:
                    
                        Dispatch(*event).handle<SessionEvent>([&](const SessionEvent &event) {
                        
                            Frame(event.data).send(socket, Frame::block::blocking, Frame::more::none);
                            
                            _state = State::FinalState;
                            
                        });
                        break;
                        
                    default:
                        break;
                }
                
            }
            
            reset();
        }
        
    private:
        
        State _state;
        Frame _address;
        std::shared_ptr<Event> _event;
        bool _useAddress;
        
    };
    
    class Peer {
        
    public:
        
        enum class State {
        
            Ready
            
        };
        
        Peer(const Socket &socket, const std::shared_ptr<SessionStore> &sessions)
        :
            _state(State::Ready),
            _socket(socket),
            _sessions(sessions),
            _eventFactory(_socket.getSocketType() == Socket::Type::router)
        {}
        
        void dispatch(const std::shared_ptr<Event> &event) {
          
            std::shared_ptr<Event> reply;
            
            switch (_state) {
                    
                case State::Ready:
                    
                    Dispatch(*event).handle<GetEvent>([&](const GetEvent &event) {
                    
                        auto session = _sessions->get(event.sessionId.str());
                        
                        auto sessionEvent = std::make_shared<SessionEvent>(event.address);
                        
                        sessionEvent->data = Frame(session.data());
                        
                        reply = sessionEvent;
                        
                    }).handle<SetEvent>([&](const SetEvent &event) {
                        
                        auto &session = _sessions->get(event.sessionId.str());
                        
                        session.setData(event.data.str());
                        
                        reply = std::make_shared<OkEvent>(event.address);
                        
                    });
                    
            }

            if (reply) {

                _eventFactory.encode(reply, _socket);
                
            }
            
        }
        
    private:
        
        State _state;
        Socket _socket;
        std::shared_ptr<SessionStore> _sessions;
        EventFactory _eventFactory;
        
    };
    
    class Server {
        
    public:
        
        Server(const Socket &socket)
        :
            _socket(socket),
            _done(false),
            _useAddress(_socket.getSocketType() == Socket::Type::router),
            _eventFactory(_useAddress)
        {
            _sessions = std::make_shared<SessionStore>();
        }
        
        enum class State {
            
            Ready
            
        };
        
        void operator()() {
            
            while (!_done) {
                
                Frame frame;
                
                try {
                    
                    frame.receive(_socket, Frame::block::blocking);
                    
                } catch (Exception &e) {
                    
                    if (e.errorCode() == EAGAIN) {
                        continue;
                    }
                    throw;
                }
                
                dispatch(frame);
                
            }
            
        }
        
        void dispatch(const Frame &frame) {
            
            auto event = _eventFactory.decode(frame);
            
            if (event) {

                auto found = _peers.find(event->address);
                
                if (found == _peers.end()) {
                    
                    found = _peers.emplace(event->address, Peer(_socket, _sessions)).first;
                    
                }
                
                auto peer = found->second;
                
                peer.dispatch(event);
                
            }
        }
        
        void stop() {
            _done = true;
        }
        
    private:
        
        Socket          _socket;
        bool            _done;
        State           _state;
        bool            _useAddress;
        EventFactory    _eventFactory;
        std::map<Frame, Peer> _peers;
        
        std::shared_ptr<SessionStore> _sessions;
        
    };
    
    
} }


#endif /* defined(__Messaging__Session__) */
