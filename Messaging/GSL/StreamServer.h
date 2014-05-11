//
//  StreamServer.h
//  Messaging
//
//  Created by Dave Meehan on 11/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__StreamServer__
#define __Messaging__StreamServer__

#include "Message.h"
#include "Reactor.h"
#include "StreamProtocol.h"

namespace StreamServer {

    class Dispatch {
        
    public:
        
        Dispatch(const StreamProtocol::Event &event)
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
        
        const StreamProtocol::Event &_event;
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
        
        State dispatch(const StreamProtocol::Event &event) {
            
            Messaging::Message reply;
            
            switch (_state) {
                    
                case State::Open:
                    
                    Dispatch(event).handle<StreamProtocol::DataEvent>([&](const StreamProtocol::DataEvent &event) {
                        
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
        
        void isEmptyAction(const StreamProtocol::DataEvent &event) {
            
            if (event.data.size() > 0) {
                
                _state = State::Open;
                dispatch(StreamProtocol::TerminateEvent(event));
                
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
        
        void dispatch(const StreamProtocol::Event &event) {
            
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
        StreamProtocol::Codec _frontendCodec;
        std::map<Messaging::Frame, Client> _clients;
        
    };
    
}


#endif /* defined(__Messaging__StreamServer__) */
