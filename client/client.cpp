#include <grpcpp/grpcpp.h>
#include "map_service.grpc.pb.h"
#include <iostream>

int main() {
    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    auto stub = robot::map::MapService::NewStub(channel);

    robot::map::MapRequest req;
    req.set_map_id("test_map");

    robot::map::MapResponse resp;
    grpc::ClientContext ctx;
    grpc::Status status = stub->GetMap(&ctx, req, &resp);
    if (status.ok()) {
        std::cout << "GetMap reply: " << resp.message() << std::endl;
    } else {
        std::cerr << "RPC failed: " << status.error_message() << std::endl;
    }

    // Test stream
    grpc::ClientContext ctx2;
    std::unique_ptr<grpc::ClientReader<robot::map::MapChunk>> reader(
        stub->StreamMap(&ctx2, req)
    );
    robot::map::MapChunk chunk;
    while (reader->Read(&chunk)) {
        std::cout << "Chunk " << chunk.chunk_id() << " size=" << chunk.data().size()
                  << (chunk.last_chunk() ? " (last)" : "") << std::endl;
    }
    grpc::Status s2 = reader->Finish();
    if (!s2.ok()) std::cerr << "Stream finished with error: " << s2.error_message() << std::endl;

    return 0;
}
