//
//  Dispatch.h
//  Messaging
//
//  Created by Dave Meehan on 13/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__Dispatch__
#define __Messaging__Dispatch__

#include <functional>

namespace Messaging { namespace Protocol {

    template <class Base>
    class Dispatch {

    public:

        Dispatch(const Base &event)
        :
            _event(event),
            _handled(false)
        {}

        template <class T>
        Dispatch &handle(const std::function<void(const T&)> &functor) {

            if (_handled || ! functor) {
                return *this;
            }

            try {

                auto specific = dynamic_cast<const T&>(_event);
                functor(specific);
                _handled = true;

            } catch (std::bad_cast &) {

                // pass to next handler, if any

            }

            return *this;
        }

    private:

        const Base &_event;
        bool _handled;

    };

} }

#endif /* defined(__Messaging__Dispatch__) */
