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

namespace Messaging { namespace NomProtocol {

    using namespace ::Messaging::Protocol;

    class NomSession {

    public:

        NomSession(const NomSocket &socket)
        :
            _socket(socket),
            _state(State::OpenPeering),
            _numCheezBurgers(10)
        {}

        void dispatch(const std::shared_ptr<Msg> &msg) {

            if (msg) {
                _replyAddress = msg->address;
            }

            switch (_state) {

                case State::OpenPeering:
                {

                    Dispatch<Msg>(*msg).handle<Ohai>([&](const Ohai &ohai) {

                        _state = State::UsePeering;
                        OpenPeering(ohai);

                    });

                    break;
                }

                case State::UsePeering:
                {

                    Dispatch<Msg>(*msg).handle<ICanHaz>([&](const ICanHaz &iCanHaz) {

                        UsePeering(iCanHaz);

                    }).handle<Hugz>([&](const Hugz &hugz) {

                        UsePeering(hugz);

                    });

                    break;
                }
                default:
                    break;
            }
        }

        void OpenPeering(const Ohai &ohai) {

            if (_numCheezBurgers) {

                reply(OhaiOk());

            }
        }

        void UsePeering(const ICanHaz &iCanHaz) {

            if (_numCheezBurgers-- > 0) {

                reply(CheezBurger());

            } else {

                reply(Wtf());

            }

        }

        void UsePeering(const Hugz &hugz) {

            reply(HugzOk());

        }

        std::shared_ptr<Msg> hugz() {

            return nullptr;

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

        unsigned    _numCheezBurgers;

    };

} }

#endif /* defined(__Messaging__NomSession__) */
