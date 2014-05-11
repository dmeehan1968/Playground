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

namespace SessionPacketProtocol {

    class PacketEvent : public Messaging::Event {
        
    };
    
    class Codec : public Messaging::AbstractCodec {
        
    public:
        
        void reset() {}
        
        std::shared_ptr<Messaging::Event> decode(const Messaging::Frame &frame) {
            return nullptr;
        }
    };
    
}

namespace StreamServer {

    class Dispatch {
        
    public:
        
        Dispatch(const Messaging::Event &event)
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
        
        const Messaging::Event &_event;
        bool _handled;
        
    };
    
    class Client {
        
    public:
        
        enum class State {
            
            Open,
            Ready,
            Close
            
        };
        
        class ControlEvent : public StreamProtocol::RawEvent {
        
        public:
            
            ControlEvent(const Messaging::Frame &replyTo, const Messaging::Frame &data)
            :
                RawEvent(Event(replyTo), data)
            {}
            
        };
        
        class DataEvent : public StreamProtocol::RawEvent {
            
        public:
            
            DataEvent(const Messaging::Frame &replyTo, const Messaging::Frame &data)
            :
                RawEvent(Event(replyTo), data)
            {}
            
        };
        
        Client(const Messaging::Socket &frontend, const Messaging::Socket &backend)
        :
            _state(State::Open),
            _frontend(frontend),
            _backend(backend)
        {}
        
        State dispatch(const Messaging::Event &event) {
            
            Messaging::Message reply;
            
            switch (_state) {
                    
                case State::Open:
                    
                    Dispatch(event).handle<StreamProtocol::RawEvent>([&](const StreamProtocol::RawEvent &event) {
                        
                        detectMessageType(event);
                        
                    }).handle<ControlEvent>([&](const ControlEvent &event) {
                        
                        terminate(event);
                        
                    });
                    
                    break;
                    
                case State::Ready:
                    
                    Dispatch(event).handle<StreamProtocol::RawEvent>([&](const StreamProtocol::RawEvent &event) {
                        
                        detectMessageType(event);
                        
                    }).handle<ControlEvent>([&](const ControlEvent &event) {
                        
                        _state = State::Close;
                        
                    }).handle<DataEvent>([&](const DataEvent &event) {
                        
                        convertBytes(event);
                        
                    }).handle<SessionPacketProtocol::PacketEvent>([&](const SessionPacketProtocol::PacketEvent &event) {
                        
                        convertPacket(event);
                        
                    });
                    
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
        
        void detectMessageType(const StreamProtocol::RawEvent &event) {
            
            if (event.data.size() == 0) {
                
                throw ControlEvent(event.replyTo, event.data);
                
            }
            
            throw DataEvent(event.replyTo, event.data);
        }
        
        void terminate(const Messaging::Event &event) {
            
        }
        
        void convertBytes(const Messaging::Event &event) {
            
        }
        
        void convertPacket(const Messaging::Event &event) {
            
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
            
            _reactor.addObserver(_backend, Messaging::Reactor::Event::Readable, std::bind(&Server::onBackendReadable, this, _1, _2));
            
            _reactor.run(100);
            
        }
        
        void stop() {
            _reactor.stop();
        }
        
    protected:
        
        void onFrontendReadable(Messaging::Socket &socket, const Messaging::Reactor::Event &event) {
            
            onSocketReadable(socket, event, _frontendCodec);
            
        }
        
        void onBackendReadable(Messaging::Socket &socket, const Messaging::Reactor::Event &event) {
            
            onSocketReadable(socket, event, _backendCodec);
            
        }
        
        void onSocketReadable(Messaging::Socket &socket, const Messaging::Reactor::Event &event, Messaging::AbstractCodec &codec) {
            
            bool more = false;
            std::shared_ptr<Messaging::Event> eventPtr;
            
            do {
                
                Messaging::Frame frame;
                frame.receive(socket, Messaging::Frame::block::none);
                
                more = frame.hasMore();
                
                eventPtr = codec.decode(frame);
                
            } while (more);
            
            if (eventPtr) {

                dispatch(*eventPtr);
                
            } else {
                
                // WTF
            }
            
        }
        
        void dispatch(const Messaging::Event &event) {
            
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
        SessionPacketProtocol::Codec _backendCodec;
        
    };
    
}


#endif /* defined(__Messaging__StreamServer__) */
