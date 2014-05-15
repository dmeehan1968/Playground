//
//  NomClient.h
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__NomClient__
#define __Messaging__NomClient__

#include "NomMessage.h"
#include "NomSocket.h"
#include "Dispatch.h"

#include "Reactor.h"

#include <memory>

namespace Messaging { namespace NomProtocol {

    class NomClient {

    public:

        NomClient(const Context &context,
                  const std::shared_ptr<Reactor> reactor,
                  const std::string &endpoint = std::string("inproc://nom"))
        :
            _socket(context,
                    Socket::Type::dealer,
                    NomCodec::Address::Ignore,
                    NomCodec::Envelope::Use),
            _reactor(reactor),
            _endpoint(endpoint),
            _state(State::OpenPeering),
            _lastRequest(NomCodec::MsgType::_None)
        {
            _socket.connect(_endpoint);

            onOpenPeeringHugz = [](const Hugz &){};
            onUsePeeringHugz = [](const Hugz &){};

            init();

        }

        void init();

        std::shared_ptr<Msg> ohai() {

            send(Ohai());

            return receive();

        }

        std::shared_ptr<Msg> iCanHaz() {

            send(ICanHaz());

            return receive();

        }

        std::shared_ptr<Msg> hugz() {

            send(Hugz());

            return receive();

        }

        std::function<void(const Hugz &)> onOpenPeeringHugz;
        std::function<void(const Hugz &)> onUsePeeringHugz;

    protected:

        template <class T>
        void send(T &&msg) {

            switch (_state) {

                case State::OpenPeering:
                {

                    if (std::is_base_of<Ohai, T>::value) {

                        _socket.send(std::forward<T>(msg));
                        _lastRequest = NomCodec::MsgType::Ohai;

                    } else if (std::is_base_of<HugzOk, T>::value) {

                        _socket.send(std::forward<T>(msg));

                    }
                    break;
                }

                case State::UsePeering:
                {

                    if (std::is_base_of<ICanHaz, T>::value) {

                        _socket.send(std::forward<T>(msg));
                        _lastRequest = NomCodec::MsgType::ICanHaz;

                    } else if (std::is_base_of<Hugz, T>::value) {

                        _socket.send(std::forward<T>(msg));
                        _lastRequest = NomCodec::MsgType::Hugz;

                    } else if (std::is_base_of<HugzOk, T>::value) {

                        _socket.send(std::forward<T>(msg));

                    }

                    break;
                }

            }
        }

        std::shared_ptr<Msg> receive() {

            do {

                auto reply = _socket.receive();

                if ( ! reply ) {
                    break;
                }

                switch (_state) {

                    case State::OpenPeering:
                    {
                        switch (_lastRequest) {

                            case NomCodec::MsgType::_None:
                                break;

                            case NomCodec::MsgType::Ohai:
                            {
                                if (reply->isa<OhaiOk>()) {

                                    _lastRequest = NomCodec::MsgType::_None;
                                    _state = State::UsePeering;
                                    return reply;
                                }

                                if (reply->isa<Wtf>()) {

                                    _lastRequest = NomCodec::MsgType::_None;
                                    return reply;

                                }
                                break;
                            }

                            default:
                                errno = EFSM;
                                throw Exception("invalid last request");
                        }

                        Dispatch<Msg>(*reply).handle<Hugz>([&](const Hugz &hugz) {

                            onOpenPeeringHugz(hugz);

                        }).handle<Msg>([&](const Msg &msg) {

                            throw Exception("invalid message");

                        });

                        break;
                    }

                    case State::UsePeering:
                    {
                        switch (_lastRequest) {

                            case NomCodec::MsgType::_None:
                                break;

                            case NomCodec::MsgType::ICanHaz:
                            {

                                if (reply->isa<CheezBurger>() ||
                                    reply->isa<Wtf>() ||
                                    reply->isa<Hugz>()) {

                                    return reply;

                                }
                                break;
                            }

                            case NomCodec::MsgType::Hugz:
                            {

                                if (reply->isa<HugzOk>()) {

                                    return reply;

                                }
                                break;
                            }

                            default:
                                errno = EFSM;
                                throw Exception("invalid last request");
                        }

                        Dispatch<Msg>(*reply).handle<Hugz>([&](const Hugz &hugz) {

                            onUsePeeringHugz(hugz);

                        }).handle<Msg>([&](const Msg &msg) {

                            throw Exception("invalid message");

                        });

                        break;

                    }

                    default:
                        break;
                }


            } while (1);

            throw Exception("no message received, unexpected", 0);

        }

    private:

        enum class State {

            OpenPeering,
            UsePeering,

        };

        NomSocket _socket;
        std::shared_ptr<Reactor> _reactor;
        std::string _endpoint;
        State _state;
        NomCodec::MsgType _lastRequest;

    };

} }

#endif /* defined(__Messaging__NomClient__) */
