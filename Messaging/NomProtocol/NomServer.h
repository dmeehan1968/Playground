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
                  const std::string &endpoint = std::string("inproc://nom"))
        :
            _socket(context,
                    Socket::Type::router,
                    NomCodec::Address::Use,
                    NomCodec::Envelope::Use),
            _reactor(reactor),
            _endpoint(endpoint)
        {

            _socket.bind(_endpoint);

            using namespace std::placeholders;

            _reactor->addObserver(_socket.socket(), Reactor::Event::Readable, std::bind(&NomServer::onSocketReadable, this, _1, _2));

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

        void onSocketReadable(Socket &socket, const Reactor::Event &event) {

            auto msg = _socket.receive();

            if (msg) {

                auto found = _sessions.find(msg->address);

                if (found == _sessions.end()) {

                    found = _sessions.emplace(msg->address, _socket).first;

                }

                found->second.dispatch(msg);

            }
        }

    private:

        std::map<Frame, NomSession> _sessions;

        NomSocket _socket;
        std::shared_ptr<Reactor> _reactor;
        std::string _endpoint;

    };

} }

#endif /* defined(__Messaging__NomServer__) */
