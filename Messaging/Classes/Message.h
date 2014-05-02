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
    
    class Message : public std::deque<Frame> {
      
    public:
        
        using block = Frame::block;
        
        size_t send(Socket &socket, const block block_type = block::blocking) {
        
            size_t len = 0;
            
            if (size() == 0) {
                throw Exception("cannot send an empty message", 0);
            }
            
            while (size() > 0) {
            
                Frame::more more_type = size() > 1 ? Frame::more::more : Frame::more::none;
                
                len += front().send(socket, block_type, more_type);

                pop_front();
                
            }
            
            return len;
            
        }
        
        size_t receive(Socket &socket, const block block_type = block::blocking) {
            
            size_t len = 0;
            auto more = false;
            
            if (size() > 0) {
                throw Exception("cannot receive into message that already has frames", 0);
            }
            
            do {
                
                Frame frame;
                
                try {
                    
                    len += frame.receive(socket, block_type);
                    
                } catch( Exception &e ) {
                    
                    if (e.errorCode() == EAGAIN) {
                      
                        if (more) {
                            errno = EFAULT;
                            throw Exception("unable to read when more expected");
                        }
                        
                    }
                    
                    throw;
                }
                
                more = frame.hasMore();
                
                push_back(std::move(frame));
                
            } while (more);
            
            return len;
            
        }
        
    private:
        
    };
    
}

#endif /* defined(__Messaging__Message__) */
