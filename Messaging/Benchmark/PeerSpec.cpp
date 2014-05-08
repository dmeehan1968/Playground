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
            _server(ctx, Socket::Type::stream),
            _done(false),
            _complete(0),
            _maxConcurrentClients(0)
        {}
        
        using State = void (Server::*)();
        
        void operator() () {
            
            _state = &Server::connecting;
            
            while (_state != nullptr && ! _done) {
                
                (this->*_state)();
                
            }
            
        }
        
        size_t complete() const {
            return _complete;
        }
        
        size_t maxConcurrentClients() const {
            return _maxConcurrentClients;
        }
        
        void stop() {
            _done = true;
        }

    protected:

        void connecting() {
        
            _server.setReceiveTimeout(100);
            _server.setBacklog(4000);
            _server.setSendBufferSize(1000000);
            _server.setReceiveBufferSize(1000000);
            _server.setSendHighWaterMark(1000000);
            _server.setReceiveHighWaterMark(1000000);
            
            _server.bind("tcp://*:5555");
            
            _state = &Server::receiveConnId;
            
        }
        
        class Client {

        public:
            Client()
            :
                _state(&Client::sizeMsb),
                _size(0),
                _totalBytes(0),
                _complete(false)
            {}
            
            using State = void (Client::*)(unsigned char);
            
            void processData(unsigned char *data, size_t len, const std::function<void()> &acknowledge) {
                
                while (len-- > 0 && _state != nullptr) {
                    
                    (this->*_state)(*data++);
                    
                    if (_state == &Client::data && _size == 0) {
                        acknowledge();
                        _state = &Client::sizeMsb;
                    }
                }
                
            }
            
            size_t totalBytes() const {
                return _totalBytes;
            }
            
            bool complete() const {
                return _complete;
            }
            
        protected:
            
            void sizeMsb(unsigned char data) {
            
                _size |= data << 8;
                
                _state = &Client::sizeLsb;
                
            }
            
            void sizeLsb(unsigned char data) {
                
                _size |= data;
                
                if (_size == 0) {
                    _complete = true;
                }
                
                _state = &Client::data;
                
            }
            
            void data(unsigned char data) {
                
                _totalBytes++;
                _size--;
                
            }
            
        private:
            
            State _state;
            size_t _size;
            
            size_t _totalBytes;
            bool _complete;
        };
        
        
        void receiveConnId() {
            
            Frame connect;
            
            try {
                
                connect.receive(_server, Frame::block::blocking);
                
            } catch (Exception &e) {
                
                if (e.errorCode() == EAGAIN) {
                    return;
                }
                
                throw;
        
            }
        
            _connectionId = connect.str();
            
            _state = &Server::receiveData;
            
        }
        
        void receiveData() {
            
            Frame data;
            
            try {
                
                data.receive(_server, Message::block::none);
                
            } catch (Exception &e) {
            
                if (e.errorCode() == EAGAIN) {
                    return;
                }
                
                throw;
                
            }
            
            auto found = _clients.find(_connectionId);
            
//            std::cout << "S:" << _connectionId << " " << data.size() << " " << data.str() << std::endl;
            
            if (data.size() == 0) {
                
                if (found == _clients.end()) {
                    
                    // new connection
                    
                    _clients.emplace(std::move(_connectionId), Client());
                    
                    if (_clients.size() > _maxConcurrentClients) {
                        _maxConcurrentClients = _clients.size();
                    }
                    
                } else {
                    
                    // disconnection
                    
//                    std::cout << "TotalBytes: " << found->second.totalBytes() << std::endl;
                    
                    if (found->second.complete()) {
                        _complete++;
                    }
                    
                    _clients.erase(found);
                    
                }
                
            } else {
                
                // received bytes
                
                found->second.processData(data.data<unsigned char>(), data.size(), [&] {
                
                    _connectionId.send(_server, Frame::block::blocking, Frame::more::more);
                    Frame("OK").send(_server, Frame::block::blocking, Frame::more::none);

                });
                
            }
            
            _state = &Server::receiveConnId;
        }
        
    private:
        
        Context _ctx;
        Socket _server;
        bool _done;
        State _state;
        size_t _complete;
        size_t _maxConcurrentClients;
        std::map<Frame, Client> _clients;
        
        Frame _connectionId;
        
    };
    
    class Client {
        
        
    public:
        Client(const Context &ctx, const size_t msgSize, const std::chrono::seconds &duration)
        :
            _ctx(ctx),
            _server(_ctx, Socket::Type::stream),
            _state(&Client::connecting),
            _sentBytes(0),
            _sentMessages(0),
            _duration(duration)
        {
            _data.resize(msgSize, '0');
        }
        
        using State = void (Client::*)();
        
        bool run() {

            if (_state != nullptr) {
                
                (this->*_state)();
                
            }
            
            return _state == nullptr ? true : false;
            
        }
        
        size_t sentBytes() const {
            return _sentBytes;
        }
        
        size_t sentMessages() const {
            return _sentMessages;
        }
        
    protected:
        
        void connecting() {
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
            _server.connect("tcp://127.0.0.1:5555");
            
            _state = &Client::receiveConnId;
            
            _until = std::chrono::high_resolution_clock::now() + _duration;

        }
        
        void receiveConnId() {

            _connectionId.receive(_server, Frame::block::blocking);

            if ( ! _connectionId.hasMore()) {
                throw;
            }
            
            // receive zero data frame;
            Frame nil;
            
            nil.receive(_server, Frame::block::blocking);
            
            if (nil.size() > 0) {
                throw ;
            }
            
            _state = &Client::sendData;
            
        }
        
        void sendData() {
            
            Frame data(_data.size()+2);
            *(data.data<unsigned char>()) = _data.size() >> 8;
            *(data.data<unsigned char>()+1) = _data.size() & 0xFF;
            memcpy(data.data<unsigned char>()+2, _data.data(), _data.size());
            
            _connectionId.send(_server, Frame::block::blocking, Frame::more::more);
            data.send(_server, Frame::block::blocking, Frame::more::none);
            
            _sentBytes += _data.size();
            _sentMessages++;
            
            _state = &Client::receiveOkay;
            
        }
        
        void receiveOkay() {

            try {
                
                _connectionId.receive(_server, Frame::block::blocking);
                
            } catch (Exception &e) {
                
                if (e.errorCode() == EAGAIN) {
                    return;
                }
                throw;
            }
            
            _state = &Client::terminate;
            
            Frame okay;
            
            okay.receive(_server, Frame::block::none);
            
            if (okay.size() == 0) {
                std::cout << "C: " << _connectionId << " - " << "Disconnect" << std::endl;
                return;
            }
            
            if (okay.str() != "OK") {
                std::cout << "C: Expected OK, got " << okay << std::endl;
                return;
            }

//            std::cout << "C: " << okay.envelope().front() << " - " << okay.front() << std::endl;
            
            if (std::chrono::high_resolution_clock::now() < _until) {
                
                _state = &Client::sendData;
                
            } else {
                _state = &Client::sendEnd;
            }
            
        }
        
        void sendEnd() {
            
            Frame size(2);
            *size.data<char>() = 0;
            *(size.data<char>()+1) = 0;

            _connectionId.send(_server, Frame::block::blocking, Frame::more::more);
            size.send(_server, Frame::block::blocking, Frame::more::none);
            
            _state = &Client::terminate;
            
        }
     
        void terminate() {
            
//            std::cout << "Client terminate" << std::endl;
            
            _state = nullptr;
            
        }
        
    private:
        
        Context _ctx;
        Socket _server;
        State _state;
        Frame _connectionId;
        size_t _sentBytes;
        size_t _sentMessages;
        std::string _data;
        std::chrono::high_resolution_clock::time_point _until;
        std::chrono::seconds _duration;
        
    };
    
    
    class Worker {
        
    public:
        
        void operator()() {
            
            while (_clients.size() > 0) {

                for (auto iter = _clients.begin(); iter != _clients.end(); iter++) {

                    if (iter->run()) {
                        
                        _completed.emplace_back(std::move(*iter));
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
        
        const std::vector<Client> &completed() const {
            return _completed;
        }
        
        void reset() {
            _clients.clear();
            _completed.clear();
        }
        
    private:
        
        std::vector<Client> _clients;
        std::vector<Client> _completed;
        
    };
    
    class Test {
        
    public:
        
        Test(const size_t numClients, const size_t msgSize, const std::chrono::seconds &duration, const size_t numWorkers, const size_t ioThreads)
        :
            _numClients(numClients),
            _msgSize(msgSize),
            _duration(duration),
            _numWorkers(numWorkers),
            _ioThreads(ioThreads)
        {}
        
        size_t operator()() {
            
            Context ctx;
            ctx.setIOThreads((int)_ioThreads);
            ctx.setMaxSockets(static_cast<int>(_numClients+1));
            
            auto server = Server(ctx);
            
            auto serverThread = std::thread(std::ref(server));
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
            std::vector<Worker> workers;
            workers.resize(_numWorkers);
            
            // Assign clients to workers
            
            for(unsigned i = 0 ; i < _numClients ; i++) {
                
                workers[i % workers.size()].push_back(Client(ctx, _msgSize, _duration));
                
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

            _totalBytes = _totalMessages = 0;
            
            for (auto &worker : workers) {
                
                for ( auto &client : worker.completed()) {
                    
//                    std::cout << "ClientBytes: " << client.sentBytes() << std::endl;
            
                    _totalBytes += client.sentBytes();
                    _totalMessages += client.sentMessages();
                    
                }

                worker.reset();
                
            }
            
            // short sleep to allow connections to close so server can count them as complete
            
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            // stop the server and wait for completion
            
            server.stop();
            serverThread.join();
            
            if (server.maxConcurrentClients() < _numClients) {
                std::cout << "Warning: MaxConcurrent = " << server.maxConcurrentClients() << std::endl;
            }
            
            return server.complete();
        }
        
        size_t totalBytes() const {
            return _totalBytes;
        }
        
        size_t totalMessages() const {
            return _totalMessages;
        }
        
    private:
        
        size_t _numClients;
        size_t _msgSize;
        std::chrono::seconds _duration;
        size_t _numWorkers;
        size_t _ioThreads;

        size_t _totalBytes;
        size_t _totalMessages;
        
    };

    struct Stat {
        size_t mps;
        double kbps;
    };
    
    describe(Peer, {
       
        std::chrono::seconds duration(3);   // Test duration (actually sending data, after setup)
        
        const size_t ioThreads = 2;         // Number of ZeroMQ threads
        const size_t numWorkers = 2;        // Number of client worker threads
        const size_t minClients = 1;        // Minimum number of clients
        const size_t maxClients = 1024;     // Maximum number of clients
        const size_t scaleClients = 2;      // Scale factor for clients
        const size_t minMsgSize = 8;        // Minimum message size
        const size_t maxMsgSize = 8192;     // Maximum message size
        const size_t scaleMsgSize = 2;      // Scale factor for message size
        
        std::map<size_t, std::map<size_t, struct Stat>> _clientStats;
        
        for(size_t clients=minClients ; clients <= maxClients ; clients *= scaleClients) {

            std::ostringstream clientContext;

            clientContext << clients << " clients";

            context(clientContext.str(), {

                for (size_t msgSize = minMsgSize ; msgSize <= maxMsgSize ; msgSize *= scaleMsgSize) {
                
                    std::ostringstream messageContext;

                    messageContext << msgSize << " bytes message";

                    context(messageContext.str(), {
                        
                        Test test(clients, msgSize, duration, numWorkers, ioThreads);
                        
                        auto complete = test();
                        
                        it("completes test", {
                            
                            expect(complete).should.equal(clients);
                            
                        });
                        
                        auto &stats = _clientStats[clients][msgSize];
                        stats.mps = test.totalMessages() / duration.count();
                        stats.kbps = ((double)test.totalBytes() / duration.count()) / 1024;

                    });

                }

            });
                    
        }

        if (_clientStats.size() > 0) {
            
            std::cout << "ioThreads = " << ioThreads << ", numWorkers = " << numWorkers << ", duration = " << duration.count() << std::endl;
            
            std::cout << "Messages Per Second" << std::endl << "-------------------------------" << std::endl;
            
            std::cout << "NumClients";
            
            for (size_t msgSize = minMsgSize ; msgSize <= maxMsgSize ; msgSize *= scaleMsgSize) {
                
                std::cout << ", " << msgSize;
                
            }
            
            std::cout << std::endl;
            
            for (auto &clientStat : _clientStats) {
                
                std::cout << clientStat.first;
                
                for( auto &msgStat : clientStat.second ) {
                    
                    std::cout << ", " << msgStat.second.mps;
                    
                }
                
                std::cout << std::endl;
                
            }
            
            std::cout << std::endl;
            
            std::cout << "Kilobytes Per Second" << std::endl << "-------------------------------" << std::endl;
            
            std::cout << "NumClients";
            
            for (size_t msgSize = minMsgSize ; msgSize <= maxMsgSize ; msgSize *= scaleMsgSize) {
                
                std::cout << ", " << msgSize;
                
            }
            
            std::cout << std::endl;
            
            for (auto &clientStat : _clientStats) {
                
                std::cout << clientStat.first;
                
                for( auto &msgStat : clientStat.second ) {
                    
                    std::cout << ", " << msgStat.second.kbps;
                    
                }
                
                std::cout << std::endl;
            }
            
            std::cout << std::endl << std::endl;
            
        }
    });
    
} }

