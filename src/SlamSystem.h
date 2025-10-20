#pragma once
#include <memory>
#include <mutex>
#include <string>
#include <vector>

class GrpcServerManager;

class SlamSystem : public std::enable_shared_from_this<SlamSystem> {
public:
    SlamSystem();
    ~SlamSystem();

    // lifecycle
    void initialize(); // starts gRPC server
    void runLoop();    // do some fake SLAM work
    void shutdown();

    // called by MapServiceImpl in a thread-safe manner
    std::string getCurrentMapInfo(); // returns info string
    // get map chunk indexed by i; returns true if chunk available, last flag indicates final chunk
    bool getMapChunk(int index, std::string& out_chunk, bool& last);

private:
    std::unique_ptr<GrpcServerManager> grpcManager_;
    std::mutex mutex_;
    std::string currentMapInfo_;

    // For demo: pretend map split into chunks
    std::vector<std::string> mapChunks_;
};
