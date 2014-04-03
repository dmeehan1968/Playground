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
    
    template <class... T>
    using Predicate = std::function<bool(T...)>;
    
    template <class... T>
    using Action = std::function<void(T...)>;
    
    void create(const std::initializer_list<Contact::Name> &givenNames,
                const std::initializer_list<Contact::Name> &familyNames,
                const std::initializer_list<Contact::Email> &emails) {
        
        _contacts.emplace_back(givenNames, familyNames, emails);
        
    }
    
    void matching(const Predicate<const Contact &> &predicate, const Action<const Contact &> &action) const {
        
        std::for_each(_contacts.begin(), _contacts.end(), [&predicate, &action](const Contact &contact) {
           
            if (predicate(contact)) {
                action(contact);
            }
            
        });
    }
    
private:
    
    std::vector<Contact> _contacts;
    
};

#endif /* defined(__ConcurrentContacts__Contacts__) */
