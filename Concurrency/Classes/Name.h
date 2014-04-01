//
//  Name.h
//  Concurrency
//
//  Created by Dave Meehan on 01/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Concurrency__Name__
#define __Concurrency__Name__

#include <string>

class Name : public std::string {
    
public:
    
    Name(const std::string &name)
    :
        std::string(name)
    {}
    
    Name(const char *name)
    :
        std::string(name)
    {}
    
};

#endif /* defined(__Concurrency__Name__) */
