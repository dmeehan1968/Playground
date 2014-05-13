//
//  NomSession.h
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__NomSession__
#define __Messaging__NomSession__

#include "NomMessage.h"
#include "NomSessionBase.h"
#include "Dispatch.h"

#include <memory>

namespace Messaging { namespace NomProtocol {

    using namespace ::Messaging::Protocol;
    
    class NomSession : public NomSessionBase {
      
    public:

        NomSession()
        :
            _state(State::OpenPeering)
        {}
        
        void dispatch(const std::shared_ptr<Msg> &msg) {
            
            switch (_state) {
                    
                case State::OpenPeering:
                {
                    
                    Dispatch<Msg>(*msg).handle<Ohai>([&](const Ohai &ohai) {
                        
                        OpenPeering(ohai);
                        
                    }).handle<Wtf>([&](const Wtf &wtf) {
                        
                        OpenPeering(wtf);
                        
                    });
                    
                    break;
                }
                    
                case State::UsePeering:
                {
                    
                    Dispatch<Msg>(*msg).handle<ICanHaz>([&](const ICanHaz &iCanHaz) {
                       
                        UsePeering(iCanHaz);
                        
                    }).handle<Hugz>([&](const Hugz &hugz) {
                        
                        UsePeering(hugz);
                        
                    });
                    
                    break;
                }
                default:
                    break;
            }
        }
        
        std::shared_ptr<Msg> hugz() {
            
            return nullptr;
            
        }
        
    private:
        
        enum class State {
            
            OpenPeering,
            UsePeering
            
        };
        
        State _state;
        
    };
    
} }

#endif /* defined(__Messaging__NomSession__) */
