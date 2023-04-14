TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -ltbb \
        -lpthread

SOURCES += \
        main.cpp \
        src/domain.cpp \
        src/geo.cpp \
        json/json.cpp \
        json/json_builder.cpp \
        src/map_renderer.cpp \
        src/request_handler.cpp \
        src/stat_reader.cpp \
        svg/svg.cpp \
        src/tests.cpp \
        src/transport_catalogue.cpp

HEADERS += \
    include/domain.h \
    include/geo.h \
    json/json.h \
    json/json_builder.h \
    include/log_duration.h \
    include/map_renderer.h \
    include/request_handler.h \
    include/stat_reader.h \
    svg/svg.h \
    include/test_framework.h \
    include/tests.h \
    include/transport_catalogue.h
