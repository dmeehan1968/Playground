//
//  SessionCodecSpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 12/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "SessionCodec.h"

namespace Messaging { namespace Nodes { namespace Specs {
    
    describe(SessionCodec, {
       
        it("exists", {
            
            SessionCodec codec;
            
            codec.decode(Frame());
            
        });

        context("decoding", {
            
            context("abc message", {
                
                SessionCodec codec;
                Frame expectedAddress("12345");
                Frame expectedIdentity("ABC");
                
                it("accepts address", {
                    
                    expect((bool)codec.decode(expectedAddress)).should.beFalse();
                    
                });
                
                it("accepts delimiter", {
                    
                    expect((bool)codec.decode(Frame())).should.beFalse();
                    
                });
                
                context("creates on identity", {
                    
                    auto msg = codec.decode(expectedIdentity);
                    
                    it("creates", {
                        
                        expect((bool)msg).should.beTrue();
                        
                    });

                    it("has expected address", {
                        
                        expect(msg->address).should.equal(expectedAddress);
                        
                    });
                    
                    it("has expected identity", {
                        
                        expect(msg->identity).should.equal(expectedIdentity);
                        
                    });
                    
                    it("is expected type", {
                        
                        expect((bool)std::dynamic_pointer_cast<AbcMsg>(msg)).should.beTrue();
                        
                    });

                });
                
            });
            
            context("def message", {
                
                SessionCodec codec;
                Frame address("12345");
                Frame identity("DEF");
                
                it("accepts address", {
                    
                    expect((bool)codec.decode(address)).should.beFalse();
                });

                it("accepts delimiter", {
                    
                    expect((bool)codec.decode(Frame())).should.beFalse();
                    
                });
                
                it("accepts identity", {
                    
                    expect((bool)codec.decode(identity)).should.beFalse();
                    
                });

                it("accepts d frame", {
                    
                    expect((bool)codec.decode(Frame("d"))).should.beFalse();
                    
                });
                
                it("accepts e frame", {
                    
                    expect((bool)codec.decode(Frame("e"))).should.beFalse();
                    
                });
                
                context("accepts f frame", {
                    
                    auto msg = codec.decode(Frame("f"));
                    
                    it("creates msg", {
                        
                        expect((bool)msg).should.beTrue();
                        
                    });
                    
                    it("has expected address", {
                        
                        expect(msg->address).should.equal(address);
                        
                    });
                    
                    it("has expected identity", {
                        
                        expect(msg->identity).should.equal(identity);
                        
                    });
                    
                    context("msg specific", {
                        
                        auto def = std::dynamic_pointer_cast<DefMsg>(msg);
                        
                        it("has expected d value", {
                            
                            expect(def->d).should.equal(Frame("d"));
                            
                        });
                        
                        it("has expected e value", {
                            
                            expect(def->e).should.equal(Frame("e"));
                            
                        });
                        
                        it("has expected f value", {
                            
                            expect(def->f).should.equal(Frame("f"));
                            
                        });
                        
                    });
                    
                });
                
            });

        });
        
        context("encoding", {
        
            Frame address("12345");
            Frame identity("ABC");

            context("abc message", {
                
                AbcMsg abc;
                Frame address("12345");
                Frame identity("ABC");
                
                abc.address = address;
                abc.identity = identity;
                auto frames = abc.encode();
                
                it("has address", {
                    
                    expect(frames.size()).should.beGreaterThan(0);
                    expect(frames.front()).should.equal(address);
                    
                });
                
                it("has delimiter", {
                    
                    frames.pop_front();
                    
                    expect(frames.size()).should.beGreaterThan(0);
                    expect(frames.front()).should.equal(Frame());
                    
                });
                
                it("has identity", {
                    
                    frames.pop_front();
                    
                    expect(frames.size()).should.beGreaterThan(0);
                    expect(frames.front()).should.equal(identity);
                    
                });
                
                it("should be empty", {
                    
                    frames.pop_front();
                    
                    expect(frames.size()).should.equal(0);
                    
                });
                
            });

            context("def message", {
                
                DefMsg def;
                Frame address("12345");
                Frame identity("DEF");
                Frame d("d");
                Frame e("e");
                Frame f("f");
                
                def.address = address;
                def.identity = identity;
                def.d = d;
                def.e = e;
                def.f = f;
                
                context("to deque", {
                    
                    auto frames = def.encode();
                    
                    it("has address", {
                        
                        expect(frames.size()).should.beGreaterThan(0);
                        expect(frames.front()).should.equal(address);
                        
                    });
                    
                    it("has delimiter", {
                        
                        frames.pop_front();
                        
                        expect(frames.size()).should.beGreaterThan(0);
                        expect(frames.front()).should.equal(Frame());
                        
                    });
                    
                    it("has identity", {
                        
                        frames.pop_front();
                        
                        expect(frames.size()).should.beGreaterThan(0);
                        expect(frames.front()).should.equal(identity);
                        
                    });
                    
                    it("has d frame", {
                        
                        frames.pop_front();
                        
                        expect(frames.size()).should.beGreaterThan(0);
                        expect(frames.front()).should.equal(d);
                        
                    });
                    
                    it("has e frame", {
                        
                        frames.pop_front();
                        
                        expect(frames.size()).should.beGreaterThan(0);
                        expect(frames.front()).should.equal(e);
                        
                    });
                    
                    it("has f frame", {
                        
                        frames.pop_front();
                        
                        expect(frames.size()).should.beGreaterThan(0);
                        expect(frames.front()).should.equal(f);
                        
                    });
                    
                    it("should be empty", {
                        
                        frames.pop_front();
                        
                        expect(frames.size()).should.equal(0);
                        
                    });
                    
                });
                
                context("to socket", {
                    
                    Context context;
                    std::string endpoint("inproc://SessionCodec");
                    
                    Socket pub(context, Socket::Type::push);
                    pub.bind(endpoint);
                    
                    Socket sub(context, Socket::Type::pull);
                    sub.connect(endpoint);
                    
                    auto expected = def;
                    
                    it("sends to socket", {
                        
                        expect(def.encode(&pub, Frame::more::none)).should.haveCountOf(0);

                    });

                    it("gets address", {
                        
                        Frame address;
                        address.receive(sub, Frame::block::none);
                        
                        expect(address).should.equal(expected.address);
                        
                    });
                    
                    it("gets delimiter", {
                        
                        Frame delim;
                        delim.receive(sub, Frame::block::none);
                        
                        expect(delim).should.equal(Frame());
                        
                    });
                    
                    it("gets identity", {
                        
                        Frame ident;
                        ident.receive(sub, Frame::block::none);
                        
                        expect(ident).should.equal(expected.identity);
                        
                    });
                    
                    it("gets d frame", {
                        
                        Frame d;
                        d.receive(sub, Frame::block::none);
                        
                        expect(d).should.equal(expected.d);
                        
                    });
                    
                    it("gets e frame", {
                        
                        Frame e;
                        e.receive(sub, Frame::block::none);
                        
                        expect(e).should.equal(expected.e);
                        
                    });
                    
                    Frame f;

                    it("gets f frame", {
                        
                        f.receive(sub, Frame::block::none);
                        
                        expect(f).should.equal(expected.f);

                    });

                    it("f frame should have no more", {
                        
                        expect(f.hasMore()).should.beFalse();
                        
                    });

                });


            });
            
        });
        
    });
    
} } }