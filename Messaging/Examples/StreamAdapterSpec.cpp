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
        
        context("protocol", {
            
            StreamAdapter::Protocol factory;
            
            beforeEach({
                
                factory = StreamAdapter::Protocol();
                
            });

            context("first frame", {
                
                std::shared_ptr<MalformedEvent> malformed;
                
                context("zero length", {
                    
                    beforeEach({
                        
                        auto event = factory.parse(nullptr, 0, true);
                        
                        malformed = std::dynamic_pointer_cast<MalformedEvent>(event);
                        
                    });

                    
                    it("returns malformed", {
                        
                        expect((bool)malformed).should.beTrue();
                        
                    });
                    
                });

                context("no more", {

                    beforeEach({
                        
                        auto event = factory.parse("ABC", 3, false);
                        
                        malformed = std::dynamic_pointer_cast<MalformedEvent>(event);
                        
                    });

                    
                    it("returns malformed", {
                        
                        expect((bool)malformed).should.beTrue();
                        
                    });
                    
                });

                context("valid client id", {
                    
                    it("returns null", {
                        
                        auto event = factory.parse("ABCDE", 5, true);
                        
                        expect((bool)event).should.beFalse();
                        
                    });
                    
                });

                
            });

            context("second frame", {
                
                StreamAdapter::StreamEvent::ClientID expectedClientId({ 'A', 'B', 'C', 'D', 'E' });
                
                StreamAdapter::DataEvent::Data expectedData( { 'D', 'A', 'T', 'A' } );
                
                beforeEach({
                    
                    factory.parse(expectedClientId.data(), expectedClientId.size(), true);
                });

                context("no data", {
                    
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

                context("has more", {
                    
                    std::shared_ptr<MalformedEvent> malformed;
                    
                    beforeEach({
                        
                        auto event = factory.parse(expectedData.data(), expectedData.size(), true);
                        
                        malformed = std::dynamic_pointer_cast<MalformedEvent>(event);
                        
                    });

                    it("returns malformed", {
                        
                        expect((bool)malformed).should.beTrue();
                        
                    });
                    
                });
                
                context("with data", {
                    
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

