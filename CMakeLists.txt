cmake_minimum_required(VERSION 3.24)

project(TransportCatalogue)
set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
        proto/transport_catalogue.proto
        proto/svg.proto
        proto/map_renderer.proto
        proto/graph.proto
        proto/transport_router.proto)

set(FILES_INCLUDE include/domain.h
        include/geo.h
        include/log_duration.h
        include/map_renderer.h
        include/request_handler.h
        include/json_reader.h
        include/test_framework.h
        include/tests.h
        include/transport_catalogue.h
        include/graph.h
        include/ranges.h
        include/router.h
        include/transport_router.h
        include/serialization.h)

set(FILES_SOURCE src/domain.cpp
        src/geo.cpp
        src/map_renderer.cpp
        src/request_handler.cpp
        src/json_reader.cpp
        src/tests.cpp
        src/transport_catalogue.cpp
        src/transport_router.cpp
        src/serialization.cpp)

set(FILES_JSON json/json.h
        json/json.cpp
        json/json_builder.h
        json/json_builder.cpp)

set(FILES_SVG svg/svg.h
        svg/svg.cpp)

set(PROJECT_NAME TransportCatalogue)

add_executable(${PROJECT_NAME} main.cpp ${PROTO_SRCS} ${PROTO_HDRS} ${FILES_JSON} ${FILES_SVG} ${FILES_INCLUDE} ${FILES_SOURCE})

target_include_directories(${PROJECT_NAME} PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
string(REPLACE "protobuf.a" "protobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")

target_link_libraries(${PROJECT_NAME} "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>" Threads::Threads)
target_link_libraries(${PROJECT_NAME} -ltbb -lpthread)