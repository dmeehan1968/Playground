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
    
    void *socket = zmq_socket(context, ZMQ_STREAM);
    
    if (zmq_bind(socket, "tcp://*:5555") < 0) {
        throw zmq_strerror(errno);
    }
    
    while (1) {
        
        zmq_msg_t msg;
        
        zmq_msg_init(&msg);
        
        auto size = zmq_msg_recv(&msg, socket, 0);
        
        if (size < 0) {
            throw zmq_strerror(errno);
        }
        
        std::string strmsg((char *)zmq_msg_data(&msg), zmq_msg_size(&msg));
        
        std::cout << "[" << strmsg << "]" << std::endl;
        
        zmq_msg_close(&msg);
        
    }
    
    zmq_close(socket);
    zmq_ctx_destroy(context);
    
    return 0;
}

