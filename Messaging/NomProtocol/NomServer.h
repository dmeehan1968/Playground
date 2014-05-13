//
//  NomServer.h
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__NomServer__
#define __Messaging__NomServer__

#include "NomSession.h"
#include "NomCodec.h"

#include "Msg.h"

#include "Frame.h"
#include "Reactor.h"

#include <memory>
#include <map>
#include <thread>


namespace Messaging { namespace NomProtocol {
  
    using namespace ::Messaging::Protocol;
    
    class NomServer {
        
    public:
        
        NomServer(const std::string &endpoint = std::string("inproc://nom"))
        :
            _socket(Context(), Socket::Type::router),
            _endpoint(endpoint)
        {}
        
        ~NomServer() {
            
            stop();
            
        }
        
        void start() {
        
            _serverThread = std::thread(std::ref(*this));
            
        }
        
        void operator()() {
            
            _socket.bind(_endpoint);
            
            _reactor.addObserver(_socket, Reactor::Event::Readable, std::bind(&NomServer::onSocketReadable, this, std::placeholders::_1, std::placeholders::_2));
            
            _reactor.run(100);

            try {
                
                _socket.unbind(_endpoint);
                
            } catch (Exception &e) {
                
                if (e.errorCode() != ENOENT) {
                    throw;
                }
            }
            
        }
        
        void stop() {
            
            _reactor.stop();
            
            if (_serverThread.joinable()) {
                _serverThread.join();
            }
        }
        
    protected:
        
        void onSocketReadable(Socket &socket, const Reactor::Event &event) {
            
            bool more = false;
            
            do {
                Frame frame;
                frame.receive(socket, Frame::block::none);
                
                auto msg = _codec.decode(frame);
                
                if (msg) {
                 
                    auto found = _sessions.find(msg->address);

                    if (found == _sessions.end()) {
                        
                        found = _sessions.emplace(msg->address, NomSession()).first;
                        
                    }
                    
                    auto &session = found->second;
                    
                    msg->setSocket(_socket);
                    
                    session.dispatch(msg);
                    
                }
                
            } while (more);
        }
        
    private:
        
        std::map<Frame, NomSession> _sessions;
        
        std::thread _serverThread;
        
        Reactor _reactor;
        Socket _socket;
        std::string _endpoint;
        NomCodec _codec;
        
    };
    
} }

#endif /* defined(__Messaging__NomServer__) */
