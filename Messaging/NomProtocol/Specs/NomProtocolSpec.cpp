//
//  NomProtocolSpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "NomServer.h"
#include "NomClient.h"

namespace Messaging { namespace NomProtocol { namespace Specs {
    
    describe(NomProtocol, {

        NomServer server;
        NomClient client;

        server.start();
        
        it("opens peering", {
            
            auto reply = client.ohai();
            
            expect(reply && reply->isa<Ohai>()).should.beTrue();
            
        });

        
    });
    
} } }
