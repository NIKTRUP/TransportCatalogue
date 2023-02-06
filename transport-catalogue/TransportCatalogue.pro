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
        src/json.cpp \
        src/map_renderer.cpp \
        src/request_handler.cpp \
        src/stat_reader.cpp \
        src/svg.cpp \
        src/tests.cpp \
        src/transport_catalogue.cpp

HEADERS += \
    include/domain.h \
    include/geo.h \
    include/json.h \
    include/log_duration.h \
    include/map_renderer.h \
    include/request_handler.h \
    include/stat_reader.h \
    include/svg.h \
    include/test_framework.h \
    include/tests.h \
    include/transport_catalogue.h
