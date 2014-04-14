//
//  ATM.h
//  ATM
//
//  Created by Dave Meehan on 08/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __ATM__ATM__
#define __ATM__ATM__

#include <iostream>
#include <queue>
#include <map>

namespace Messaging {
    
    class Message {
        
    public:
        
        virtual ~Message() {}
        
    };
    
    class MessageQueue {
        
    public:
        
        std::shared_ptr<Message> pop() {
            
            std::unique_lock<std::mutex> lock(_mutex);
            
            _cv.wait(lock, [&]{
                
                return ! _queue.empty();
                
            });
            
            std::shared_ptr<Message> value = _queue.front();
            _queue.pop();
            
            return value;
        }
        
        template <class M>
        void push(M &&value) {
            
            std::lock_guard<std::mutex> lock(_mutex);
            
            _queue.push(std::make_shared<M>(std::forward<M>(value)));
            
            _cv.notify_one();
            
        }
        
    private:
        
        std::queue<std::shared_ptr<Message>> _queue;
        std::mutex _mutex;
        std::condition_variable _cv;
        
    };
    
    class Context {
      
    public:
        
        using shared_queue_type = std::shared_ptr<MessageQueue>;
        using uri = std::string;
        using queue_map = std::map<uri, shared_queue_type>;

        Context()
        :
            _queues(std::make_shared<queue_map>())
        {}
        
        shared_queue_type getQueue(const std::string &uri) {
            
            auto found = _queues->find(uri);
            
            if (found != _queues->end()) {
                return found->second;
            }
            
            return _queues->emplace(uri, std::make_shared<MessageQueue>()).first->second;
            
        }
        
    private:
        
        std::shared_ptr<queue_map> _queues;
        
    };
    
    class Dispatcher {

    public:
        
        Dispatcher(const std::shared_ptr<Message> &msg)
        :
            _msg(msg)
        {}
        
        template <class T>
        Dispatcher &handle(std::function<void(const T&)> func) {
            
            auto msg = std::dynamic_pointer_cast<T>(_msg);
            
            if (msg) {
                
                func(*msg);
                
            }
            
            return *this;
        }
        
    private:
        
        std::shared_ptr<Message> _msg;
        
    };
    
    class Socket {
        
    public:
        
        Socket() : _queue(std::make_shared<MessageQueue>()) {}
        
        Socket(Messaging::Context &ctx, const std::string &uri)
        :
            _queue(ctx.getQueue(uri))
        {}
        
        Dispatcher receive() {
            
            Dispatcher dispatcher( _queue->pop() );
            
            return dispatcher;
        }
        
        template <class T>
        void send(T &&msg) {
            
            _queue->push(std::forward<T>(msg));
            
        }
        
    private:

        std::shared_ptr<MessageQueue> _queue;
        
    };
    
}


class BANK {
    
public:
    
    class getAccount : public Messaging::Message {
      
    public:
        
        getAccount(const std::string &account) {}
        
    };
    
    class accountDetails : public Messaging::Message {
        
    public:
        
        const std::string &name() const {
            return _name;
        }
        
    private:
        
        std::string _name;
    };
    
};


class ATM {

public:
    
    ATM(const Messaging::Context &ctx)
    :
        _ctx(ctx)
    {}
    
    using state = void (ATM::*)();
    
    void operator()() {

        _state = &ATM::init;
        
        while (_state) {
            
            (this->*_state)();
            
        }
        
    }
    
    class card : public Messaging::Message {
        
    public:
        
        card(const std::string &account)
        :
            _account(account)
        {}
        
        const std::string &account() const {
            return _account;
        }
        
    private:
        
        std::string _account;
        
    };
    
    class pin : public Messaging::Message {
        
    public:
        
        pin(const std::string &number)
        :
            _number(number)
        {}
        
        const std::string &number() const {
            return _number;
        }
        
    private:
        
        std::string _number;
        
    };
    
protected:
    
    void init() {
        
        _customer = Messaging::Socket(_ctx, "shmem://atm");
        _bank = Messaging::Socket(_ctx, "shmem://bank");
        
        _state = &ATM::wait_card;
        
    }
    
    void wait_card() {
        
        std::cout << "Please insert your card" << std::endl;
        
        _customer.receive().handle<card>([&](const card &msg) {
            
            _bank.send(BANK::getAccount(msg.account()));
            
            _bank.receive().handle<BANK::accountDetails>([&](const BANK::accountDetails &msg) {

                std::cout << "Hello " << msg.name() << ", how are you today?" << std::endl;
                
            });
            
            _state = &ATM::wait_pin;
            
        });
    }
    
    void wait_pin() {
        
        std::cout << "Please enter your PIN" << std::endl;
        
        _customer.receive().handle<pin>([&](const pin &msg) {
            
            std::cout << "Validating PIN, please wait..." << std::endl;
            
            _state = nullptr;
            
        });
    }
    
private:
    
    state _state;
    Messaging::Context _ctx;
    Messaging::Socket _customer;
    Messaging::Socket _bank;
    
};


#endif /* defined(__ATM__ATM__) */
