//
//  StreamProtocolSpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 09/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "StreamProtocol.h"

using namespace StreamProtocol;
using namespace Messaging;

describe(StreamProtocol, {

    Parser parser;
    
    context("message", {
        
        Frame expectedClient("12345");
        Frame expectedData;
        
        it("parses first field", {
            
            parser.parse(expectedClient);
            
        });

        it("parses second field", {
            
            parser.parse(expectedData);
            
        });
        
        it("is a message", {
            
            expect(parser.type).should.equal(Parser::Type::Message);
            
        });

        it("matches client id", {
            
            expect(parser.client).should.equal(expectedClient);
            
        });
        
        it("matches data", {
            
            expect(parser.data).should.equal(expectedData);
            
        });


        
    });

    
});
