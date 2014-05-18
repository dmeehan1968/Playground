//
//  NomSession.cpp
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include "NomSession.h"

namespace Messaging { namespace NomProtocol {

    void NomSession::init() {

        onOpenPeeringOhai = [&](const Ohai &ohai) {

            if (_numCheezBurgers) {

                send(OhaiOk());

            }
        };

        onUsePeeringICanHaz = [&](const ICanHaz &iCanHaz) {

            if (_numCheezBurgers-- > 0) {

                send(CheezBurger());

            } else {

                send(Wtf());

            }

        };

        onUsePeeringHugz = [&](const Hugz &hugz) {

            send(HugzOk());

        };

        onUsePeeringTimeout = [&](const Hugz &hugz) {

            send(Hugz());

        };

    }

} }