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

    using Codec = Codec<MockFrame>;
    
    context("decoding", {

        Codec codec;
        
        context("stream message", {
            
            MockFrame expectedClient("12345", true);
            MockFrame expectedData("ABCDEF", false);
            
            it("decodes first field", {
                
                codec.decode(expectedClient);
                
            });
            
            it("decodes second field", {
                
                codec.decode(expectedData);
                
            });
            
            it("is a message", {
                
                expect(codec.type).should.equal(Codec::MessageType::Stream);
                
            });
            
            it("matches client id", {
                
                expect(codec.stream.client).should.equal(expectedClient);
                
            });
            
            it("matches data", {
                
                expect(codec.stream.data).should.equal(expectedData);
                
            });
            
        });

    });

    context("encoding", {
        
        Codec codec;
        
        context("stream message", {
            
            codec.type = Codec::MessageType::Stream;
            codec.stream.client = MockFrame("12345", true);
            codec.stream.data = MockFrame("ABCDEF", false);
            
            it("encodes client", {
                
                expect(codec.encode().data()).should.equal("12345");
                
            });
            
            it("encodes data", {
                
                expect(codec.encode().data()).should.equal("ABCDEF");
                
            });
            
        });
        
    });

});

