//
//  Customer.h
//  Messaging
//
//  Created by Dave Meehan on 25/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#ifndef __Messaging__Customer__
#define __Messaging__Customer__

#include "Message.h"

#include <map>
#include <string>
#include <sstream>

namespace ATM {

    using namespace Messaging;
    
    class MapMessage : public std::map<std::string, std::string> {
        
    public:

        size_t send(Socket &socket, const Message::block block_type = Message::block::blocking) {
        
            Message msg;
            
            for( auto &pair : *this ) {

                msg.push_back(Frame(pair.first));
                msg.push_back(Frame(pair.second));
                
            }
            
            return msg.send(socket, block_type);
            
        }
        
        size_t receive(Socket &socket, const Message::block block_type = Message::block::blocking) {
            
            clear();
            
            Message msg;
            
            auto size = msg.receive(socket, block_type);
            
            while (msg.frames() > 1) {

                auto key = std::string(msg.front().data<char>(), msg.front().size());
                msg.pop_front();
                
                auto value = std::string(msg.front().data<char>(), msg.front().size());
                msg.pop_front();
                
                (*this)[key] = value;

            }
            
            return size;
        }

    };
    
    class ATMAgent {
        
    public:
        
        enum class Instruction {
            InsertCard,
            InputPIN,
            InputAmount,
            TakeCash,
            Complete
        };
        
        ATMAgent(Context &ctx)
        :
            _done(false),
            _agent(ctx, Socket::Type::reply)
        {}
        
        void operator()() {
            
            _agent.bind("inproc://atm");
            _agent.setReceiveTimeout(100);
            
            while (! _done) {
                
                MapMessage request;
                
                try {
                    
                    request.receive(_agent);
                    
                } catch (Exception &e) {
                    
                    if (e.errorCode() == EAGAIN) {
                        
                        continue;
                    }
                    
                    throw;
                    
                }
                
                dispatch(request);
                
            }
        }

        void dispatch(const MapMessage &request) {
            
            std::string sessionID;
            
            auto found = request.find("sessionID");
            if (found != request.end()) {
                sessionID = found->second;
            }
            
            if (sessionID.size() < 1) {
                std::ostringstream os;
                std::hash<int> hash;
                os << hash(rand());
                sessionID = os.str();
            }
            
            auto &data = getSessionData(sessionID);
            
            if (request.count("card")) {
                data.card = request.at("card");
            }
            
            if (request.count("pin")) {
                data.pin = request.at("pin");
            }
            
            if (request.count("amount")) {
                int amount = 0;
                std::istringstream(request.at("amount")) >> amount;
                data.amount = data.cash = amount;
            }
            
            if (request.count("cash")) {
                data.cash = 0;
            }
            
            MapMessage reply;
            
            if (data.card.size() == 0) {
                data.instruction = Instruction::InsertCard;
            } else if (data.pin.size() == 0) {
                data.instruction = Instruction::InputPIN;
            } else if (data.amount == 0) {
                data.instruction = Instruction::InputAmount;
            } else if (data.cash == data.amount) {
                data.instruction = Instruction::TakeCash;
            } else {
                data.instruction = Instruction::Complete;
            }
            
            reply["sessionID"] = sessionID;
            
            std::ostringstream os;
            os << static_cast<int>(data.instruction);
            
            reply["instruction"] = os.str();
            
            reply.send(_agent);
            

        }
        
        void stop() {
            _done = true;
        }
        
    protected:
        
        class data {

        public:
            
            data()
            :
                instruction(Instruction::InsertCard),
                amount(0),
                cash(0)
            {}
            
            Instruction instruction;
            std::string card;
            std::string pin;
            unsigned amount;
            unsigned cash;
        };
        
        data &getSessionData(const std::string &sessionID) {
            
            return _sessionData[sessionID];
            
        }
    private:
        
        bool _done;
        Socket _agent;
        
        std::map<std::string, data> _sessionData;
        
    };
    

    class ATM {

    public:
        
        ATM(const Context &ctx)
        :
            _atm(ctx, Socket::Type::request)
        {
            _atm.connect("inproc://atm");

            MapMessage msg;
            
            msg["sessionID"] = _sessionID;
            
            msg.send(_atm);
            
        }
        
        using Instruction = ATMAgent::Instruction;
        
        Instruction getInstruction() {
            
            MapMessage msg;
            
            msg.receive(_atm);
            
            _sessionID = msg["sessionID"];
            
            int i = 0;
            std::istringstream(msg["instruction"]) >> i;
            
            return static_cast<Instruction>(i);
        }
        
        void insertCard(const std::string &card) {
            
            MapMessage reply;
            
            reply["sessionID"] = _sessionID;
            reply["card"] = card;
            
            reply.send(_atm);
            
        }
        
        void inputAmount(const unsigned amount) {
            
            MapMessage reply;
            
            reply["sessionID"] = _sessionID;

            std::ostringstream os;
            os << amount;
            reply["amount"] = os.str();
            
            reply.send(_atm);
            
        }
        
        void inputPIN(const std::string &pin) {
            
            MapMessage reply;
            
            reply["sessionID"] = _sessionID;
            reply["pin"] = pin;
            
            reply.send(_atm);
            
        }
        
        void getCash(unsigned amount) {

            MapMessage reply;
            
            reply["sessionID"] = _sessionID;
            std::ostringstream os;
            os << amount;
            reply["cash"] = os.str();
            
            reply.send(_atm);
            
        }
        
    private:
        
        Context _ctx;
        Socket _atm;
        std::string _sessionID;
        
    };
    
    class Customer {
        
    public:
        
        Customer() : _done(false) {}
        
        void operator() (const Context ctx, const unsigned amount, const std::function<void()> &completion) {

            ATM atm(ctx);
            
            while (! _done) {
                
                switch (atm.getInstruction()) {
                        
                    case ATM::Instruction::InsertCard:
                        
                        atm.insertCard("01234567890");
                        break;
                        
                    case ATM::Instruction::InputAmount:
                        
                        atm.inputAmount(amount);
                        break;
                        
                    case ATM::Instruction::InputPIN:
                        
                        atm.inputPIN("1234");
                        break;
                        
                    case ATM::Instruction::TakeCash:
                        
                        atm.getCash(amount);
                        break;
                        
                    case ATM::Instruction::Complete:
                        
                        _done = true;

                        if (completion) {
                            completion();
                        }
                        
                        break;
                        
                    default:
                        break;
                }

            }
            

        }

    private:
        bool _done;
    };
}

#endif /* defined(__Messaging__Customer__) */
