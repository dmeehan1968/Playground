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

                reply(OhaiOk());

            }
        };

        onUsePeeringICanHaz = [&](const ICanHaz &iCanHaz) {

            if (_numCheezBurgers-- > 0) {

                reply(CheezBurger());

            } else {

                reply(Wtf());

            }

        };

        onUsePeeringHugz = [&](const Hugz &hugz) {

            reply(HugzOk());

        };

    }

} }