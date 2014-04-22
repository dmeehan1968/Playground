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
        
        class Event {

        public:
            
            Event() : _readable(false), _writable(false), _error(false) {}
            
            Event(const short event_mask)
            :
                _readable(event_mask & ZMQ_POLLIN),
                _writable(event_mask & ZMQ_POLLOUT),
                _error(event_mask & ZMQ_POLLERR)
            {}
            
            Event &none() {
                return *this;
            }
            
            Event &read() {
                _readable = true;
                return *this;
            }
            
            bool isReadable() const {
                return _readable;
            }
            
            Event &write() {
                _writable = true;
                return *this;
            }
            
            bool isWritable() const {
                return _writable;
            }
            
            Event &error() {
                _error = true;
                return *this;
            }
            
            bool isError() const {
                return _error;
            }
            
            bool isClear() const {
                return !_readable && !_writable && !_error;
            }
            
            short eventMask() const {
                short mask = 0;
                mask |= _readable ? ZMQ_POLLIN : 0;
                mask |= _writable ? ZMQ_POLLOUT : 0;
                mask |= _error ? ZMQ_POLLERR : 0;
                return mask;
            }
            
        private:
            bool _readable;
            bool _writable;
            bool _error;
        };
        
        size_t socketCount() const {
            
            return _sockets.size();
            
        }
        
        void observe(Socket &socket, const Event &events = Event().read() ) {
            
            auto index = getSocketIndex(socket);

            if (index == -1) {
                
                if (events.isClear()) {
                    
                    throw Exception("no events specified", 0);
                    
                }
                
                zmq_pollitem_t poll = { socket.get(), 0, events.eventMask(), 0 };
                
                _items.push_back(poll);
                _sockets.push_back(socket);

                return;
                
            }

            if (events.isClear()) {
                
                _sockets.erase(_sockets.begin()+index);
                _items.erase(_items.begin()+index);
                
            } else {
                
                _items[index].events = events.eventMask();
                _items[index].revents = 0;
                
            }
            
        }
        
        struct event_flags {
            bool readable;
            bool writable;
            bool error;
        };
        
        Event operator()(Socket &socket) const {
        
            auto index = getSocketIndex(socket);
            
            if (index >= 0) {
                return Event(_items[index].revents);
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
        
        void dispatch(const std::function<void(Socket &socket, const Event &events)> &functor) {
         
            auto index = 0;
            
            for (auto &socket : _sockets) {
            
                auto events = Event(_items[index].revents);
                
                if ( ! events.isClear()) {
                 
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
    
    inline std::ostream &operator<< (std::ostream &stream, const Poller::Event &events) {
        
        if (events.isReadable()) {
            stream << "Read";
        }
        if (events.isWritable()) {
            stream << "Write";
        }
        if (events.isError()) {
            stream << "Error";
        }
        return stream;
        
    }
}

#endif /* defined(__Messaging__Poller__) */
