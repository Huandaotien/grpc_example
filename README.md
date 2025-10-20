# Install protobuf and gRPC libraries:
- Build and install proto3.
    ```
    sudo rm -rf /usr/local/include/google/protobuf
    sudo rm -f /usr/local/lib/libprotobuf*
    sudo rm -f /usr/local/bin/protoc
    sudo rm -rf /usr/local/lib/cmake/protobuf
    git clone https://github.com/protocolbuffers/protobuf.git
    cd protobuf
    git checkout v21.12
    mkdir build
    cd build
    cmake -G Ninja \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -Dprotobuf_BUILD_TESTS=OFF \
        ../cmake
    ninja
    sudo ninja install
    ```
- Build and install gRPC
    ```
    git clone -b v1.47.0 https://github.com/grpc/grpc.git
    cd grpc
    git submodule update --init --recursive
    mkdir -p cmake/build
    cd cmake/build

    cmake ../.. \
    -DCMAKE_BUILD_TYPE=Release \
    -DgRPC_INSTALL=ON \
    -DgRPC_BUILD_TESTS=OFF \
    -DgRPC_ABSL_PROVIDER=package \
    -DgRPC_PROTOBUF_PROVIDER=package \
    -DProtobuf_DIR=/usr/local/lib/cmake/protobuf \
    -DCMAKE_PREFIX_PATH="/usr/local/lib/cmake/absl;/usr/local/lib/cmake/protobuf"

    make -j$(nproc)
    sudo make install
    sudo ldconfig
    ```
    - Generate C++ files from .proto file:
    ```
    protoc -I=path-containt-proto-files \
        --cpp_out=path-to-save-cpp-files \
        --grpc_out=path-to-save-grpc-cpp-files \
        --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` \
        path-to-proto-files
    ```

# Generate cpp files from proto file:
    ```
    cd grpc_example

    protoc -I=proto \
    --cpp_out=src \
    --grpc_out=src \
    --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) \
    proto/map_service.proto
    ```

# Build:
    ```
    cd grpc_example

    mkdir -p build
    cd build
    cmake ..
    cmake --build . -j$(nproc)
    ```

# Run test:
    - Run server
    ```
    ./slam_server
    ```

    - Run client
    ```
    ./slam_client
    ```