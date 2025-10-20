#include "SlamSystem.h"
#include "GrpcServerManager.h"
#include <iostream>
#include <thread>
#include <chrono>

SlamSystem::SlamSystem() {
    grpcManager_ = std::make_unique<GrpcServerManager>();
    currentMapInfo_ = "initial_map_v0";
    // prepare some fake chunks
    mapChunks_.push_back("chunk0_data_abc");
    mapChunks_.push_back("chunk1_data_def");
    mapChunks_.push_back("chunk2_data_ghi");
}

SlamSystem::~SlamSystem() {
    shutdown();
}

void SlamSystem::initialize() {
    std::cout << "[SlamSystem] initialize\n";
    // pass shared_ptr to manager; owner is SlamSystem (this caller holds shared_ptr)
    grpcManager_->start("0.0.0.0:50051", shared_from_this());
}

void SlamSystem::runLoop() {
    // Do fake work updating map info
    for (int i = 0; i < 5; ++i) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            currentMapInfo_ = "map_version_" + std::to_string(i);
        }
        std::cout << "[SlamSystem] updated map to " << currentMapInfo_ << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void SlamSystem::shutdown() {
    std::cout << "[SlamSystem] shutdown\n";
    if (grpcManager_) grpcManager_->stop();
}

std::string SlamSystem::getCurrentMapInfo() {
    std::lock_guard<std::mutex> lock(mutex_);
    return currentMapInfo_;
}

bool SlamSystem::getMapChunk(int index, std::string& out_chunk, bool& last) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (index < 0 || index >= static_cast<int>(mapChunks_.size())) {
        return false;
    }
    out_chunk = mapChunks_[index];
    last = (index == static_cast<int>(mapChunks_.size()) - 1);
    return true;
}
