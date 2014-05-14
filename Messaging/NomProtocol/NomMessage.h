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

        Ohai() {
            identity = "OHAI";
        }

        Ohai(const Msg &msg) : Msg(msg) {}

    };

    class OhaiOk : public Msg {

    public:

        OhaiOk() {
            identity = "OHAI-OK";
        }

        OhaiOk(const Msg &msg) : Msg(msg) {}

    };

    class Wtf : public Msg {

    public:

        Wtf() {
            identity = "WTF";
        }

        Wtf(const Msg &msg) : Msg(msg) {}

    };

    class ICanHaz : public Msg {

    public:

        ICanHaz() {
            identity = "ICANHAZ";
        }

        ICanHaz(const Msg &msg) : Msg(msg) {}

    };

    class CheezBurger : public Msg {

    public:

        CheezBurger() {
            identity = "CHEEZBURGER";
        }

        CheezBurger(const Msg &msg) : Msg(msg) {}

    };

    class Hugz : public Msg {

    public:

        Hugz() {
            identity = "HUGZ";
        }

        Hugz(const Msg &msg) : Msg(msg) {}

    };

    class HugzOk : public Msg {

    public:

        HugzOk() {
            identity = "HUGZ-OK";
        }

        HugzOk(const Msg &msg) : Msg(msg) {}

    };

} }

#endif /* defined(__Messaging__NomMessage__) */
