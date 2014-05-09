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
    class Parser {

    public:
        
        enum Type {
            Message
        };
        
        enum Field {
            Client,
            Data
        };
        
        Parser()
        :
            type(Type::Message),
            field(Field::Client)
        {}
        
        bool parse(const Frame &frame) {

            switch (type) {

                case Type::Message:
                    
                    switch (field) {
                            
                        case Field::Client:
                            
                            client = frame;
                            field = Field::Data;
                            break;
                            
                        case Field::Data:
                            
                            data = frame;
                            field = Field::Client;
                            return true;
                            
                        default:
                            break;
                            
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
        
        Type type;
        Field field;
        
        Frame client;
        Frame data;
        
    };
    
}

#endif /* defined(__Messaging__StreamProtocol__) */
