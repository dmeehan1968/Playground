//
//  ATM.h
//  ATM
//
//  Created by Dave Meehan on 08/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ATM__ATM__
#define __ATM__ATM__

#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <iostream>

class ATM {
    
public:
    
    ATM()
    :
        _cancelled(false)
    {}
    
    ~ATM() {
        cancel();
        
        if (_thread.joinable()) {
            _thread.join();
        }
    }
    
    class atm_message {
        
    public:
        
        template <class T, class F>
        atm_message &handle(F f) {
            auto p = dynamic_cast<T*>(this);
            if (p) {
                f(*p);
            }
            return *this;
        }

        virtual ~atm_message() {}
        
        virtual std::shared_ptr<atm_message> clone() const {
            return std::make_shared<atm_message>(*this);
        }
    };
    
    class insert_card : public atm_message {
        
    public:
        virtual std::shared_ptr<atm_message> clone() const {
            return std::make_shared<insert_card>(*this);
        }
        
    };
    
    class digit : public atm_message {
    
    public:
        digit(unsigned value) : _value(value) {}

        unsigned value() const {
            return _value;
        }
        
        virtual std::shared_ptr<atm_message> clone() const {
            return std::make_shared<digit>(*this);
        }
        
    private:
        unsigned _value;

    };
    
    
    ATM &operator << (const atm_message &msg) {
        
        _queue.push(std::shared_ptr<atm_message>(msg.clone()));
        
        return *this;
    }
    
    void run() {
        
        _thread = std::thread([&]{ (*this)(); });
        
    }
    
protected:
    
    void operator() () {
        
        while ( ! _cancelled) {
            
            auto msg = wait();
            
            if (msg) {
                (*msg).handle<insert_card>([&](insert_card const &msg) {
                    
                    std::cout << "Card Inserted" << std::endl;
                    
                })
                .handle<digit>([&](digit const &msg) {
                   
                    std::cout << "Digit = " << msg.value() << std::endl;
                    
                });
            }
            
        }
    }
    
    std::shared_ptr<atm_message> wait() {
        
        if (_queue.empty()) {
            return nullptr;
        }
        
        auto result = _queue.front();
        _queue.pop();
        return result;
        
    }
    
    void cancel() {
        _cancelled = true;
    }
    
private:
    
    bool _cancelled;
    std::queue<std::shared_ptr<atm_message>> _queue;
    std::thread _thread;
    
};


#endif /* defined(__ATM__ATM__) */
