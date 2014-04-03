//
//  ContactStreamFormatter.h
//  ConcurrentContacts
//
//  Created by Dave Meehan on 03/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ConcurrentContacts__ContactStreamFormatter__
#define __ConcurrentContacts__ContactStreamFormatter__

#include "Contact.h"
#include "NameStreamFormatter.h"
#include "EmailStreamFormatter.h"

#include <ostream>

class ContactStreamFormatter {

public:
    
    ContactStreamFormatter(const Contact &contact) : _contact(contact) {}

    std::ostream &toStream(std::ostream &stream) const {

        NameStreamFormatter formatter(stream, " ", "Name: ");
        
        _contact.givenNames(std::cref(formatter));
        _contact.familyNames(std::cref(formatter));
        
        stream << std::endl;
        
        _contact.emails(EmailStreamFormatter(stream, ", ", "Email: "));
        
        stream << std::endl;
        
        return stream;
    }

private:
    
    const Contact &_contact;
    
};

inline std::ostream &operator << (std::ostream &stream, const ContactStreamFormatter &formatter) {
    
    return formatter.toStream(stream);
    
}


#endif /* defined(__ConcurrentContacts__ContactStreamFormatter__) */
