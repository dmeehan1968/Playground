//
//  Names.h
//  Concurrency
//
//  Created by Dave Meehan on 01/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Concurrency__Names__
#define __Concurrency__Names__

#include "Name.h"

#include <vector>

class Names : public std::vector<Name> {

public:
    
    Names(const std::initializer_list<Name> &names)
    :
        std::vector<Name>(names)
    {}
    
};

#endif /* defined(__Concurrency__Names__) */
