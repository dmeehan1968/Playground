//
//  NomSession.h
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__NomSession__
#define __Messaging__NomSession__

#include "NomMessage.h"
#include "NomSocket.h"
#include "Dispatch.h"

#include <memory>
#include <iostream>

namespace Messaging { namespace NomProtocol {

    using namespace ::Messaging::Protocol;

    class NomSession {

    public:

        using Clock = std::chrono::steady_clock;
        using Milliseconds = std::chrono::milliseconds;

        NomSession(const NomSocket &socket, const Milliseconds &timeout)
        :
            _socket(socket),
            _state(State::OpenPeering),
            _numCheezBurgers(10),
            _timeout(timeout)
        {
            resetTimeout();

            onOpenPeeringOhai = [](const Ohai &) {};
            onUsePeeringICanHaz = [](const ICanHaz &) {};
            onUsePeeringHugz = [](const Hugz &) {};
            onUsePeeringTimeout = [](const Timeout &) {};

            init();
        }

        void init();

        void dispatch(const std::shared_ptr<Msg> &msg) {

            if (msg) {

                _replyAddress = msg->address;

                resetTimeout();

            }

            switch (_state) {

                case State::OpenPeering:
                {

                    Dispatch<Msg>(*msg).handle<Ohai>([&](const Ohai &ohai) {

                        _state = State::UsePeering;
                        onOpenPeeringOhai(ohai);

                    });

                    break;
                }

                case State::UsePeering:
                {

                    Dispatch<Msg>(*msg).handle<ICanHaz>([&](const ICanHaz &iCanHaz) {

                        onUsePeeringICanHaz(iCanHaz);

                    }).handle<Hugz>([&](const Hugz &hugz) {

                        onUsePeeringHugz(hugz);

                    }).handle<Timeout>([&](const Timeout &timeout) {

                        onUsePeeringTimeout(timeout);

                    });

                    break;
                }
                default:
                    break;
            }
        }

        std::function<void(const Ohai &)> onOpenPeeringOhai;
        std::function<void(const ICanHaz &)> onUsePeeringICanHaz;
        std::function<void(const Hugz &)> onUsePeeringHugz;
        std::function<void(const Timeout &)> onUsePeeringTimeout;

        std::shared_ptr<Msg> hugz() {

            return nullptr;

        }

        bool isExpired() const {

            auto now = Clock::now();
            return _expires < now;

        }

        void resetTimeout() {

            auto now = Clock::now();

            auto limit = std::chrono::duration_cast<Milliseconds>(Clock::time_point::duration::max() - now.time_since_epoch());

            _expires = now + ((limit < _timeout) ? limit : _timeout);

        }

    protected:

        template <class T>
        void reply(T &&msg) {

            msg.address = _replyAddress;
            _socket.send(std::forward<T>(msg));

        }

    private:

        enum class State {

            OpenPeering,
            UsePeering

        };

        State       _state;
        NomSocket   _socket;
        Frame       _replyAddress;

        Clock::time_point   _expires;
        Milliseconds        _timeout;

        unsigned    _numCheezBurgers;

    };

} }

#endif /* defined(__Messaging__NomSession__) */
