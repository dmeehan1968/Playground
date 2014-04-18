//
//  main.cpp
//  Messaging
//
//  Created by Dave Meehan on 18/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <iostream>

#include <zmq.h>

int main(int argc, const char * argv[])
{

    void *ctx = zmq_ctx_new();
    
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}

