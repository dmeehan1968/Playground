//
//  AnyPredicate.h
//  ConcurrentContacts
//
//  Created by Dave Meehan on 03/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ConcurrentContacts__AnyPredicate__
#define __ConcurrentContacts__AnyPredicate__

template <class T>
class AnyPredicate {
    
public:
    bool operator() (T) {
        return true;
    }
    
};

#endif /* defined(__ConcurrentContacts__AnyPredicate__) */
