#include "MapServiceImpl.h"
#include "SlamSystem.h"
#include <iostream>
#include <chrono>
#include <thread>

MapServiceImpl::MapServiceImpl(std::weak_ptr<SlamSystem> slam_weak)
    : slamWeak_(std::move(slam_weak)) {}

grpc::Status MapServiceImpl::GetMap(grpc::ServerContext* /*context*/,
                                    const robot::map::MapRequest* request,
                                    robot::map::MapResponse* response) {
    auto slamShared = slamWeak_.lock();
    if (!slamShared) {
        response->set_message("SlamSystem not available");
        return grpc::Status(grpc::StatusCode::UNAVAILABLE, "SlamSystem gone");
    }

    // Ask SlamSystem for map info (thread-safe)
    std::string info = slamShared->getCurrentMapInfo();
    response->set_message("MapID=" + request->map_id() + " info: " + info);
    std::cout << "[MapServiceImpl] GetMap served for " << request->map_id()
              << " -> " << info << std::endl;
    return grpc::Status::OK;
}

grpc::Status MapServiceImpl::StreamMap(grpc::ServerContext* context,
                                      const robot::map::MapRequest* request,
                                      grpc::ServerWriter<robot::map::MapChunk>* writer) {
    auto slamShared = slamWeak_.lock();
    if (!slamShared) {
        return grpc::Status(grpc::StatusCode::UNAVAILABLE, "SlamSystem gone");
    }

    std::cout << "[MapServiceImpl] StreamMap started for " << request->map_id() << std::endl;

    // We'll stream chunks produced by SlamSystem::getMapChunk(i).
    // The SlamSystem provides thread-safe access to chunks.
    for (int i = 0; ; ++i) {
        if (context->IsCancelled()) {
            std::cout << "[MapServiceImpl] client cancelled stream\n";
            return grpc::Status::OK;
        }

        std::string chunk_data;
        bool last = false;
        bool ok = slamShared->getMapChunk(i, chunk_data, last);
        if (!ok) {
            // no more chunks (or map not found)
            break;
        }

        robot::map::MapChunk chunk;
        chunk.set_data(chunk_data);
        chunk.set_chunk_id(i);
        chunk.set_last_chunk(last);
        if (!writer->Write(chunk)) {
            // client closed connection
            std::cout << "[MapServiceImpl] Write failed, client likely disconnected\n";
            return grpc::Status::OK;
        }

        if (last) break;

        // optional throttle
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::cout << "[MapServiceImpl] StreamMap finished for " << request->map_id() << std::endl;
    return grpc::Status::OK;
}
