//
//  Dispatcher.h
//  ATM
//
//  Created by Dave Meehan on 16/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ATM__Dispatcher__
#define __ATM__Dispatcher__

#include <memory>
#include <functional>

namespace Messaging {
    
    template <class T>
    class Dispatcher {
        
    public:
        Dispatcher(const std::shared_ptr<T> msg)
        :
        _msg(msg),
        _handled(false)
        {}
        
        template <class M>
        Dispatcher &handle(const std::function<bool(M&)> &func) {
            
            if ( ! _handled) {
                
                auto specific = std::dynamic_pointer_cast<M>(_msg);
                
                if (specific) {
                    _handled = func(*specific);
                }
                
            }
            
            return *this;
        }
        
        Dispatcher &handle(const std::function<bool()> &func) {
            
            if (!_handled) {
                _handled = func();
            }
            return *this;
            
        }
        
    private:
        
        std::shared_ptr<T> _msg;
        bool _handled;
        
    };
    
}
#endif /* defined(__ATM__Dispatcher__) */
