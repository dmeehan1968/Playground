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

class MockFrame {
    
public:
    
    MockFrame()
    :
    _more(false)
    {}
    
    MockFrame(const std::string data, const bool more)
    :
    _data(data),
    _more(more)
    {}
    
    bool hasMore() const {
        return _more;
    }
    
    std::string data() const {
        return _data;
    }
    
    bool operator == (const MockFrame &other) const {
        return _more == other._more && _data == other._data;
    }
    
    bool operator != (const MockFrame &other) const {
        return ! operator ==(other);
    }
    
private:
    
    std::string _data;
    bool _more;
    
};

inline std::ostream &operator << (std::ostream &stream, const MockFrame &frame) {

    stream << frame.data();
    return stream;
    
}

describe(StreamProtocol, {

    using Parser = Parser<MockFrame>;
    
    Parser parser;
    
    context("message", {
        
        MockFrame expectedClient("12345", true);
        MockFrame expectedData;
        
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

