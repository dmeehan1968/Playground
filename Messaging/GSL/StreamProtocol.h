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
                
            } catch (std::bad_cast &e) {
                
            }
            
            return *this;
        }
        
    private:
        
        const Event &_event;
        bool _handled;
        
    };
    
    class Client {
      
    public:
        
        enum class State {
            
            Open,
            Ready,
            Terminate,
            Close
            
        };
        
        Client(const Messaging::Socket &frontend, const Messaging::Socket &backend)
        :
            _state(State::Open),
            _frontend(frontend),
            _backend(backend)
        {}

        State dispatch(const Event &event) {
            
            Messaging::Message reply;
            
            switch (_state) {

                case State::Open:
                    
                    Dispatch(event).handle<DataEvent>([&](const DataEvent &event) {
                       
                        _state = State::Ready;
                        
                        isEmptyAction(event);
                        
                    });
                    
                    break;
                    
                case State::Ready:
                    
                    _state = State::Ready;
                    
                    
                    
                    break;
                    
                case State::Terminate:
                    
                    _state = State::Close;
                    
                    reply.emplace_back("");
                    break;
                    
                case State::Close:
                    
                    break;
                    
                default:
                    break;
            }
            
            if (reply.size() > 0) {
                
                reply.envelope().emplace_front(event.replyTo);
            
                reply.send(_frontend);
                
            }
            
            return _state;
            
        }
        
    protected:
        
        void isEmptyAction(const DataEvent &event) {
            
            if (event.data.size() > 0) {
                
                _state = State::Open;
                dispatch(TerminateEvent(event));
                
            }
        }
        
    private:
        
        State _state;
        Messaging::Socket _frontend;
        Messaging::Socket _backend;
        
    };
    
    class Server {
        
    public:

        Server(const Messaging::Socket &frontend, const Messaging::Socket &backend)
        :
            _frontend(frontend),
            _backend(backend)
        {}
        
        void operator()() {
            
            using namespace std::placeholders;
            
            _reactor.addObserver(_frontend, Messaging::Reactor::Event::Readable, std::bind(&Server::onFrontendReadable, this, _1, _2));
         
            _reactor.run(100);
            
        }
        
        void stop() {
            _reactor.stop();
        }
        
    protected:
        
        void onFrontendReadable(const Messaging::Socket &socket, const Messaging::Reactor::Event &event) {
            
            bool more = false;
            
            do {

                Messaging::Frame frame;
                frame.receive(_frontend, Messaging::Frame::block::none);
                
                more = frame.hasMore();
                
                auto event = _frontendCodec.decode(frame);
                
                if (event) {
                    dispatch(*event);
                }
                
            } while (more);
            
        }
        
        void dispatch(const Event &event) {

            auto found = _clients.find(event.replyTo);
            
            if (found == _clients.end()) {
                
                found = _clients.emplace(event.replyTo, Client(_frontend, _backend)).first;
                
            }
            
            if (found->second.dispatch(event) == Client::State::Close) {
                
                _clients.erase(found);
                
            }
            
        }
        
    private:
        
        Messaging::Reactor _reactor;
        Messaging::Socket _frontend;
        Messaging::Socket _backend;
        Codec _frontendCodec;
        std::map<Messaging::Frame, Client> _clients;
        
    };
    
    
}

#endif /* defined(__Messaging__StreamProtocol__) */
