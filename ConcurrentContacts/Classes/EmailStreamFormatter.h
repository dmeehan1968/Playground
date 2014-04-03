//
//  EmailStreamFormatter.h
//  ConcurrentContacts
//
//  Created by Dave Meehan on 03/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ConcurrentContacts__EmailStreamFormatter__
#define __ConcurrentContacts__EmailStreamFormatter__

#include "Contact.h"

#include <string>
#include <ostream>

class EmailStreamFormatter {
    
public:
    
    EmailStreamFormatter(std::ostream &stream, const std::string &delim, const std::string &prefix)
    :
        _stream(stream),
        _delim(delim),
        _prefix(prefix),
        _count(0)
    {}
    
    void operator() (const Contact::Email &email) const {
        
        if (_count++ > 0) {
            _stream << _delim;
        } else {
            _stream << _prefix;
        }
        
        _stream << email;
        
    }
    
private:
    
    std::ostream &_stream;
    std::string _delim;
    std::string _prefix;
    mutable unsigned _count;
    
};


#endif /* defined(__ConcurrentContacts__EmailStreamFormatter__) */
