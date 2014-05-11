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

    context("decoding", {

        Codec codec;
        
        context("stream message", {
            
            Frame expectedClient("12345");
            Frame expectedData("ABCDEF");
            
            context("client frame", {
                
                auto event = codec.decode(expectedClient);
                
                it("accepts, no event", {
                    
                    expect(event.get()).should.beNil();

                });
                
            });

            context("data frame", {
                
                auto event = codec.decode(expectedData);
                
                it("accepts, decodes event", {
                    
                    expect(event.get()).shouldNot.beNil();
                    
                });
                
                context("data event", {
                    
                    auto data = std::dynamic_pointer_cast<DataEvent>(event);
                    
                    it("is a data event", {
                        
                        expect(data.get()).shouldNot.beNil();
                        
                    });
                    
                    if (data) {
                    
                        it("has expected client", {
                            
                            expect(data->replyTo).should.equal(expectedClient);
                            
                        });
                        
                        it("has expected data", {
                            
                            expect(data->data).should.equal(expectedData);
                            
                        });
                        
                    }
                    
                });
                
            });
            
        });

    });

});

