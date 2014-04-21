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
            
            auto size = string.size();
            void *data = new char(size);
            memcpy(data, string.data(), size);
            
            zmq_msg_init_data(&_msg, data, size, &Frame::free_msg_data, NULL);
        }

        ~Frame() {
            zmq_msg_close(&_msg);
        }

        size_t size() const {
            return zmq_msg_size(&_msg);
        }
        
        template <typename T>
        T *data() const {
            return (T *)zmq_msg_data(&_msg);
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
            
            auto len = zmq_msg_send(&_msg, socket, flags);
            
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
            
            auto len = zmq_msg_recv(&_msg, socket, flags);
            
            if (len < 0) {
                
                if (errno == EAGAIN) {
                    return 0;
                }
                
                throw Exception("receive failed");
                
            }
            
            return len;
        }
        
    private:
        
        static void free_msg_data(void *ptr, void *hint) {
            delete (char *)ptr;
        }
        
        mutable zmq_msg_t _msg;
        
    };

}

#endif /* defined(__Messaging__Frame__) */
