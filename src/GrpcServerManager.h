#pragma once

#include <grpcpp/grpcpp.h>
#include <thread>
#include <memory>
#include <string>

class SlamSystem;

class GrpcServerManager {
public:
    GrpcServerManager();
    ~GrpcServerManager();

    // start server in background thread; slamShared is passed and stored as weak_ptr internally
    void start(const std::string& address, std::shared_ptr<SlamSystem> slamShared);
    void stop();

private:
    std::unique_ptr<grpc::Server> server_;
    std::thread serverThread_;
    bool running_;
};
