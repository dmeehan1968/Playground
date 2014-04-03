//
//  NameStreamFormatter.h
//  ConcurrentContacts
//
//  Created by Dave Meehan on 03/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ConcurrentContacts__NameStreamFormatter__
#define __ConcurrentContacts__NameStreamFormatter__

#include "Contact.h"

#include <string>
#include <ostream>

class NameStreamFormatter {
    
public:
    
    NameStreamFormatter(std::ostream &stream, const std::string &delim)
    :
        _stream(stream),
        _delim(delim),
        _count(0)
    {}
    
    void operator() (const Contact::Name &name) const {
        
        if (_count++ > 0) {
            _stream << " ";
        }
        
        _stream << name;
        
    }
    
private:
    
    std::ostream &_stream;
    std::string _delim;
    mutable unsigned _count;
    
};

#endif /* defined(__ConcurrentContacts__NameStreamFormatter__) */
