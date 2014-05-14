//
//  NomSocket.h
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__NomSocket__
#define __Messaging__NomSocket__

#include "NomCodec.h"

#include "Socket.h"

namespace Messaging { namespace NomProtocol {

    class NomSocket {

    public:

        NomSocket(const Context &context,
                  const Socket::Type &type,
                  const NomCodec::Address address,
                  const NomCodec::Envelope envelope)
        :
            _socket(context, type),
            _codec(address, envelope)
        {}

        void bind(const std::string &endpoint) {

            _socket.bind(endpoint);

        }

        void unbind(const std::string &endpoint) {

            _socket.unbind(endpoint);

        }

        void connect(const std::string &endpoint) {

            _socket.connect(endpoint);

        }

        void disconnect() {

            _socket.disconnect();

        }

        template <class T>
        void send(T &&msg) {

            _codec.encode(_socket, std::forward<T>(msg), Frame::more::none);

        }

        std::shared_ptr<Msg> receive() {

            auto more = false;

            do {

                Frame frame;

                frame.receive(_socket, Frame::block::blocking);

                more = frame.hasMore();

                auto msg = _codec.decode(frame);

                if (msg) {

                    return msg;

                }

            } while (more);

            return nullptr;

        }

        const Socket &socket() const {
            return _socket;
        }

    private:

        Socket _socket;
        NomCodec _codec;

    };

} }

#endif /* defined(__Messaging__NomSocket__) */
