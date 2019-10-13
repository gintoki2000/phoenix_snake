TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += sdl2

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += SDL2_image

unix: PKGCONFIG += SDL2_ttf

HEADERS += \
    textrenderer.h

SOURCES += \
    textrenderer.c\
    main.c


