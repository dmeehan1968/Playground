//
//  StreamAdapter.h
//  Messaging
//
//  Created by Dave Meehan on 30/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__StreamAdapter__
#define __Messaging__StreamAdapter__

#include "Frame.h"
#include "Reactor.h"

#include <stdexcept>
#include <vector>

namespace Messaging { namespace Examples {
   
    class Event {
        
    public:
        virtual ~Event() {}
        
    };
    
    class TimeoutEvent : public Event {};
    class MalformedEvent : public Event {};

    namespace StreamAdapter {
        
        class StreamEvent : public Event {

        public:
            
            using ClientID = std::vector<char>;
            
            template <class C>
            StreamEvent(C &&clientId)
            :
                _clientId(std::forward<C>(clientId))
            {}
            
            const ClientID &clientId() const {
                return _clientId;
            }
            
        private:
            
            ClientID _clientId;
            
        };

        class ConnectionEvent : public StreamEvent {
            
        public:
            
            template <class C>
            ConnectionEvent(C &&clientId)
            :
                StreamEvent(std::forward<C>(clientId))
            {}
            
        };
        
        class DataEvent : public StreamEvent {
          
        public:
            
            using Data = std::vector<char>;
            
            template <class C, class D>
            DataEvent(C &&clientId, D &&data)
            :
                StreamEvent(std::forward<C>(clientId)),
                _data(data)
            {}
            
            const Data &data() const {
                return _data;
            }
            
        private:
            
            Data _data;
            
        };
        
        class Protocol {
        
        public:

            enum class State {

                Init,
                ClientID,
                Data
                
            };
            
            Protocol() : _state(State::Init) {}
            
            std::shared_ptr<Event> parse(const char *data, size_t size, bool more) {
                
                const char *needle = data;
                const char *end = needle + size;
                
                do {
                    
                    switch (_state) {
                            
                        case State::Init:
                            
                            _clientId = {};
                            
                            _state = State::ClientID;
                            
                            break;
                            
                        case State::ClientID:
                            
                            if (size == 0) {
                                return std::make_shared<MalformedEvent>();
                            }
                            
                            if (more == 0) {
                                return std::make_shared<MalformedEvent>();
                            }
                            
                            _clientId = StreamEvent::ClientID(needle, end);
                            
                            _state = State::Data;
                            
                            return nullptr;
                            
                        case State::Data:
                            
                            if (more) {
                                return std::make_shared<MalformedEvent>();
                            }
                            
                            if (size == 0) {
                                
                                return std::make_shared<ConnectionEvent>(std::move(_clientId));
                                
                            }

                            _state = State::Init;
                            
                            return std::make_shared<DataEvent>(std::move(_clientId), DataEvent::Data(needle, end));

                        default:
                            
                            throw std::runtime_error("Invalid state");
                            
                    }
                    
                } while (1);
                
                return nullptr;
                
            }
            
        private:
            
            State _state;
            StreamEvent::ClientID _clientId;
            
        };
        
        class Dispatcher {
            
        public:
            
            Dispatcher(Event &event) : _event(event), _handled(false) {}
            
            template <class T>
            Dispatcher &handle(const std::function<void(T&)> &functor) {
            
                if (functor && ! _handled) {
                    try {
                 
                        functor(dynamic_cast<T&>(_event));
                        _handled = true;
                        
                    } catch (std::bad_cast &) {
                        // ignore
                    }
                }
                
                return *this;
                
            }
            
        private:
            bool _handled;
            Event &_event;
            
        };
        
        class Device {
            
        public:
            
            Device(Socket &stream, Socket &output)
            :
                _done(false),
                _stream(stream),
                _output(output)
            {}
            
            void operator()() {
                
                while ( ! _done ) {
                    
                }
            }

            void dispatch(Event &event) {
                
                Dispatcher(event).handle<Event>([&](const Event &event) {
                    
                });
            }
        private:
            
            bool        _done;
            Socket      _stream;
            Socket      _output;
            
        };
        
    }
    
} }

#endif /* defined(__Messaging__StreamAdapter__) */
