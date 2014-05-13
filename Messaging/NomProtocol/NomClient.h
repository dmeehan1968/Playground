//
//  NomClient.h
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__NomClient__
#define __Messaging__NomClient__

#include "NomMessage.h"

#include <memory>

namespace Messaging { namespace NomProtocol {
  
    class NomClient {

    public:
        
        std::shared_ptr<Msg> ohai() {
        
            return nullptr;
            
        }
        
        std::shared_ptr<Msg> iCanHaz() {
            
            return nullptr;
            
        }
        
        std::shared_ptr<Msg> hugz() {
            
            return nullptr;
            
        }
        
    };
    
} }

#endif /* defined(__Messaging__NomClient__) */
