//
//  Message.h
//  Messaging
//
//  Created by Dave Meehan on 21/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__Message__
#define __Messaging__Message__

#include "Frame.h"

#include <deque>

namespace Messaging {
    
    class Message {
      
        using Envelope = std::deque<Frame>;

    private:
        
        std::deque<Frame> _data;
        std::deque<Frame> _envelope;

    public:
    
        using block = Frame::block;
        
        size_t send(Socket &socket, const block block_type = block::blocking) {
        
            size_t len = 0;
            
            if (_data.size() == 0) {
                throw Exception("cannot send an empty message", 0);
            }
            
            while (_envelope.size() > 0) {
                
                _envelope.front().send(socket, block_type, Frame::more::more);
                
                _envelope.pop_front();
                
            }
            
            if (socket.getSocketType() != Socket::Type::stream) {
                Frame().send(socket, block_type, Frame::more::more);
            }
            
            while (_data.size() > 0) {
            
                Frame::more more_type = _data.size() > 1 ? Frame::more::more : Frame::more::none;
                
                len += _data.front().send(socket, block_type, more_type);

                _data.pop_front();
                
            }
            
            return len;
            
        }
        
        size_t receive(Socket &socket, const block block_type = block::blocking) {
            
            if (_data.size() > 0) {
                throw Exception("cannot receive into message that already has frames", 0);
            }
            
            auto env = receiveEnvelope(socket, block_type);
            
            if (env.second == false) {
                throw Exception("no data after envelope", 0);
            }
            
            auto len = receiveData(socket, env.second ? block::none : block_type, env.second);
            
            return env.first + len;
            
        }
        
        auto back() -> decltype(_data.back()) {
            return _data.back();
        }
        
        auto back() const -> decltype(_data.back()) {
            return _data.back();
        }
        
        auto front() -> decltype(_data.front()) {
            return _data.front();
        }
        
        auto front() const -> decltype(_data.front()) {
            return _data.front();
        }
        
        auto size() const -> decltype(_data.size()) {
            return _data.size();
        }
        
        auto clear() -> decltype(_data.clear()) {
            return _data.clear();
        }
        
        template <class...T>
        auto emplace_back(T &&...Args) -> decltype(_data.emplace_back()) {
            return _data.emplace_back(std::forward<T...>(Args...));
        }
        
        auto push_back(decltype(_data)::value_type &value) -> decltype(_data.push_back(value)) {
            _data.push_back(value);
        }
        
        auto envelope() -> std::add_lvalue_reference<decltype(_envelope)>::type {
            return _envelope;
        }
        
    protected:
        
        std::pair<size_t, bool> receiveEnvelope(Socket &socket, block block_type) {
        
            size_t len = 0;
            bool more = false;
            
            auto socketType = socket.getSocketType();
            
            do {
                
                Frame frame;
                
                try {
                    
                    len += frame.receive(socket, block_type);
                    
                    block_type = block::none;
                    
                } catch (Exception &e) {
                
                    if (e.errorCode() == EAGAIN && more) {
                        errno = EFAULT;
                        throw Exception("unable to read when more expected");
                    }
                    
                    throw;
                    
                }
            
                more = frame.hasMore();
                
                if (frame.size() > 0) {
                    _envelope.emplace_back(std::move(frame));
                } else {
                    break;
                }
                
                if (socketType == Socket::Type::stream) {
                    break;
                }
                
            } while (more);
            
            return std::pair<size_t, bool>(len, more);
            
        }
        
        size_t receiveData(Socket &socket, block block_type, bool more) {

            size_t len = 0;
            
            do {
                
                Frame frame;
                
                try {
                    
                    len += frame.receive(socket, block_type);
                    
                    block_type = block::none;
                    
                } catch (Exception &e) {
                    
                    if (e.errorCode() == EAGAIN && more) {
                        errno = EFAULT;
                        throw Exception("unable to read when more expected");
                    }
                    
                    throw;
                    
                }
                
                more = frame.hasMore();
                
                _data.emplace_back(std::move(frame));
                
            } while (more);
            
            return len;
            
        }
        
    };
    
}

#endif /* defined(__Messaging__Message__) */
