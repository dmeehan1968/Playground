//
//  StreamAdapterSpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 30/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "StreamAdapter.h"

namespace Messaging { namespace Examples {

    describe(StreamAdapter, {
        
        Context ctx;
        Socket stream(ctx, Socket::Type::stream);
        Socket output(ctx, Socket::Type::push);
        
        it("exists", {
            
            StreamAdapter::Device adapter(stream, output);
            
        });
        
        context("event factory", {
            
            StreamAdapter::EventFactory factory;
            
            beforeEach({
                
                factory = StreamAdapter::EventFactory();
                
            });

            it("returns malformed if initial frame is zero", {
                
                auto event = factory.parse(nullptr, 0, true);
                
                expect((bool)std::dynamic_pointer_cast<MalformedEvent>(event)).should.beTrue();
                
            });

            it("returns malformed if initial frame has no more", {
                
                auto event = factory.parse("ABC", 3, false);

                expect((bool)std::dynamic_pointer_cast<MalformedEvent>(event)).should.beTrue();
                
            });
            
            it("returns null with valid client id", {
                
                auto event = factory.parse("ABCDE", 5, true);
                
                expect((bool)event).should.beFalse();
                
            });

            context("valid client id", {

                StreamAdapter::StreamEvent::ClientID expectedClientId( { 'A', 'B', 'C', 'D', 'E' });
                
                StreamAdapter::DataEvent::Data expectedData( { 'D', 'A', 'T', 'A' } );
                
                beforeEach({
                    
                    factory.parse(expectedClientId.data(), expectedClientId.size(), true);
                    
                });
                
                it("returns malformed if has more", {
                    
                    auto event = factory.parse(expectedData.data(), expectedData.size(), true);
                    
                    expect((bool)std::dynamic_pointer_cast<MalformedEvent>(event)).should.beTrue();
                    
                });
                
                context("data size is zero", {

                    std::shared_ptr<StreamAdapter::ConnectionEvent> connectionEvent;

                    beforeEach({

                        auto event = factory.parse("", 0, false);
                        
                        connectionEvent = std::dynamic_pointer_cast<StreamAdapter::ConnectionEvent>(event);
                    });

                    it("returns ConnectionEvent", {
                        
                        expect((bool)connectionEvent).should.beTrue();
                        
                    });
                    
                    it("has expected client id", {
                        
                        expect(connectionEvent->clientId()).should.equal(expectedClientId);
                        
                    });

                    
                });

                context("data size is greater than zero", {

                    std::shared_ptr<StreamAdapter::DataEvent> dataEvent;
                    
                    beforeEach({

                        auto event = factory.parse(expectedData.data(), expectedData.size(), false);

                        dataEvent = std::dynamic_pointer_cast<StreamAdapter::DataEvent>(event);
                        
                    });

                    it("returns DataEvent", {

                        expect((bool)dataEvent).should.beTrue();
                        
                    });

                    it("has expected client id", {
                        
                        expect(dataEvent->clientId()).should.equal(expectedClientId);
                        
                    });
                    
                    it("has expected data", {
                        
                        expect(dataEvent->data()).should.equal(expectedData);
                        
                    });

                });

            });

        });

    });
    
} }

