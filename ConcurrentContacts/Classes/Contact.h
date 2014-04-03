//
//  Contact.h
//  ConcurrentContacts
//
//  Created by Dave Meehan on 03/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ConcurrentContacts__Contact__
#define __ConcurrentContacts__Contact__

#include <string>

class Contact {
    
public:
    
    using Name = std::string;
    using Email = std::string;
    
    Contact(const std::initializer_list<Name> &givenNames,
            const std::initializer_list<Name> &familyNames,
            const std::initializer_list<Email> &emails)
    {}
    
};

#endif /* defined(__ConcurrentContacts__Contact__) */
