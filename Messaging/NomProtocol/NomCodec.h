//
//  NomCodec.h
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__NomCodec__
#define __Messaging__NomCodec__

#include "NomMessage.h"

#include "Frame.h"

#include <deque>

namespace Messaging { namespace NomProtocol {
    
    using namespace ::Messaging::Protocol;
    
    class NomCodec {
        
    public:
        
        using Address = Msg::Address;
        using Envelope = Msg::Envelope;
        
        NomCodec(const Address address, const Envelope envelope)
        :
            _address(address),
            _envelope(envelope)
        {
            
            reset();
            
        }
        
        void reset() {
            
            _msgType = MsgType::Msg;
            _msg = std::make_shared<Msg>(_address, _envelope);
            
        }
        
        template <class T>
        void encode(Socket &socket, T &&msg, const Frame::more more_type) {
            
            msg.encode(socket, _address, _envelope, more_type);
            
        }
        
        std::shared_ptr<Msg> decode(const Frame &frame) {
            
            switch (_msgType) {
                    
                case MsgType::Msg:
                {
                    if (_msg->decode(frame)) {
                        
                        // detect message types and transform
                        
                        if (_msg->identity == "OHAI") {
                            
                            _msg = std::make_shared<Ohai>(*_msg);
                            _msgType = MsgType::_Final;
                            
                        } else if (_msg->identity == "OHAI-OK") {
                            
                            _msg = std::make_shared<OhaiOk>(*_msg);
                            _msgType = MsgType::_Final;
                            
                        } else if (_msg->identity == "WTF") {
                            
                            _msg = std::make_shared<Wtf>(*_msg);
                            _msgType = MsgType::_Final;
                            
                        } else if (_msg->identity == "ICANHAZ") {
                            
                            _msg = std::make_shared<ICanHaz>(*_msg);
                            _msgType = MsgType::_Final;
                            
                        } else if (_msg->identity == "CHEEZBURGER") {
                            
                            _msg = std::make_shared<CheezBurger>(*_msg);
                            _msgType = MsgType::_Final;
                            
                        } else if (_msg->identity == "HUGZ") {
                            
                            _msg = std::make_shared<Hugz>(*_msg);
                            _msgType = MsgType::_Final;
                            
                        } else if (_msg->identity == "HUGZ-OK") {
                            
                            _msg = std::make_shared<HugzOk>(*_msg);
                            _msgType = MsgType::_Final;
                            
                        } else {
                            
                            throw std::runtime_error("unexpected identity");
                            
                        }
                        
                    }
                    break;
                }

            /*
                case MsgType::XXX:
                    // Repeat for other message types
                {
                    // specific message decode handles error
                    
                    _msg->decode(frame);
                    break;
                }
             */
                    
                default:
                {
                    throw std::runtime_error("invalid message type");
                    break;
                }
                    
            }
            
            if (_msg->isFinal()) {
                
                auto retval = _msg;
                reset();
                return retval;
            }
            
            return nullptr;
            
        }
        
    private:
        
        enum class MsgType {
            
            Msg,
            
            // insert message types here
            Ohai,
            OhaiOk,
            Wtf,
            ICanHaz,
            CheezBurger,
            Hugz,
            HugzOk,

            // Leave this alone
            
            _Final
            
        };
        
        MsgType _msgType;
        std::shared_ptr<Msg> _msg;

        Address _address;
        Envelope _envelope;
        
    };
    
} }

#endif /* defined(__Messaging__NomCodec__) */
