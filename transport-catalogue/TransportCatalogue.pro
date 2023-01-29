TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -ltbb \
        -lpthread

SOURCES += \
        main.cpp \
        src/input_reader.cpp \
        src/stat_reader.cpp \
        src/tests.cpp \
        src/transport_catalogue.cpp

HEADERS += \
    include/geo.h \
    include/input_reader.h \
    include/log_duration.h \
    include/stat_reader.h \
    include/test_framework.h \
    include/tests.h \
    include/transport_catalogue.h
