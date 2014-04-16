//
//  MessageIterator.h
//  ATM
//
//  Created by Dave Meehan on 16/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ATM__MessageIterator__
#define __ATM__MessageIterator__

#include "Exception.h"

#include <zmq.h>

#include <memory>

namespace Messaging {
    
    class MessageIterator {
        
    public:
        MessageIterator(const std::shared_ptr<void> &socket, const int flags = 0)
        :
        _socket(socket),
        _flags(flags),
        _valid(false),
        _close_msg(false)
        {
            _valid = receive();
        }
        
        MessageIterator(const MessageIterator &) = delete;
        
        ~MessageIterator() {
            
            if (_valid) {
                while (more()) {
                    // dump remainder of message
                }
            }
            
            if (_close_msg) {
                zmq_msg_close(&_msg);
            }
            
        }
        
        const void *data() {
            
            if (_valid) {
                return zmq_msg_data(&_msg);
            }
            
            return nullptr;
            
        }
        
        const size_t size() {
            
            if (_valid) {
                return zmq_msg_size(&_msg);
            }
            
            return 0;
            
        }
        
        operator bool() const {
            
            return _valid;
            
        }
        
        bool more() {
            
            _valid = receive();
            
            return _valid;
            
        }
        
    protected:
        
        bool receive() {
            
            if (_close_msg) {
                
                auto more = zmq_msg_more(&_msg);
                
                zmq_msg_close(&_msg);
                
                _close_msg = false;
                
                if ( ! more) {
                    return more;
                }
            }
            
            zmq_msg_init(&_msg);
            _close_msg = true;
            
            auto len = zmq_msg_recv(&_msg, _socket.get(), _flags);
            
            if (len < 0) {
                if (len == EAGAIN) {
                    return false;
                }
                
                throw Exception("Receiving message part");
            }
            
            //                std::cout << std::this_thread::get_id() << ": [" << std::string((char*)zmq_msg_data(&_msg),zmq_msg_size(&_msg)) << "]" << std::endl;
            
            return true;
            
        }
        
    private:
        std::shared_ptr<void> _socket;
        zmq_msg_t _msg;
        int _flags;
        bool _valid;
        bool _close_msg;
        
    };
    
}

#endif /* defined(__ATM__MessageIterator__) */
