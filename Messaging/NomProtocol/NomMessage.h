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

    class Timeout : public Msg {

    public:

        Timeout() : Msg("TIMEOUT") {}

        Timeout(const Msg &msg) : Msg(msg) {}

    };

    class Ohai : public Msg {

    public:

        Ohai() : Msg("OHAI") {}

        Ohai(const Msg &msg) : Msg(msg) {}

    };

    class OhaiOk : public Msg {

    public:

        OhaiOk() : Msg("OHAI-OK") {}

        OhaiOk(const Msg &msg) : Msg(msg) {}

    };

    class Wtf : public Msg {

    public:

        Wtf() : Msg("WTF") {}

        Wtf(const Msg &msg) : Msg(msg) {}

    };

    class ICanHaz : public Msg {

    public:

        ICanHaz() : Msg("ICANHAZ") {}

        ICanHaz(const Msg &msg) : Msg(msg) {}

    };

    class CheezBurger : public Msg {

    public:

        CheezBurger() : Msg("CHEEZBURGER") {}

        CheezBurger(const Msg &msg) : Msg(msg) {}

    };

    class Hugz : public Msg {

    public:

        Hugz() : Msg("HUGZ") {}

        Hugz(const Msg &msg) : Msg(msg) {}

    };

    class HugzOk : public Msg {

    public:

        HugzOk() : Msg("HUGZ-OK") {}

        HugzOk(const Msg &msg) : Msg(msg) {}

    };

} }

#endif /* defined(__Messaging__NomMessage__) */
