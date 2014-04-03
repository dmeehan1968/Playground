//
//  Contact.h
//  ConcurrentContacts
//
//  Created by Dave Meehan on 03/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ConcurrentContacts__Contact__
#define __ConcurrentContacts__Contact__

#include <functional>
#include <string>
#include <vector>

class Contact {
    
public:
    
    using Name = std::string;
    using Email = std::string;
    using Names = std::vector<Name>;
    using Emails = std::vector<Email>;
    
    Contact(const std::initializer_list<Name> &givenNames,
            const std::initializer_list<Name> &familyNames,
            const std::initializer_list<Email> &emails)
    :
        _givenNames(givenNames),
        _familyNames(familyNames),
        _emails(emails)
    {}
    
    void givenNames(std::function<void(const Name &)> functor) const {
        
        std::for_each(_givenNames.begin(), _givenNames.end(), functor);
        
    }
    
    void familyNames(std::function<void(const Name &)> functor) const {
        
        std::for_each(_familyNames.begin(), _familyNames.end(), functor);
        
    }
    
    void emails(std::function<void(const Email &)> functor) const {
        
        std::for_each(_emails.begin(), _emails.end(), functor);
        
    }
    
private:
    
    Names _givenNames;
    Names _familyNames;
    Emails _emails;
    
};

#endif /* defined(__ConcurrentContacts__Contact__) */
