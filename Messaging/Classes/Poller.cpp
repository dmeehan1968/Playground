//
//  Poller.cpp
//  Messaging
//
//  Created by Dave Meehan on 21/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include "Poller.h"

namespace Messaging {

    constexpr Poller::Events Poller::Events::None(0);
    constexpr Poller::Events Poller::Events::Readable(ZMQ_POLLIN);
    constexpr Poller::Events Poller::Events::Writable(ZMQ_POLLOUT);
    constexpr Poller::Events Poller::Events::Error(ZMQ_POLLERR);
    constexpr Poller::Events Poller::Events::All(ZMQ_POLLIN | ZMQ_POLLOUT | ZMQ_POLLERR);

}