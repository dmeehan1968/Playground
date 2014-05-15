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
#include <thread>

namespace Messaging {
    
    class Reactor {

    public:
        
        using Event = Poller::Event;
        using Observer = std::function<void(const Socket &, const Event &event, const bool didTimeout)>;

        using Milliseconds = std::chrono::milliseconds;
        using TimePoint = std::chrono::steady_clock::time_point;

        class EventObservation {

        public:

            EventObservation(const Observer &observer, const Milliseconds &duration)
            :
                _observer(observer),
                _duration(duration)
            {
                resetTimeout();
            }

            const Observer &observer() const {
                return _observer;
            }

            void setObserver(const Observer &observer) {
                _observer = observer;
            }

            const Milliseconds &duration() const {
                return _duration;
            }

            void setDuration(const Milliseconds &duration) {
                _duration = duration;
            }

            bool isExpired() const {

                auto now = TimePoint::clock::now();

                return _expires < now;

            }

            void resetTimeout() {

                auto now = TimePoint::clock::now();

                auto limit = std::chrono::duration_cast<Milliseconds>(TimePoint::duration::max() - now.time_since_epoch());

                _expires = now + ((limit < _duration) ? limit : _duration);

            }

        private:

            Observer _observer;
            std::chrono::milliseconds _duration;
            std::chrono::high_resolution_clock::time_point _expires;

        };

        Reactor()
        :
            _done(false),
            _poller(new Poller)
        {}
        
        Reactor(const std::shared_ptr<Poller> &poller)
        :
            _done(false),
            _poller(poller)
        {}
        
        void addObserver(const Socket &socket,
                         const Event &event,
                         const Observer &observer,
                         const Milliseconds timeout = Milliseconds::max()) {
            
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
                    _readObservers.emplace(socket, EventObservation(observer, timeout));
                    break;
                    
                case Event::Writable:
                    _writeObservers.emplace(socket, EventObservation(observer, timeout));
                    break;
                    
                case Event::Error:
                    _errorObservers.emplace(socket, EventObservation(observer, timeout));
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
        
        bool runOnce(const long timeout) {

            bool didNotify = false;

            if (_poller->poll(timeout) > 0) {
                
                _poller->dispatch([&](Socket &socket, const Poller::Events &events) {
                
                    if (events.is(Poller::Event::Readable)) {
                        
                        notify(_readObservers, socket, Poller::Event::Readable, false);
                        didNotify = true;
                        
                    }
                    
                    if (events.is(Poller::Event::Writable)) {
                        
                        notify(_writeObservers, socket, Poller::Event::Writable, false);
                        didNotify = true;

                    }
                    
                    if (events.is(Poller::Event::Error)) {
                        
                        notify(_errorObservers, socket, Poller::Event::Error, false);
                        didNotify = true;
                        
                    }

                });
            
            }

            didNotify |= notifyTimeouts(_readObservers, Event::Readable);
            didNotify |= notifyTimeouts(_writeObservers, Event::Writable);
            didNotify |= notifyTimeouts(_errorObservers, Event::Error);

            return didNotify;
            
        }
        
        void run(const long interval) {
            
            while (! _done) {
                runOnce(interval);
            }
            
        }
        
        void operator()() {
            
            run(100);
            
        }
        
        void start() {

            _thread = std::thread(std::ref(*this));
            
        }
        
        void stop() {
            
            _done = true;
            
            if (_thread.joinable()) {
                
                _thread.join();
                
            }
            
        }
        
    protected:
        
        void notify(std::map<Socket, EventObservation> &observers, Socket &socket, const Event &event, const bool didTimeout) {
            
            auto found = observers.find(socket);
            
            if (found != observers.end()) {
                
                (found->second.observer())(socket, event, didTimeout);
                found->second.resetTimeout();
            }
            
        }

        bool notifyTimeouts(std::map<Socket, EventObservation> &observers, const Event &event) {

            bool didNotify = false;

            for ( auto &observation : observers ) {

                if (observation.second.isExpired()) {

                    (observation.second.observer())(observation.first, event, true);
                    didNotify = true;

                }
            }

            return didNotify;

        }
        
    private:

        bool _done;
        std::shared_ptr<Poller> _poller;
        std::map<Socket, EventObservation> _readObservers;
        std::map<Socket, EventObservation> _writeObservers;
        std::map<Socket, EventObservation> _errorObservers;
        
        std::thread _thread;
        
    };
    
}

#endif /* defined(__Messaging__Reactor__) */
