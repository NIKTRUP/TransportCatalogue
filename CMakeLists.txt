cmake_minimum_required(VERSION 3.10)

project(TransportCatalogue CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
    ./proto/transport_catalogue.proto
    ./proto/map_renderer.proto
    ./proto/svg.proto
    ./proto/transport_router.proto
    ./proto/graph.proto
)

file(GLOB SOURCES ./src/*.cpp)
file(GLOB INCLUDES ./include/*.h)
file(GLOB SVG ./svg/*.h ./svg/*.cpp)
file(GLOB JSON ./json/*.h ./json/*.cpp)

add_executable(
    transport_catalogue
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    main.cpp
    ${SOURCES}
    ${INCLUDES}
    ${SVG}
    ${JSON}
)

target_include_directories(transport_catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(transport_catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
string(REPLACE "protobuf.a" "protobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")

target_link_libraries(transport_catalogue $<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}> Threads::Threads)

