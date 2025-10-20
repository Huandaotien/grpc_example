#include "GrpcServerManager.h"
#include "MapServiceImpl.h"
#include "SlamSystem.h"
#include <iostream>

GrpcServerManager::GrpcServerManager() : running_(false) {}

GrpcServerManager::~GrpcServerManager() {
    stop();
}

void GrpcServerManager::start(const std::string& address, std::shared_ptr<SlamSystem> slamShared) {
    if (running_) return;
    running_ = true;

    // Start server in a background thread. MapServiceImpl will use weak_ptr to SlamSystem.
    serverThread_ = std::thread([this, address, slamShared]() {
        // Create MapServiceImpl with weak_ptr
        auto slamWeak = std::weak_ptr<SlamSystem>(slamShared);
        MapServiceImpl mapService(slamWeak);

        grpc::ServerBuilder builder;
        builder.AddListeningPort(address, grpc::InsecureServerCredentials());
        builder.RegisterService(&mapService);

        server_ = builder.BuildAndStart();
        std::cout << "[GrpcServerManager] gRPC server listening on " << address << std::endl;

        // server_->Wait() blocks until Shutdown
        server_->Wait();

        std::cout << "[GrpcServerManager] server thread exiting\n";
    });
}

void GrpcServerManager::stop() {
    if (!running_) return;
    running_ = false;

    if (server_) {
        server_->Shutdown();
        server_->Wait();
    }
    if (serverThread_.joinable()) serverThread_.join();
}
