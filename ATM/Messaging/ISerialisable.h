//
//  ISerialisable.h
//  ATM
//
//  Created by Dave Meehan on 16/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ATM__ISerialisable__
#define __ATM__ISerialisable__

namespace Messaging {
    
    class Socket;
    
    class ISerialisable {
        
    public:
        
        virtual int serialise(Socket &socket, const int flags = 0) const = 0;
        
    };
    
}

#endif /* defined(__ATM__ISerialisable__) */
