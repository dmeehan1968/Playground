//
//  NomMessage.h
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__NomMessage__
#define __Messaging__NomMessage__

#include "Msg.h"

namespace Messaging { namespace NomProtocol {

    using namespace ::Messaging::Protocol;
    
    class Ohai : public Msg {
    
    public:
        
        Ohai() = default;
        Ohai(const Msg &msg) : Msg(msg) {}
        
        virtual Ohai *clone() const override {
            return new Ohai(*this);
        }
    
    };
    
    class OhaiOk : public Msg {
    
    public:
        
        OhaiOk() = default;
        OhaiOk(const Msg &msg) : Msg(msg) {}
        
        virtual OhaiOk *clone() const override {
            return new OhaiOk(*this);
        }
        
    };
    
    class Wtf : public Msg {
        
    public:
        
        Wtf() = default;
        Wtf(const Msg &msg) : Msg(msg) {}

        virtual Wtf *clone() const override {
            return new Wtf(*this);
        }
        
    };
    
    class ICanHaz : public Msg {
        
    public:
        
        ICanHaz() = default;
        ICanHaz(const Msg &msg) : Msg(msg) {}
        
        virtual ICanHaz *clone() const override {
            return new ICanHaz(*this);
        }
        
    };
    
    class CheezBurger : public Msg {
        
    public:
        
        CheezBurger() = default;
        CheezBurger(const Msg &msg) : Msg(msg) {}
        
        virtual CheezBurger *clone() const override {
            return new CheezBurger(*this);
        }
        
    };
    
    class Hugz : public Msg {
        
    public:
        
        Hugz() = default;
        Hugz(const Msg &msg) : Msg(msg) {}
        
        virtual Hugz *clone() const override {
            return new Hugz(*this);
        }
        
    };
    
    class HugzOk : public Msg {
        
    public:
        
        HugzOk() = default;
        HugzOk(const Msg &msg) : Msg(msg) {}
        
        virtual HugzOk *clone() const override {
            return new HugzOk(*this);
        }
        
    };
    
} }

#endif /* defined(__Messaging__NomMessage__) */
