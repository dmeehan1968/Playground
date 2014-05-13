//
//  NomSessionBase.h
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__NomSessionBase__
#define __Messaging__NomSessionBase__

#include "NomMessage.h"

namespace Messaging { namespace NomProtocol {
    
    class NomSessionBase {
        
    public:
        
        NomSessionBase() : numCheezBurgers(10) {}
        
        void OpenPeering(const Ohai &ohai) {
            
            if (numCheezBurgers) {
                
                OhaiOk().reply(ohai);
                
            }
        }
        
        void UsePeering(const ICanHaz &iCanHaz) {
            
            if (numCheezBurgers-- > 0) {
                
                CheezBurger().reply(iCanHaz);
                
            } else {
                
                Wtf().reply(iCanHaz);
                
            }
            
        }
        
        void UsePeering(const Hugz &hugz) {
            
            HugzOk().reply(hugz);
            
        }
        
    protected:
        
        unsigned numCheezBurgers;
        
    };
    
} }

#endif /* defined(__Messaging__NomSessionBase__) */
