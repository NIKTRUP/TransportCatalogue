cmake_minimum_required(VERSION 3.24)
project(TransportCatalogue)

set(CMAKE_CXX_STANDARD 17)

add_executable(TransportCatalogue
        main.cpp

        include/domain.h
        include/geo.h
        include/log_duration.h
        include/map_renderer.h
        include/request_handler.h
        include/stat_reader.h
        include/test_framework.h
        include/tests.h
        include/transport_catalogue.h
        include/graph.h
        include/ranges.h
        include/router.h

        src/domain.cpp
        src/geo.cpp
        src/map_renderer.cpp
        src/request_handler.cpp
        src/stat_reader.cpp
        src/tests.cpp
        src/transport_catalogue.cpp

        svg/svg.h
        svg/svg.cpp

        json/json.h
        json/json.cpp
        json/json_builder.h
        json/json_builder.cpp
        src/transport_router.cpp include/transport_router.h)

target_link_libraries(TransportCatalogue -ltbb -lpthread)