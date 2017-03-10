
C_OBS_ROOT=E:\\bitbrothers\\obs-studio

C_RELEASE_DEBUG=debug
INCLUDEPATH +=$${C_OBS_ROOT}/libobs

DEBUG_RELEASE_NAME=release
DEBUG_RELEASE_SUBFIX=
CONFIG(debug, debug|release) {
    DEBUG_RELEASE_NAME=debug
    DEBUG_RELEASE_SUBFIX=d
}

C_DEPENDENCIES_PATH=$${C_OBS_ROOT}/dependencies2013
C_LIB_PATH=$$PWD/$${DEBUG_RELEASE_NAME}/lib

LIBS += -luser32  -lkernel32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32
LIBS += -L$${C_LIB_PATH} -lobs -lobs-frontend-api  -llibff  -lfile-updater  -lobsglad  -lipc-util  -ljansson_d  -lw32-pthreads
LIBS += -L$${C_DEPENDENCIES_PATH}/win32/bin -llibcurl -lavcodec -lavfilter -lavdevice -lavutil -lswscale -lavformat -lswresample

INCLUDEPATH +=$${C_DEPENDENCIES_PATH}/win32/include
INCLUDEPATH +=$${C_OBS_ROOT}/deps/libff
INCLUDEPATH +=$${C_OBS_ROOT}/UI/obs-frontend-api
INCLUDEPATH +=$${C_OBS_ROOT}/deps/libff/libff


