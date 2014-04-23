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
        
        class Events {

        public:
            
            static const Events None;
            static const Events Readable;
            static const Events Writable;
            static const Events Error;
            static const Events All;
            
            operator bool() const {
                return _flags != 0;
            }

            std::string toString() const {
                
                std::string string;
                
                if (*this & Readable) {
                    string += "Read";
                }
                if (*this & Writable) {
                    string += "Write";
                }
                if (*this & Error) {
                    string += "Error";
                }
                return string;
            }
            
            Events operator| (const Events &other) const {

                return Events(_flags | other._flags);
                
            }
            
            Events operator& (const Events &other) const {
                
                return Events(_flags & other._flags);
                
            }
            
            bool isReadable() {
                return *this == Readable;
            }
            
            bool isWritable() {
                return *this == Writable;
            }
            
            bool isError() {
                return *this == Error;
            }
            
        protected:
            
            friend class Poller;

            constexpr Events(const short type) : _flags(type) {}
            
            short get() const {
                return _flags;
            }
            
        private:
            
            short _flags;
        };
        
        size_t socketCount() const {
            
            return _sockets.size();
            
        }
        
        void observe(Socket &socket, const Events &events = Events::Readable ) {
            
            auto index = getSocketIndex(socket);

            if (index == -1) {
                
                if ( ! events ) {
                    
                    throw Exception("no events specified", 0);
                    
                }
                
                zmq_pollitem_t poll = { socket.get(), 0, events.get(), 0 };
                
                _items.push_back(poll);
                _sockets.push_back(socket);

                return;
                
            }

            if ( ! events ) {
                
                _sockets.erase(_sockets.begin()+index);
                _items.erase(_items.begin()+index);
                
            } else {
                
                _items[index].events = events.get();
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
            
            for (auto &socket : _sockets) {
            
                auto events = Events(_items[index].revents);
                
                if ( events ) {
                 
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
    
    inline std::ostream &operator<< (std::ostream &stream, const Poller::Events &events) {
        
        stream << events.toString();

        return stream;
        
    }
}

#endif /* defined(__Messaging__Poller__) */
