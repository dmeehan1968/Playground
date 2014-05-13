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
    
    class Ohai : public Msg {};
    class OhaiOk : public Msg {};
    class Wtf : public Msg {};
    class ICanHaz : public Msg {};
    class CheezBurger : public Msg {};
    class Hugz : public Msg {};
    class HugzOk : public Msg {};
    
} }

#endif /* defined(__Messaging__NomMessage__) */
