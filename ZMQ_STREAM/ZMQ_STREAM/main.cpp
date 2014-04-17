//
//  main.cpp
//  ZMQ_STREAM
//
//  Created by Dave Meehan on 17/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <iostream>
#include <map>

#include <zmq.h>

int main(int argc, const char * argv[])
{
    
    void *context = zmq_ctx_new();

    std::cout << "Server starting..." << std::endl;
    
    void *socket = zmq_socket(context, ZMQ_STREAM);
    
    if (zmq_bind(socket, "tcp://*:5555") < 0) {
        throw zmq_strerror(errno);
    }
    
    std::map<std::string, std::string> client_map;
    
    while (1) {

        std::pair<std::string, std::string> pair;
        
        for (int msg_index = 0 ; msg_index < 2; msg_index++) {
            
            zmq_msg_t msg;
            
            zmq_msg_init(&msg);
            
            auto size = zmq_msg_recv(&msg, socket, 0);
            
            if (size < 0) {
                throw zmq_strerror(errno);
            }
            
            std::string strmsg((char *)zmq_msg_data(&msg), zmq_msg_size(&msg));
        
            if (msg_index == 0) {
                pair.first = strmsg;
            } else {
                pair.second = strmsg;
            }
            
            zmq_msg_close(&msg);
            
        }

        std::cout << "[" << pair.first << "] ";
        
        if (pair.second.size() == 0) {
            
            auto found = client_map.find(pair.first);
            
            if (found != client_map.end()) {
                
                std::cout << "Disconnecting";
                client_map.erase(found);
                
            } else {
                
                std::cout << "Connecting";
                client_map[pair.first] = pair.second;
                
            }
            
        } else {
            
            std::cout << pair.second;
            
        }
        
        std::cout << std::endl;
        
        // Echo it back
        
        if (pair.second.size() > 0) {
            
            if (zmq_send(socket, pair.first.data(), pair.first.size(), ZMQ_SNDMORE) < 0) {
                throw zmq_strerror(errno);
            }
            
            if (zmq_send(socket, pair.second.data(), pair.second.size(), 0) < 0) {
                throw zmq_strerror(errno);
            }

        }
        
    }
    
    zmq_close(socket);
    zmq_ctx_destroy(context);
    
    return 0;
}

