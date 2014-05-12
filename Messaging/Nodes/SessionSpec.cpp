//
//  SessionSpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 12/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "Session.h"
#include <thread>

namespace Messaging { namespace Specs {
    
    using namespace SessionManager;
    
    describe(Session, {

        Context context;
        Socket serverSocket(context, Socket::Type::router);
        std::string endpoint("inproc://SessionSpec");
        serverSocket.bind(endpoint);
        serverSocket.setReceiveTimeout(100);
        Socket clientSocket(context, Socket::Type::request);
        clientSocket.connect(endpoint);
        
        Client client(clientSocket);
        Server server(serverSocket);
        
        auto serverThread = std::thread(std::ref(server));

        it("gets state", {
            
            Session expectedSession;
            std::string expectedData("ABCDEF");
            expectedSession.setData(expectedData);
            
            SessionManager::SessionId testAddress("12345");
            
            client.set(testAddress, expectedSession);
            
            auto actualState = client.get(testAddress);
            
            expect(*actualState).should.equal(expectedSession);
            
        });

        server.stop();
        serverThread.join();
        
    });
    
} }