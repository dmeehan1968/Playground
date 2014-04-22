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

#include <vector>
#include <ostream>

namespace Messaging {
    
    class Poller {
        
    public:
        
        Poller() {}
        Poller(const Poller &) = delete;
        Poller & operator= (const Poller &) = delete;
        
        size_t socketCount() const {
            
            return _sockets.size();
            
        }
        
        enum class event {
            none,
            readable,
            writable,
            error,
            all
        };
        
        void observe(Socket &socket, const std::initializer_list<event> &events = { event::none } ) {
            
            auto event_mask = eventMaskFromEnums(events);
        
            auto index = 0;
            for (auto &s : _sockets) {
                
                if (s == socket) {
                    
                    if (event_mask == 0) {
                    
                        _items.erase(_items.begin()+index);
                        _sockets.erase(_sockets.begin()+index);
                    
                    } else {
                        
                        _items[index].events = event_mask;
                        _items[index].revents = 0;
                        
                    }
                    
                    return;
                    
                }
                
                index++;
                
            }
            
            if (event_mask == 0) {
                throw Exception("no events specified", 0);
            }
            
            zmq_pollitem_t poll = { socket, 0, event_mask, 0 };
            
            _items.push_back(poll);
            _sockets.push_back(socket);
            
            
        }
        
        struct event_flags {
            bool readable;
            bool writable;
            bool error;
        };
        
        event_flags events(const Socket &socket) const {
        
            unsigned index;
            decltype(_sockets.begin()) iter;
            
            for (   index = 0, iter = _sockets.begin();
                    iter != _sockets.end() ;
                    iter++, index++) {
                
                if (*iter == socket) {
                    
                    return eventFlagsFromMask(_items[index].revents);
                    
                }
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
        
        void dispatch(const std::function<void(Socket &socket, const event_flags &events)> &functor) {
         
            auto index = 0;
            
            for (auto &socket : _sockets) {
            
                auto event_mask = _items[index].revents;

                if (event_mask != 0) {
                 
                    functor(socket, eventFlagsFromMask(event_mask));
                    
                }
                index++;
            }
        }
        
    protected:
        
        short eventMaskFromEnums(const std::vector<event> &events) const {
            
            short event_mask = 0;
            
            for (auto &event : events) {
                switch (event) {
                    case event::readable:
                        
                        event_mask |= ZMQ_POLLIN;
                        break;
                        
                    case event::writable:
                        
                        event_mask |= ZMQ_POLLOUT;
                        break;
                        
                    case event::error:
                        
                        event_mask |= ZMQ_POLLERR;
                        break;
                        
                    case event::all:
                        
                        event_mask |= ZMQ_POLLIN | ZMQ_POLLOUT | ZMQ_POLLERR;
                        break;
                        
                    case event::none:
                        
                        event_mask = 0;
                        break;
                        
                    default:
                        
                        throw Exception("Unrecognised event type", 0);
                        
                }
            }
            
            return event_mask;
            
        }

        event_flags eventFlagsFromMask(const short event_mask) const {
            
            event_flags flags;
            
            flags.readable = event_mask & ZMQ_POLLIN ? true : false;
            flags.writable = event_mask & ZMQ_POLLOUT ? true : false;
            flags.error = event_mask & ZMQ_POLLERR ? true : false;
            
            return flags;
            
        }
        
    private:
        
        std::vector<zmq_pollitem_t> _items;
        std::vector<Socket> _sockets;
        
    };
    
    inline std::ostream &operator<< (std::ostream &stream, const Poller::event &event) {
        
        if (event == Poller::event::readable) {
            stream << "Readable";
        } else if (event == Poller::event::writable) {
            stream << "Writable";
        } else if (event == Poller::event::error) {
            stream << "Error";
        } else {
            stream << "Unrecognised";
        }
        return stream;
        
    }
}

#endif /* defined(__Messaging__Poller__) */
