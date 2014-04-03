//
//  main.cpp
//  ConcurrentContacts
//
//  Created by Dave Meehan on 03/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include "Contact.h"
#include "Contacts.h"
#include "ContactStreamFormatter.h"

#include <iostream>
#include <stdio.h>

int main(int argc, char *argv[]) {
    
    Contacts contacts;
    
    contacts.create({"David", "John"},
                    {"Meehan"},
                    {"dave.meehan@test.com", "dave.meehan@another.com"});
    
    return 0;
    
}