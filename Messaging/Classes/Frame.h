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
#include <iomanip>

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
            zmq_msg_init(&_msg);
            copyFrom(other);
        }
        
        Frame & operator = (const Frame &other) {
            zmq_msg_init(&_msg);
            copyFrom(other);
            return *this;
        }
        
        Frame(Frame &&other) {
            zmq_msg_init(&_msg);
            moveFrom(std::move(other));
        }
        
        Frame & operator = (Frame &&other) {
            zmq_msg_init(&_msg);
            moveFrom(std::move(other));
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
            
            return std::stoi(str(), nullptr, 10);
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
            
            if (len >= 0 && len != size()) {
                throw Exception("msg size inconsistency", 0);
            }
            
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
            auto rc = zmq_msg_init_size(&_msg, other.size());
            
            if (rc < 0) {
                throw Exception("message init with size failed");
            }
            
            memcpy(zmq_msg_data(&_msg), other.data<void>(), other.size());
        }
        
        void moveFrom(Frame &&other) {
            auto rc = zmq_msg_move(&_msg, &other._msg);
            
            if (rc < 0) {
                throw Exception("message move failed");
            }
        }
        
    private:
        
        mutable zmq_msg_t _msg;
        
    };

    inline std::ostream &operator << (std::ostream &stream, const Frame &frame) {
        
        auto size = frame.size();
        unsigned char *ptr = frame.data<unsigned char>();
        unsigned char *end = ptr + size;

        auto flags = stream.flags();
        
        while (ptr < end) {
            
            auto needle = ptr;
            
            
            for (int i=0; i < 16 && needle < end; i++, needle++) {
                
                if (i > 0) stream << " ";
                
                stream << std::hex << std::setw(2) << std::setfill('0');
                
                stream << (int)*needle;
                
            }

            stream.flags(flags);
            stream << " ";
            
            needle = ptr;
            for (int i=0; i < 16 && needle < end; i++, needle++) {
                
                stream << (unsigned char)(isprint(*needle) ? *needle : '.');
                
            }
            
            ptr = needle;
            
        }
        
        stream.flags(flags);
        
        return stream;
        
    }
}

#endif /* defined(__Messaging__Frame__) */
