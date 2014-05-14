//
//  NomProtocolSpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "NomServer.h"
#include "NomClient.h"

namespace Messaging { namespace NomProtocol { namespace Specs {

    describe(NomProtocol, {

        Context context;
        auto reactor = std::make_shared<Reactor>();

        NomServer server(context, reactor);
        NomClient client(context);

        reactor->start();

        it("opens peering", {

            auto reply = client.ohai();

            expect(reply && reply->isa<Ohai>()).should.beTrue();

        });

        it("gets cheezburger", {

            auto reply = client.iCanHaz();

            expect(reply && reply->isa<CheezBurger>()).should.beTrue();

        });

        it("gets hugz", {

            auto reply = client.hugz();

            expect(reply && reply->isa<HugzOk>()).should.beTrue();

        });

        reactor->stop();

    });

} } }
