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