//
//  NomClient.cpp
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include "NomClient.h"

namespace Messaging { namespace NomProtocol {

    void NomClient::init() {

        onOpenPeeringHugz = [&](const Hugz &hugz) {

            send(HugzOk());

        };

        onUsePeeringHugz = [&](const Hugz &hugz) {

            send(HugzOk());

        };

    }

} }