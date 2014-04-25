//
//  Reactor.h
//  Messaging
//
//  Created by Dave Meehan on 25/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__Reactor__
#define __Messaging__Reactor__

#include "Poller.h"

#include <map>

namespace Messaging {
    
    class Reactor {

    public:
        
        using Event = Poller::Event;
        using EventObserver = std::function<void(Socket &, const Event &)>;
        
        Reactor()
        :
            _poller(new Poller)
        {}
        
        Reactor(const std::shared_ptr<Poller> &poller)
        :
            _poller(poller)
        {}
        
        void addObserver(const Socket &socket, const Event &event, const EventObserver &observer) {
            
            if ( ! observer ) {
                throw Exception("observer cannot be null", 0);
            }
            
            auto events = _poller->observedEvents(socket);
            
            if (events.is(event)) {
                
                throw Exception("event already observed", 0);
                
            }
            
            events.insert(event);
            
            _poller->observe(socket, events);
            
            switch (event) {
                case Event::Readable:
                    _readObservers[socket] = observer;
                    break;
                    
                case Event::Writable:
                    _writeObservers[socket] = observer;
                    break;
                    
                case Event::Error:
                    _errorObservers[socket] = observer;
                    break;
                    
            }
            
        }
        
        void removeObserver(const Socket &socket, const Event &event) {
            
            auto events = _poller->observedEvents(socket);
            
            if ( ! events.is(event)) {
                
                throw Exception("event is not observed", 0);
                
            }
            
            events.erase(event);
            
            _poller->observe(socket, events);
            
            switch (event) {
                case Event::Readable:
                    _readObservers.erase(socket);
                    break;

                case Event::Writable:
                    _writeObservers.erase(socket);
                    break;
                    
                case Event::Error:
                    _errorObservers.erase(socket);
                    break;
            }
            
        }
        
    private:
        
        std::shared_ptr<Poller> _poller;
        std::map<Socket, EventObserver> _readObservers;
        std::map<Socket, EventObserver> _writeObservers;
        std::map<Socket, EventObserver> _errorObservers;
        
    };
    
}

#endif /* defined(__Messaging__Reactor__) */
