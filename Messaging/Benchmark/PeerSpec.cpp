//
//  PeerSpec.cpp
//  Messaging
//
//  Created by Dave Meehan on 05/05/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <ZingBDD/ZingBDD.h>

#include "Message.h"

#include <thread>
#include <iomanip>

namespace Messaging { namespace Benchmark {

    class Server {
        
    public:
        Server(const Context &ctx)
        :
            _ctx(ctx),
            _done(false),
            _maxConcurrentClients(0),
            _totalClients(0),
            _successClients(0)
        {}
        
        void operator() (const size_t expectedClients, const size_t messageBytes, const size_t expectedBytes) {
            
            Socket server(_ctx, Socket::Type::stream);
            
            server.setReceiveTimeout(1000);
            server.setBacklog(2000);
            
            server.bind("tcp://*:5555");
            
            std::map<std::string, size_t> connectionBytes;
            
            auto start = std::chrono::high_resolution_clock::now();
            size_t totalMessages = 0;
            size_t totalBytes = 0;
            const std::string okay("OK");
            
            while (!_done) {
                
                Frame connectionId;
                Frame data;
                
                try {
                    
                    connectionId.receive(server, Frame::block::blocking);
                    
                    if (connectionId.hasMore()) {
                        
                        data.receive(server, Frame::block::none);
                        
                    } else {
                        
                        std::cout << "invalid id" << std::endl;
                    }
                    
                } catch (Exception &e) {
                    
//                    std::cout << "Server receive failed: " << zmq_strerror(e.errorCode()) << std::endl;

                    if (e.errorCode() == EAGAIN) {
                        
                        continue;
                    }
                    
                    throw;
                }
                
                auto found = connectionBytes.find(connectionId.str());
                
                if (data.size() > 0) {

//                    std::cout << data.size() << "-" << data.str() << std::endl;

                    totalMessages++;
                    
                    if (found != connectionBytes.end()) {
                        
                        found->second += data.size();
                        
                    } else {
                        
                        std::cout << "data without previous connection" << std::endl;
                        
                    }
                    
                    Frame(connectionId).send(server, Frame::block::none, Frame::more::more);
                    Frame(okay).send(server, Frame::block::none, Frame::more::none);
                    
                } else {
                    
                    if (found == connectionBytes.end()) {
                        
                        _totalClients++;
                        
                        // connect
                        connectionBytes[connectionId.str()] = 0;
                        
                        if (connectionBytes.size() > _maxConcurrentClients) {
                            _maxConcurrentClients = connectionBytes.size();
                        }
                        
                    } else {
                        
                        if (found->second >= expectedBytes) {
                            _successClients++;
                            
                        } else {
                        
                            std::cout << found->second << " of " << expectedBytes << std::endl;
                            
                        }
                        
                        totalBytes += found->second;

                        // disconnect
                        connectionBytes.erase(found);
                        
                    }

                }
                
            }
            
            auto runtime = std::chrono::high_resolution_clock::now() - start;
            
            auto duration = (double) runtime.count() / 1000000000;
            
            auto mps = (double)totalMessages / duration;
            
            auto bps = (double)totalBytes / duration;
            
            std::cout << _successClients << "/" << expectedClients << ", " << messageBytes << ", " << mps << ", " << std::setprecision(10) << bps << std::endl;
            
        }
        
        void stop() {
            _done = true;
        }
        
        size_t maxConcurrentClients() const {
            return _maxConcurrentClients;
        }
        
        size_t totalClients() const {
            return _totalClients;
        }
        
        size_t successClients() const {
            return _successClients;
        }
        
    private:
        
        Context _ctx;
        bool _done;
        size_t _maxConcurrentClients;
        size_t _totalClients;
        size_t _successClients;
        
    };
    
    class Client {
        
        
    public:
        Client(const Context &ctx, const size_t messageBytes, const size_t totalBytes)
        :
            _ctx(ctx),
            _server(_ctx, Socket::Type::stream),
            _state(&Client::connecting),
            _sent(0),
            _totalBytes(totalBytes)
        {
            _data.resize(messageBytes, '0');
        }
        
        using State = void (Client::*)();
        
        bool run() {

            if (_state != nullptr) {
                
                (this->*_state)();
                
            }
            
            return _state == nullptr ? true : false;
            
        }
        
        void connecting() {
            
            _server.connect("tcp://127.0.0.1:5555");
            
            _state = &Client::receiveConnId;
            
        }
        
        void receiveConnId() {

            Message connect;
            
            connect.receive(_server, Message::block::blocking);
            
            _connectionId = connect.envelope().front().str();

            _state = &Client::sendData;
            
        }
        
        void sendData() {

            Message msg;
            
            msg.envelope().push_back(_connectionId);
            msg.emplace_back(_data);
            
            _sent += msg.send(_server);
            
            _state = &Client::receiveOk;
            
        }
        
        void receiveOk() {
            
            Message ok;
            
            ok.receive(_server);
            
            _state = &Client::terminate;

            if (ok.envelope().front().str() != _connectionId) {
                std::cout << "Expected ID " << _connectionId << ", got " << ok.envelope().front().str() << std::endl;
            }
            
            if (ok.front().str() != "OK") {
                std::cout << "Expected OK, got " << ok.front().str() << std::endl;
            }

            if (_sent < _totalBytes) {
                
                _state = &Client::sendData;
                
            }
            
        }
     
        void terminate() {
            
            _state = nullptr;
            
        }
        
    private:
        
        Context _ctx;
        Socket _server;
        State _state;
        std::string _connectionId;
        size_t _sent;
        size_t _totalBytes;
        std::string _data;
    };
    
    
    class Worker {
        
    public:
        
        void operator()() {
            
            while (_clients.size() > 0) {

                for (auto iter = _clients.begin(); iter != _clients.end(); iter++) {

                    if (iter->run()) {
                        
                        _clients.erase(iter);
                        break;
                        
                    }
                    
                }

            }
            
        }
        
        void push_back(const Client &client) {
            
            _clients.push_back(client);
            
        }
        
        size_t size() const {
            
            return _clients.size();
            
        }
        
    private:
        
        std::vector<Client> _clients;
        
    };
    
    class Test {
        
    public:
        
        size_t operator()(const size_t clients, const size_t messageBytes) {
            
            Context ctx;
//            ctx.setIOThreads(1);
            ctx.setMaxSockets(3000);
            
            auto server = Server(ctx);
            auto totalBytes = 1024 * 1024;
            
            auto serverThread = std::thread(std::ref(server), clients, messageBytes, totalBytes);
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
            std::array<Worker, 2> workers;
            
            // Assign clients to workers
            
            for(unsigned i = 0 ; i < clients ; i++) {
                
                workers[i % workers.size()].push_back(Client(ctx, messageBytes, totalBytes));
                
            }
            
            // Create threads for the workers
            
            std::vector<std::thread> workerThreads;
            
            for (auto &worker : workers) {
                
                workerThreads.emplace_back(std::ref(worker));
                
            }
            
            // wait for them to complete
            
            for (auto &thread : workerThreads) {
                
                if (thread.joinable()) {
                    
                    thread.join();
                    
                }
            }

            // short sleep to allow connections to close so server can count them as complete
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            // stop the server and wait for completion
            
            server.stop();
            serverThread.join();
            
            return server.successClients();
            
        }
    };

    describe(Peer, {
       
        std::cout << "nClients,MsgSize,Msg/sec,Bytes/sec" << std::endl;
        
        for(int clients=4 ; clients <= 16 ; clients *= 2) {
            
            for (size_t bytes = 64 ; bytes <= 64 ; bytes *= 2) {
                
                size_t actual;
                
                if ((actual = Test()(clients, bytes)) != clients) {
                    std::cout << "ERROR, expected " << clients << ", got " << actual << std::endl;
                }
                
            }

        }

    });
    
} }

