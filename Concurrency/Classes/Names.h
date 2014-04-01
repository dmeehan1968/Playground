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
#include <ostream>

class Names : public std::vector<Name> {

public:
    
    Names(const std::initializer_list<Name> &names)
    :
        std::vector<Name>(names)
    {}
    
};

inline std::ostream &operator << (std::ostream &stream, const Names &names) {
    
    std::string nameString;
    
    std::for_each(names.begin(), names.end(), [&nameString](const Name &name) {
        
        if (name.size() < 1) {
            return;
        }
        
        if (nameString.size() > 0) {
            nameString += " ";
        }
        
        nameString += name;
        
    });
    
    return stream << nameString;
}

#endif /* defined(__Concurrency__Names__) */
