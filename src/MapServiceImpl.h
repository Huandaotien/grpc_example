#pragma once

#include "map_service.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <memory>

class SlamSystem; // forward

// MapServiceImpl holds only a weak_ptr to SlamSystem to avoid ownership cycles
class MapServiceImpl final : public robot::map::MapService::Service {
public:
    explicit MapServiceImpl(std::weak_ptr<SlamSystem> slam_weak);
    ~MapServiceImpl() override = default;

    grpc::Status GetMap(grpc::ServerContext* context,
                        const robot::map::MapRequest* request,
                        robot::map::MapResponse* response) override;

    grpc::Status StreamMap(grpc::ServerContext* context,
                           const robot::map::MapRequest* request,
                           grpc::ServerWriter<robot::map::MapChunk>* writer) override;

private:
    std::weak_ptr<SlamSystem> slamWeak_;
};
