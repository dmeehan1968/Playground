//
//  FrameSpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 21/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "Frame.h"
#include <string.h>

namespace Messaging { namespace Specs {
   
    describe(Frame, {
    
        context("default constructor", {
            
            Frame frame;
            
            context("size", {

                it("has size of zero", {
                    
                    expect(frame.size()).should.equal(0);
                    
                });
                
            });
            
        });
        
        context("size constructor", {
           
            size_t expectedSize = 64;
            Frame frame(expectedSize);
            
            it("has expected size", {
                
                expect(frame.size()).should.equal(expectedSize);
                
            });
            
        });
        
        context("string constructor", {
           
            char expectedData[] = "this is a test";
            
            Frame frame(expectedData);
            
            it("has expected size", {
                
                expect(strlen(frame.data<char>())).should.equal(strlen(expectedData));
                
            });
            
            it("has expected data", {
                
                expect(strcmp(frame.data<char>(), expectedData)).should.equal(0);
                
            });
            
        });
        
    });
    
} }