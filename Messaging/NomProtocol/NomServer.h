//
//  NomServer.h
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__NomServer__
#define __Messaging__NomServer__

#include "NomSession.h"
#include "NomSocket.h"

#include "Msg.h"

#include "Frame.h"
#include "Reactor.h"

#include <memory>
#include <map>
#include <thread>


namespace Messaging { namespace NomProtocol {

    using namespace ::Messaging::Protocol;

    class NomServer {

    public:

        NomServer(const Context &context,
                  const std::shared_ptr<Reactor> reactor,
                  const NomSession::Milliseconds &timeout = NomSession::Milliseconds(60000),
                  const std::string &endpoint = std::string("inproc://nom"))
        :
            _socket(context,
                    Socket::Type::router,
                    NomCodec::Address::Use,
                    NomCodec::Envelope::Use),
            _reactor(reactor),
            _timeout(timeout),
            _endpoint(endpoint)
        {

            _socket.bind(_endpoint);

            using namespace std::placeholders;

            _reactor->addObserver(_socket.socket(), Reactor::Event::Readable, std::bind(&NomServer::onSocketReadable, this, _1, _2, _3), Reactor::Milliseconds(100));

        }

        ~NomServer() {

            _reactor->removeObserver(_socket.socket(), Reactor::Event::Readable);

            try {

                _socket.unbind(_endpoint);

            } catch (Exception &e) {

                if (e.errorCode() != ENOENT) {
                    throw;
                }
            }
        }

    protected:

        void onSocketReadable(const Socket &socket, const Reactor::Event &event, const bool didTimeout) {

            if (didTimeout) {

                for ( auto &iter : _sessions ) {

                    auto session = iter.second;

                    if (session.isExpired()) {

                        auto timeout = std::make_shared<Timeout>();
                        timeout->address = iter.first;
                        session.dispatch(std::make_shared<Timeout>());

                    }
                }

                return;
            }

            auto msg = _socket.receive();

            if (msg) {

                auto found = _sessions.find(msg->address);

                if (found == _sessions.end()) {

                    found = _sessions.emplace(std::piecewise_construct, std::make_tuple(msg->address), std::make_tuple(_socket, _timeout)).first;

                }

                found->second.dispatch(msg);

            }
        }

    private:

        std::map<Frame, NomSession> _sessions;

        NomSocket _socket;
        std::shared_ptr<Reactor> _reactor;
        std::string _endpoint;

        NomSession::Milliseconds _timeout;

    };

} }

#endif /* defined(__Messaging__NomServer__) */
