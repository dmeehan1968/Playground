//
//  Msg.h
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__Msg__
#define __Messaging__Msg__

#include "Frame.h"

#include <ostream>
#include <deque>

namespace Messaging { namespace Protocol {

    class Msg {

    public:

        enum class Address {

            Use,
            Ignore

        };

        enum class Envelope {

            Use,
            Ignore

        };

        Msg()
        :
            _state(State::Address)
        {}

        Msg(const Address &address, const Envelope &envelope) {

            _state = State::Address;

            if (address == Address::Ignore) {
                _state = State::Envelope;
            }

            if (envelope == Envelope::Ignore) {
                _state = State::Identity;
            }
        }

        virtual void encode(Socket &socket,
                            const Address address_type,
                            const Envelope envelope_type,
                            const Frame::more more_type) {

            auto blocking = Frame::block::blocking;
            auto more = Frame::more::more;

            if (address_type == Address::Use) {

                address.send(socket, blocking, more);

            }

            if (envelope_type == Envelope::Use) {

                Frame().send(socket, blocking, more);
            }

            identity.send(socket, blocking, more_type);

        }

        virtual bool decode(const Frame &frame) {

            switch (_state) {

                case State::Address:

                    address = frame;
                    _state = State::Envelope;
                    break;

                case State::Envelope:

                    _state = State::Identity;
                    break;

                case State::Identity:

                    identity = frame;
                    _state = State::_Final;
                    break;

                case State::_Final:

                    throw std::runtime_error("invalid frame for AbstractMsg");

            }

            return isFinal();

        }

        virtual bool isFinal() const {

            return _state == State::_Final;

        }

        Frame address;
        Frame identity;

        virtual bool operator == (const Msg &msg) const {

            return address == msg.address && identity == msg.identity;

        }

        virtual bool operator != (const Msg &msg) const {

            return ! operator == (msg);
        }

        template <class T>
        bool isa() {

            try {

                (void)dynamic_cast<T*>(this);

            } catch (std::bad_cast &) {

                return false;

            }

            return true;

        }

    private:

        enum class State {

            Address,
            Envelope,
            Identity,
            _Final

        };

        State _state;

    };

    inline std::ostream &operator << (std::ostream &stream, const Msg &msg) {

        return stream << "Msg: identity=" << msg.identity.str();

    }

} }



#endif /* defined(__Messaging__Msg__) */
