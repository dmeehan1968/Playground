//
//  Poller.h
//  Messaging
//
//  Created by Dave Meehan on 21/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__Poller__
#define __Messaging__Poller__

#include "Socket.h"

#include <set>
#include <vector>
#include <ostream>

namespace Messaging {

    class Poller {
        
    public:
        
        Poller() {}
        Poller(const Poller &) = delete;
        Poller & operator= (const Poller &) = delete;
        
        enum class Event {
            Readable,
            Writable,
            Error
        };
        
        class Events : public std::set<Event> {
            
        public:
            
            using container = std::set<Event>;
            
            Events(const std::initializer_list<Event> &events)
            :
                std::set<Event>(events)
            {}
            
            bool is(const Event &event) const {
                return container::find(event) != container::end();
            }
            
        protected:
            
            friend class Poller;
            
            Events(const short bitmask) {
                if (bitmask & ZMQ_POLLIN) {
                    container::emplace(Event::Readable);
                }
                if (bitmask & ZMQ_POLLOUT) {
                    container::emplace(Event::Writable);
                }
                if (bitmask & ZMQ_POLLERR) {
                    container::emplace(Event::Error);
                }
            }
            
            short bitmask() const {
                
                short mask = 0;
                
                for (auto &event : *this) {
                    switch (event) {
                        case Event::Readable:
                            mask |= ZMQ_POLLIN;
                            break;
                        case Event::Writable:
                            mask |= ZMQ_POLLOUT;
                            break;
                        case Event::Error:
                            mask |= ZMQ_POLLERR;
                            break;
                    }
                }
                
                return mask;
            }
        };
        
        size_t socketCount() const {
            
            return _sockets.size();
            
        }
        
        void observe(Socket &socket, const Events &events = { Event::Readable }) {
            
            auto index = getSocketIndex(socket);

            if (index == -1) {
                
                if ( events.empty() ) {
                    
                    throw Exception("no events specified", 0);
                    
                }
                
                zmq_pollitem_t poll = { socket.get(), 0, events.bitmask(), 0 };
                
                _items.push_back(poll);
                _sockets.push_back(socket);

                return;
                
            }

            if ( events.empty() ) {
                
                _sockets.erase(_sockets.begin()+index);
                _items.erase(_items.begin()+index);
                
            } else {
                
                _items[index].events = events.bitmask();
                _items[index].revents = 0;
                
            }
            
        }
        
        Events operator()(Socket &socket) const {
        
            auto index = getSocketIndex(socket);
            
            if (index >= 0) {
                return Events(_items[index].revents);
            }
            
            throw Exception("socket not found", 0);
            
        }
        
        int poll(const long timeout) {
            
            auto rc = zmq_poll(_items.data(), (int)_items.size(), timeout);
            
            if (rc < 0) {
                throw Exception("Poll failed");
            }
            
            return rc;
            
        }
        
        void dispatch(const std::function<void(Socket &socket, const Events &events)> &functor) {
         
            auto index = 0;
            
            auto socketsCopy = _sockets;
            
            for (auto &socket : socketsCopy) {
            
                auto events = Events(_items[index].revents);
                
                if ( ! events.empty() ) {
                 
                    functor(socket, events);
                    
                }
                index++;
            }
        }
        
    protected:
        
        long getSocketIndex(Socket &socket) const {
            
            auto found = std::find(_sockets.begin(), _sockets.end(), socket);
            
            if (found == _sockets.end()) {
                return -1;
            }
            
            return found - _sockets.begin();
        }
        
    private:
        
        std::vector<zmq_pollitem_t> _items;
        std::vector<Socket> _sockets;
        
    };
    
    inline std::ostream &operator<< (std::ostream &stream, const Poller::Event &event) {
        
        switch (event) {
            case Poller::Event::Readable:
                stream << "Readable";
                break;
                
            case Poller::Event::Writable:
                stream << "Writable";
                break;
                
            case Poller::Event::Error:
                stream << "Error";
                break;
        }
        return stream;
    }
    
    inline std::ostream &operator<< (std::ostream &stream, const Poller::Events &events) {
        
        for (auto &event : events) {
            stream << event;
        }

        return stream;
        
    }
    
}

#endif /* defined(__Messaging__Poller__) */
