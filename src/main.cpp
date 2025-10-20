#include "SlamSystem.h"
#include <iostream>
#include <memory>
#include <thread>

int main() {
    auto slam = std::make_shared<SlamSystem>();

    // initialize (this will call grpcManager_->start(shared_from_this()))
    slam->initialize();

    // Run slam logic in current thread (or you could spawn separate thread)
    slam->runLoop();

    // shutdown
    slam->shutdown();
    std::cout << "Exiting\n";
    return 0;
}
