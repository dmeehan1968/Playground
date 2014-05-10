//
//  StreamProtocol.h
//  Messaging
//
//  Created by Dave Meehan on 09/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__StreamProtocol__
#define __Messaging__StreamProtocol__

#include "Frame.h"

namespace StreamProtocol {

    template <class Frame = Messaging::Frame>
    class Codec {

    public:
        
        class StreamMessage {
        
        public:
            
            Frame client;
            Frame data;
            
        };
        
        enum MessageType {
            Stream
        };
        
        enum Field {
            Client,
            Data
        };
        
        Codec() {
            reset();
        }
        
        void reset() {
            
            type = MessageType::Stream;
            field = Field::Client;
            
        }
        
        bool decode(const Frame &frame) {

            switch (type) {

                case MessageType::Stream:
                    
                    switch (field) {
                            
                        case Field::Client:
                            
                            stream.client = frame;
                            field = Field::Data;
                            break;
                            
                        case Field::Data:
                            
                            stream.data = frame;
                            field = Field::Client;
                            return true;
                            
                        default:
                            throw;
                            
                    }

                    break;
                    
                default:
                    break;
            }
            
            if (! frame.hasMore()) {
                throw std::runtime_error("end of message before match");
            }
            
            return false;
            
        }
        
        Frame encode() {
        
            switch (type) {
                case MessageType::Stream:
                    
                    switch (field) {
                            
                        case Field::Client:
                            
                            field = Field::Data;
                            return stream.client;

                        case Field::Data:
                            
                            field = Field::Client;
                            return stream.data;
                            
                        default:
                            throw;
                    }
                    break;
                    
                default:
                    break;
            }
            
        }
        
        MessageType type;
        Field field;

        StreamMessage stream;
        
    };
    
}

#endif /* defined(__Messaging__StreamProtocol__) */
