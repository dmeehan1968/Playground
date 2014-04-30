//
//  Frame.h
//  Messaging
//
//  Created by Dave Meehan on 21/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__Frame__
#define __Messaging__Frame__

#include "Socket.h"

#include <cstdlib>
#include <iostream>
#include <sstream>

namespace Messaging {

    class Frame {
        
    public:
        
        Frame() {
            zmq_msg_init(&_msg);
        }
        
        Frame(const size_t size) {
            zmq_msg_init_size(&_msg, size);
        }
        
        Frame(const std::string &string) {
            
            zmq_msg_init_size(&_msg, string.size());
            memcpy(zmq_msg_data(&_msg), string.data(), string.size());

        }

        ~Frame() {
            zmq_msg_close(&_msg);
        }

        Frame(const Frame &other) {
            copyFrom(other);
        }
        
        Frame & operator = (const Frame &other) {
            copyFrom(other);
            return *this;
        }
        
        Frame(Frame &&other) {
            moveFrom(other);
        }
        
        Frame & operator = (Frame &&other) {
            moveFrom(other);
            return *this;
        }
        
        size_t size() const {
            return zmq_msg_size(&_msg);
        }
        
        template <typename T>
        T *data() const {
            return (T *)zmq_msg_data(&_msg);
        }
        
        std::string str() const {
            return std::string(data<char>(), size());
        }
        
        int integer() const {
            
            int i = 0;
            std::istringstream(str()) >> i;
            
            return i;
        }
        
        enum class block {
            none,
            blocking
        };
        
        enum class more {
            none,
            more
        };
        
        size_t send(Socket &socket, const block block_type, const more more_type) {
        
            int flags = 0;
            flags |= block_type == block::none ? ZMQ_DONTWAIT : 0;
            flags |= more_type == more::more ? ZMQ_SNDMORE : 0;
            
            auto len = zmq_msg_send(&_msg, socket.get(), flags);
            
            if (len < 0) {

                if (errno == EAGAIN) {
                    return 0;
                }
                
                throw Exception("send failed");
            }
            
            return len;
            
        }
        
        size_t receive(Socket &socket, const block block_type) {
            
            int flags = 0;
            flags |= block_type == block::none ? ZMQ_DONTWAIT : 0;
            
            auto len = zmq_msg_recv(&_msg, socket.get(), flags);
            
            if (len < 0) {
                
                throw Exception("receive failed");
                
            }
            
            return len;
        }
        
        bool hasMore() const {
            return zmq_msg_more(&_msg);
        }
        
    protected:
        
        void copyFrom(const Frame &other) {
            zmq_msg_init_size(&_msg, other.size());
            memcpy(zmq_msg_data(&_msg), other.data<void>(), other.size());
        }
        
        void moveFrom(const Frame &other) {
            zmq_msg_move(&_msg, &other._msg);
        }
        
    private:
        
        mutable zmq_msg_t _msg;
        
    };

}

#endif /* defined(__Messaging__Frame__) */
