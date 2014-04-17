//
//  main.cpp
//  ZMQ_STREAM
//
//  Created by Dave Meehan on 17/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <iostream>

#include <zmq.h>

int main(int argc, const char * argv[])
{
    
    void *context = zmq_ctx_new();

    std::cout << "Server starting..." << std::endl;
    
    return 0;
}

