//
//  Contacts.h
//  ConcurrentContacts
//
//  Created by Dave Meehan on 03/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ConcurrentContacts__Contacts__
#define __ConcurrentContacts__Contacts__

#include "Contact.h"

#include <vector>
#include <initializer_list>

class Contacts {
    
public:
    
    void create(const std::initializer_list<Contact::Name> &givenNames,
                const std::initializer_list<Contact::Name> &familyNames,
                const std::initializer_list<Contact::Email> &emails) {
        
        _contacts.emplace_back(givenNames, familyNames, emails);
        
    }
    
private:
    
    std::vector<Contact> _contacts;
    
};

#endif /* defined(__ConcurrentContacts__Contacts__) */
