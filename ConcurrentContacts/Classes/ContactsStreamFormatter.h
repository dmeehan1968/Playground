//
//  ContactsStreamFormatter.h
//  ConcurrentContacts
//
//  Created by Dave Meehan on 03/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ConcurrentContacts__ContactsStreamFormatter__
#define __ConcurrentContacts__ContactsStreamFormatter__

#include "Contacts.h"
#include "ContactStreamFormatter.h"
#include "AnyPredicate.h"

#include <ostream>

class ContactsStreamFormatter {
    
public:
    
    ContactsStreamFormatter(const Contacts &contacts)
    :
        _contacts(contacts)
    {}
    
    std::ostream &toStream(std::ostream &stream) const {
        
        _contacts.matching(AnyPredicate<const Contact &>(), [&stream](const Contact &contact) {
           
            stream << ContactStreamFormatter(contact);
            
        });
        
        return stream;
    }
    
private:
    
    Contacts _contacts;
    
};

inline std::ostream &operator << (std::ostream &stream, const ContactsStreamFormatter &formatter) {
    
    return formatter.toStream(stream);
    
}

#endif /* defined(__ConcurrentContacts__ContactsStreamFormatter__) */
