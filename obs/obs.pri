
C_OBS_ROOT=C:\\bitbrothers\\obs-studio

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
INCLUDEPATH +=$$PWD/UI

HEADERS += \
$$PWD/UI//adv-audio-control.hpp \
$$PWD/UI//audio-encoders.hpp \
$$PWD/UI//crash-report.hpp \
$$PWD/UI//display-helpers.hpp \
$$PWD/UI//double-slider.hpp \
$$PWD/UI//focus-list.hpp \
$$PWD/UI//hotkey-edit.hpp \
$$PWD/UI//item-widget-helpers.hpp \
$$PWD/UI//menu-button.hpp \
$$PWD/UI//mute-checkbox.hpp \
$$PWD/UI//obs-app.hpp \
$$PWD/UI//platform.hpp \
$$PWD/UI//properties-view.hpp \
$$PWD/UI//properties-view.moc.hpp \
$$PWD/UI//qt-display.hpp \
$$PWD/UI//qt-wrappers.hpp \
$$PWD/UI//remote-text.hpp \
$$PWD/UI//slider-absoluteset-style.hpp \
$$PWD/UI//source-label.hpp \
$$PWD/UI//source-list-widget.hpp \
$$PWD/UI//vertical-scroll-area.hpp \
$$PWD/UI//visibility-checkbox.hpp \
$$PWD/UI//visibility-item-widget.hpp \
$$PWD/UI//volume-control.hpp \
$$PWD/UI//window-basic-adv-audio.hpp \
$$PWD/UI//window-basic-filters.hpp \
$$PWD/UI//window-basic-interaction.hpp \
$$PWD/UI//window-basic-main.hpp \
$$PWD/UI//window-basic-main-outputs.hpp \
$$PWD/UI//window-basic-preview.hpp \
$$PWD/UI//window-basic-properties.hpp \
$$PWD/UI//window-basic-settings.hpp \
$$PWD/UI//window-basic-source-select.hpp \
$$PWD/UI//window-basic-status-bar.hpp \
$$PWD/UI//window-basic-transform.hpp \
$$PWD/UI//window-license-agreement.hpp \
$$PWD/UI//window-log-reply.hpp \
$$PWD/UI//window-main.hpp \
$$PWD/UI//window-namedialog.hpp \
$$PWD/UI//window-projector.hpp \
$$PWD/UI//window-remux.hpp \
    cobspublic.h

SOURCES += \
$$PWD/UI//adv-audio-control.cpp \
$$PWD/UI//api-interface.cpp \
$$PWD/UI//audio-encoders.cpp \
$$PWD/UI//crash-report.cpp \
$$PWD/UI//double-slider.cpp \
$$PWD/UI//focus-list.cpp \
$$PWD/UI//hotkey-edit.cpp \
$$PWD/UI//item-widget-helpers.cpp \
$$PWD/UI//menu-button.cpp \
$$PWD/UI//obs-app.cpp \
$$PWD/UI//platform-windows.cpp \
#$$PWD/UI//platform-x11.cpp \
$$PWD/UI//properties-view.cpp \
$$PWD/UI//qt-display.cpp \
$$PWD/UI//qt-wrappers.cpp \
$$PWD/UI//remote-text.cpp \
$$PWD/UI//slider-absoluteset-style.cpp \
$$PWD/UI//source-label.cpp \
$$PWD/UI//source-list-widget.cpp \
$$PWD/UI//vertical-scroll-area.cpp \
$$PWD/UI//visibility-checkbox.cpp \
$$PWD/UI//visibility-item-widget.cpp \
$$PWD/UI//volume-control.cpp \
$$PWD/UI//window-basic-adv-audio.cpp \
$$PWD/UI//window-basic-filters.cpp \
$$PWD/UI//window-basic-interaction.cpp \
$$PWD/UI//window-basic-main.cpp \
$$PWD/UI//window-basic-main-dropfiles.cpp \
$$PWD/UI//window-basic-main-outputs.cpp \
$$PWD/UI//window-basic-main-profiles.cpp \
$$PWD/UI//window-basic-main-scene-collections.cpp \
$$PWD/UI//window-basic-main-transitions.cpp \
$$PWD/UI//window-basic-preview.cpp \
$$PWD/UI//window-basic-properties.cpp \
$$PWD/UI//window-basic-settings.cpp \
$$PWD/UI//window-basic-source-select.cpp \
$$PWD/UI//window-basic-status-bar.cpp \
$$PWD/UI//window-basic-transform.cpp \
$$PWD/UI//window-license-agreement.cpp \
$$PWD/UI//window-log-reply.cpp \
$$PWD/UI//window-namedialog.cpp \
$$PWD/UI//window-projector.cpp \
$$PWD/UI//window-remux.cpp

RESOURCES += \
$$PWD/UI//forms/obs.qrc

FORMS += \
$$PWD/UI//forms/NameDialog.ui \
$$PWD/UI//forms/OBSBasic.ui \
$$PWD/UI//forms/OBSBasicFilters.ui \
$$PWD/UI//forms/OBSBasicInteraction.ui \
$$PWD/UI//forms/OBSBasicSettings.ui \
$$PWD/UI//forms/OBSBasicSourceSelect.ui \
$$PWD/UI//forms/OBSBasicTransform.ui \
$$PWD/UI//forms/OBSLicenseAgreement.ui \
$$PWD/UI//forms/OBSLogReply.ui \
$$PWD/UI//forms/OBSRemux.ui

DISTFILES += \
$$PWD/UI//obs.rc
