//
//  NilFactory.h
//  ATM
//
//  Created by Dave Meehan on 16/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ATM__NilFactory__
#define __ATM__NilFactory__

#include "MessageIterator.h"

#include <memory>

namespace Messaging {
    
    class NilFactory {
        
    public:
        using value_type = void;
        using pointer_type = std::shared_ptr<void>;
        
        static pointer_type create(MessageIterator &) {
            return pointer_type(nullptr);
        }
        
    };
    
}

#endif /* defined(__ATM__NilFactory__) */
