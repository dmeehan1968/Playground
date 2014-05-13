//
//  Msg.h
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__Msg__
#define __Messaging__Msg__

#include <ostream>

namespace Messaging { namespace Protocol {
  
    class Msg {
        
    public:
        
        bool operator == (const Msg &msg) const {
            
            return true;
            
        }

        bool operator != (const Msg &msg) const {
            
            return ! operator == (msg);
        }
        
    };
    
    inline std::ostream &operator << (std::ostream &stream, const Msg &msg) {

        return stream << "Msg";
        
    }
} }

#endif /* defined(__Messaging__Msg__) */
