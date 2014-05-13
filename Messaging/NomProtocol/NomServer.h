//
//  NomServer.h
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__NomServer__
#define __Messaging__NomServer__

#include "NomSession.h"

#include "Msg.h"

#include "Frame.h"

#include <memory>
#include <map>

namespace Messaging { namespace NomProtocol {
  
    using namespace ::Messaging::Protocol;
    
    class NomServer {
        
    public:
        
        void start() {
            
        }
        
        std::shared_ptr<Msg> hugz() {
            
            return nullptr;
            
        }
        
    private:
        
        std::map<Frame, NomSession> _sessions;
        
    };
    
} }

#endif /* defined(__Messaging__NomServer__) */
